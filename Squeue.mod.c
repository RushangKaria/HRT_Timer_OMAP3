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
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x9d669763, "memcpy" },
	{ 0x37a0cba, "kfree" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x9c0bd51f, "_raw_spin_lock" },
	{ 0x8414a4fb, "timer" },
	{ 0xe3cc56ec, "kmalloc_caches" },
	{ 0x2e8df7be, "omap_dm_timer_read_counter" },
	{ 0xe914e41e, "strcpy" },
	{ 0x8dd2508a, "kmem_cache_alloc_trace" },
	{ 0x504e95e7, "cdev_add" },
	{ 0x389150a0, "cdev_init" },
	{ 0x4d2d1411, "device_create" },
	{ 0xa22b0b52, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x27e1a049, "printk" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x87d7889b, "class_destroy" },
	{ 0xac57d803, "device_destroy" },
	{ 0xc7a88455, "cdev_del" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=HRT";


MODULE_INFO(srcversion, "D0F1F275EE67F8085CAE7DE");
