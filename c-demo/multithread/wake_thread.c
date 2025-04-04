#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 共享数据
int shared_data = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

// 线程1: 等待数据准备好
void *thread1_func(void *arg) {
    pthread_mutex_lock(&mutex);
    printf("thread1: get mutex lock\n");

	// 这里 while 是为了避免OS虚假唤醒对应thread
    while (shared_data == 0) {
        // 等待条件变量被触发
		// 会阻塞在cond，同时释放mutex互斥锁
        printf("thread1: will wait on cond\n");
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Thread1: Data is ready, shared_data = %d\n", shared_data);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// 线程2: 更新共享数据并唤醒线程1
void *thread2_func(void *arg) {
    sleep(1);  // 模拟一些计算或工作
    pthread_mutex_lock(&mutex);
    shared_data = 42;  // 更新共享数据
    printf("Thread2: Data updated to %d\n", shared_data);
    // 通知等待中的线程
    pthread_cond_signal(&cond);
    printf("Thread2: pthread_cond_signal over.\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // 初始化互斥锁和条件变量
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // 创建线程
    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);

    // 等待线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // 销毁互斥锁和条件变量
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

/** run output:
thread1: get mutex lock
thread1: will wait on cond
Thread2: Data updated to 42
Thread2: pthread_cond_signal over.
Thread1: Data is ready, shared_data = 42
*/
