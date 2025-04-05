#include "comm.h"

int main()
{
    shared_data_t* shared = open_ipc_memory(IPC_OPEN_EXISTING);
    if (!shared) {
        return 1;
    }

    // 打开已存在的信号量
    ipc_sem_t sems = open_semaphores(IPC_OPEN_EXISTING);
    if (!sems.writer_sem || !sems.reader_sem) {
        close_ipc_memory(shared);
        return 1;
    }

    printf("Waiting for messages...\n");

    while (1) {
        // 等待数据就绪
        sem_wait(sems.reader_sem);

        // 读取数据
        printf("Message received: %s (seq: %u)\n", shared->data, shared->seq);

        // 允许写入新数据
        sem_post(sems.writer_sem);
    }

    close_semaphores(&sems);
    close_ipc_memory(shared);
    return 0;
}
