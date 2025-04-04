#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
信号量（Semaphore）
信号量是一种用于控制多个线程之间共享资源的机制，它通过维护一个计数器来跟踪可用资源的数量。信号量分为两种：
    无名信号量（unnamed semaphore）：使用 sem_init 初始化，线程之间共享。
    命名信号量（named semaphore）：通过 sem_open 来创建一个可以跨进程使用的信号量。
常见的函数：
    sem_init(sem_t *sem, int pshared, unsigned int value)：初始化无名信号量，pshared 指定是否在线程之间共享。
    sem_wait(sem_t *sem)：等待信号量，如果信号量的值大于 0，则减少 1；如果为 0，则阻塞等待。
    sem_post(sem_t *sem)：释放信号量，增加信号量的值，可能会唤醒等待的线程。
    sem_destroy(sem_t *sem)：销毁信号量。
用法场景：
    控制 **一定数量的资源** ，比如限制只能有 **N** 个线程访问某个共享资源。e.g.: 控制一个线程池中的最大并发线程数。
    可以在多个线程之间共享资源，也可以跨进程使用命名信号量。

notes:
当信号量的计数设为 1 时，它在功能上与互斥锁（mutex）类似，但是还有区别：
    1. 当信号量计数为 1 时，表示一次只允许一个线程访问共享资源。
       其他线程在等待信号量时将会被阻塞，直到当前持有信号量的线程释放信号量。
    2. mutex具有拥有权，即加锁的线程必须是解锁的线程。如果一个线程获取了互斥锁，只有这个线程才能解锁。不支持递归，需要递归锁；
       信号量没有拥有权，任何线程都可以释放信号量。信号量也不支持递归锁定。
    3. 互斥锁在处理线程同步时更轻量化，通常比信号量的开销更小。
    4. mutex只能用于线程间同步。信号量可以跨进程。
*/

/*
typedef union
{
  char __size[__SIZEOF_SEM_T];
  long int __align;
} sem_t;
 */
sem_t semaphore;

void *thread_func(void *arg)
{
    long id = (long)arg;
    sem_wait(&semaphore);  // 等待信号量
    printf("Thread[%ld] is running\n", id);
    sleep(1);
    sem_post(&semaphore);  // 释放信号量
    printf("Thread[%ld] finish\n", id);
    return NULL;
}

int main()
{
    pthread_t threads[5];
    sem_init(&semaphore, 0, 2);  // 初始化信号量，初始值为 2，线程共享

    for (long i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, thread_func, (void *)i);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore);  // 销毁信号量
    return 0;
}

/*
Thread[0] is running
Thread[1] is running
Thread[0] finish
Thread[1] finish
Thread[2] is running
Thread[3] is running
Thread[2] finish
Thread[3] finish
Thread[4] is running
Thread[4] finish
*/