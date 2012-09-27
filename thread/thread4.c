#include <linux/kernel.h>

#include <linux/module.h>

#include <linux/init.h>

#include <linux/param.h>

#include <linux/jiffies.h>

#include <asm/system.h>

#include <asm/processor.h>

#include <asm/signal.h>

#include <linux/completion.h>       // for DECLARE_COMPLETION()

#include <linux/sched.h>            

#include <linux/delay.h>            // mdelay()

#include <linux/kthread.h> 

 

MODULE_LICENSE("GPL");

 
static DECLARE_COMPLETION(my_completion);

 
static struct task_struct *task;

 
int flag = 0;

 
int my_fuction(void *arg)
{

    printk(" in %s()\n", __FUNCTION__);

    allow_signal(SIGKILL); //使得线程可以接收SIGKILL信号

    mdelay(2000);

    printk(" my_function complete()\n");

    printk("should stop: %d\n",kthread_should_stop());

    while (!signal_pending(current) && !kthread_should_stop()) {//使得线程可以可以被杀死，也可以再rmmod的时候结束

        printk(" jiffies is %lu\n", jiffies);

        set_current_state(TASK_INTERRUPTIBLE);

        schedule_timeout(HZ * 5);   

         printk("should stop: %d\n",kthread_should_stop());

    }

    printk("Leaving my_function\n");

    flag = 1; //flag很关键！

    return 0;

}

 

static int __init init(void)
{

    task = kthread_run(my_fuction,NULL,"my_function");

    printk("<1> init wait_for_completion()\n");

    return 0;

}

 

static void __exit finish(void)
{        

        int ret;

        if(!flag)
        {

                 if (!IS_ERR(task))
                 {  

                      int ret = kthread_stop(task);  

                      printk(KERN_INFO "First thread function has stopped ,return %d\n", ret);  

                 }                  

       }

    

    printk("task_struct: 0x%x",task);

    printk(" Goodbye\n");

}

 

module_init(init);

module_exit(finish);
