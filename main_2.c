#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>

//FOR IOCTL
#define MAGIC_NUMBER 'Z'
#define TIMER_START _IO(MAGIC_NUMBER,1)
#define TIMER_STOP _IO(MAGIC_NUMBER,2)
#define TIMER_SET _IOW(MAGIC_NUMBER,3,int)

//FOR COUNTING TOKENS
#define TOTAL_TOKENS 100
#define NO_OF_SENDERS 6


struct token
{
unsigned int id;
unsigned int BEFORE_WRITE;
unsigned int ENQUEUED;
unsigned int DEQUEUED;
unsigned int RECEIVED;
char msg[80];
struct token *NEXT;	//self referential pointer for linked list implementation
};

int token_id=0;
//pthread_mutex_t token;	//not required
//time_t generate;			//not required
int total_sent=0,total_received=0;

struct Sender			//to pass arguments to Sender threads
{
int timer;
int driver;
int total_sent;
};

struct Receiver
{
int timer;
int driver1;
int driver2;
int total_received;
};

void *send_tokens(void *arguments)
{
void generate_msg(struct token *);
int nap_time;
int driver=-1;
srand(time(NULL));
struct Sender *sender=(struct Sender *)arguments;
int read_status;
struct token *s;

	while(sender->total_sent<TOTAL_TOKENS)
	{
	s=malloc(sizeof(struct token));


		s->id=token_id;
		generate_msg(s);
		s->BEFORE_WRITE=read(sender->timer,NULL,0);
		s->ENQUEUED=0;
		s->DEQUEUED=0;
		s->RECEIVED=0;

	read_status=write(sender->driver,s,0);
			
			if(read_status==0)
			{
			printf("==============  SENT TOKEN ===============\n");
			printf("ID : %d\n",s->id);
			printf("BEFORE WRITE : %d\n",s->BEFORE_WRITE);
			printf("ENQUEUED : %d\n",s->ENQUEUED);
			printf("DEQUEUED : %d\n",s->DEQUEUED);
			printf("RECEIVED : %d\n",s->RECEIVED);
			printf("MESSAGE : %s\n",s->msg);
			printf("**********************************************\n");

			token_id++;
			sender->total_sent++;
			}
			else
			{
			printf("SEND FAILED...QUEUE MIGHT BE FULL\n");
			}

	free(s);


	nap_time=(((int)lrand48()%10)+1)*1000;


	usleep(nap_time);
	}

pthread_exit(NULL);
}

void generate_msg(struct token *temp)
{

char d;
int length=-1;
int i,index;

for(i=0;i<80;i++)
temp->msg[i]='\0';

	while(length<10)
	length=rand()%80;
	
	index=0;

		while(index<length)
		{
		d=rand()%26+(rand()%2==0?65:97);	//either capital or small
		temp->msg[index++]=d;
		}


}

void *receive_tokens(void *arguments)
{
int nap_time;
struct token *s;
char *buff;
int result;
struct Receiver *receiver=(struct Receiver *)arguments;

	while(receiver->total_received<TOTAL_TOKENS*NO_OF_SENDERS)
	{
	s=malloc(sizeof(struct token));

		result=read(receiver->driver1,s,1);

			if(result==-1)
			result=read(receiver->driver2,s,1);
			
			if(result!=-1)
			{
			s->RECEIVED=read(receiver->timer,NULL,0);

			printf("============== RECEIVED TOKEN ===============\n");
			printf("ID : %d\n",s->id);
			printf("BEFORE WRITE : %d\n",s->BEFORE_WRITE);
			printf("ENQUEUED : %d\n",s->ENQUEUED);
			printf("DEQUEUED : %d\n",s->DEQUEUED);
			printf("RECEIVED : %d\n",s->RECEIVED);
			printf("MESSAGE : %s\n",s->msg);
			printf("**********************************************\n");
			receiver->total_received++;
			}
			else
			printf("QUEUE EMPTY...Switching to another queue\n");

	free(s);
	nap_time=(((int)lrand48()%10)+1)*1000;


	usleep(nap_time);
	}

pthread_exit(NULL);
}

int main(int argc,char argv[])
{
int i;

int timer=open("/dev/HRT",O_RDWR);		//drivers opened only once
int d1=open("/dev/SQueue1",O_RDWR);
int d2=open("/dev/SQueue2",O_RDWR);

	if(d1==-1||d2==-1||timer==-1)
	{
	printf("COULD NOT OPEN SOME DEVICES...EXITING\n");
	return 0;
	}

				ioctl(timer,TIMER_START);
				ioctl(timer,TIMER_SET,10);

srand(time(NULL));	//set seed
token_id=0;
//token_id=rand();

	pthread_t Sender[6];
	pthread_t Receiver;

struct Sender *s[NO_OF_SENDERS];	//arguments for threads
struct Receiver *r1;

for(i=0;i<NO_OF_SENDERS;i++)
{
s[i]=malloc(sizeof(struct Sender));
s[i]->total_sent=0;
s[i]->timer=timer;

	if(i<3)
	s[i]->driver=d1;
	else
	s[i]->driver=d2;
}

r1=malloc(sizeof(struct Receiver));


r1->total_received=0;
r1->driver1=d1;
r1->driver2=d2;
r1->timer=timer;

//	pthread_mutex_init(&token,NULL);

	for(i=0;i<6;i++)
	{
		if(i<3)
		pthread_create(&Sender[i],NULL,send_tokens,s[i]);
		else
		pthread_create(&Sender[i],NULL,send_tokens,s[i]);

	usleep(5000);
	}
		pthread_create(&Receiver,NULL,receive_tokens,r1);

//		sleep();

		for(i=0;i<6;i++)
		pthread_join(Sender[i],NULL);
		pthread_join(Receiver,NULL);

				ioctl(timer,TIMER_STOP);

for(i=0;i<6;i++)
free(s[i]);

free(r1);

return 0;
}

