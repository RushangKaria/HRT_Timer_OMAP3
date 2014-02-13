#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4a3dbd89, "module_layout" },
	{ 0xc7a88455, "cdev_del" },
	{ 0x389150a0, "cdev_init" },
	{ 0x2e8df7be, "omap_dm_timer_read_counter" },
	{ 0xbe35faa9, "omap_dm_timer_start" },
	{ 0xac57d803, "device_destroy" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x77f46d95, "omap_dm_timer_write_counter" },
	{ 0x27e1a049, "printk" },
	{ 0x4d2d1411, "device_create" },
	{ 0x32b7cd57, "omap_dm_timer_request" },
	{ 0x504e95e7, "cdev_add" },
	{ 0x83cace74, "omap_dm_timer_set_source" },
	{ 0x87d7889b, "class_destroy" },
	{ 0x9787adf8, "omap_dm_timer_stop" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xa22b0b52, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "ECCA63A559D877072164F48");
