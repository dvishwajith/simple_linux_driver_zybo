#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0xcd71858e, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa6c92a68, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xfa78f0e3, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x138cda43, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x5419e3dc, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0x2072ee9b, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x962b781d, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xaca3a98a, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x41b892fa, __VMLINUX_SYMBOL_STR(pid_task) },
	{ 0x30a47ef7, __VMLINUX_SYMBOL_STR(find_pid_ns) },
	{ 0x2cec70a8, __VMLINUX_SYMBOL_STR(init_pid_ns) },
	{ 0x436c2179, __VMLINUX_SYMBOL_STR(iowrite32) },
	{ 0x877c688d, __VMLINUX_SYMBOL_STR(send_sig_info) },
	{ 0xe484e35f, __VMLINUX_SYMBOL_STR(ioread32) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B493C29AF172ADA10390290");
