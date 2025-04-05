#include <string.h>
#include "comm.h"

int main()
{
    shared_data_t* shared = open_ipc_memory(IPC_CREATE_NEW);
    if (!shared) {
        return 1;
    }

    // 创建信号量
    ipc_sem_t sems = open_semaphores(IPC_CREATE_NEW);
    if (!sems.writer_sem || !sems.reader_sem) {
        close_ipc_memory(shared);
        return 1;
    }

    init_shared_memory(shared);
    char buffer[MAX_BUF];

    while (1) {
        printf("Enter message (type 'quit' to exit): ");
        if (fgets(buffer, MAX_BUF, stdin) == NULL)
            break;

        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "quit") == 0)
            break;

        // 等待写入许可
        sem_wait(sems.writer_sem);

        // 写入数据
        size_t len = strlen(buffer) + 1;
        memcpy(shared->data, buffer, len);
        shared->size = len;
        shared->seq++;

        printf("Message written: %s (seq: %u)\n", buffer, shared->seq);

        // 通知读取者
        sem_post(sems.reader_sem);
    }

    close_semaphores(&sems);
    close_ipc_memory(shared);
    cleanup_semaphores();  // 清理信号量
    return 0;
}
