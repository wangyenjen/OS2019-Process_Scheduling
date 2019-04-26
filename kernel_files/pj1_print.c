// syscall #351

#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_pj1_print(int pid, long start_time, long end_time) {
    static const long BASE = 1000000000;
    printk(KERN_INFO "[Project1] %d %ld.%09ld %ld.%09ld", pid, start_time / BASE, start_time % BASE, end_time / BASE, end_time % BASE);
}
