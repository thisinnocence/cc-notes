#ifndef IPC_COMMON_H
#define IPC_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

// 在POSIX有名信号量中，名称以 '/' 开头是一个规范要求
//    有名信号量通常存储在 shm 目录下, OS管理
//    ls -l /dev/shm/sem.*
#define SEM_WRITER_NAME "/writer_sem"  // 写入者信号量名称
#define SEM_READER_NAME "/reader_sem"  // 读取者信号量名称
#define IPC_BACKEND_FILE "ipc_file"
#define MAX_BUF 256
#define FILE_SIZE 4096

typedef struct {
    char data[MAX_BUF];
    uint32_t size;
    uint32_t seq;
} shared_data_t;

typedef enum { IPC_OPEN_EXISTING = 0, IPC_CREATE_NEW = 1 } ipc_open_mode_t;

// 信号量句柄
typedef struct {
    sem_t* writer_sem;  // 控制写入访问的信号量
    sem_t* reader_sem;  // 控制读取访问的信号量
} ipc_sem_t;

// 打开或创建信号量
static inline ipc_sem_t open_semaphores(ipc_open_mode_t mode)
{
    ipc_sem_t sems = {NULL, NULL};

    if (mode == IPC_CREATE_NEW) {
        // 创建信号量，初始writer=1(可写入)，reader=0(等待数据)
        sems.writer_sem = sem_open(SEM_WRITER_NAME, O_CREAT, 0644, 1);
        sems.reader_sem = sem_open(SEM_READER_NAME, O_CREAT, 0644, 0);
    } else {
        // 打开已存在的信号量
        sems.writer_sem = sem_open(SEM_WRITER_NAME, 0);
        sems.reader_sem = sem_open(SEM_READER_NAME, 0);
    }

    if (sems.writer_sem == SEM_FAILED || sems.reader_sem == SEM_FAILED) {
        perror("sem_open");
        if (sems.writer_sem != SEM_FAILED)
            sem_close(sems.writer_sem);
        if (sems.reader_sem != SEM_FAILED)
            sem_close(sems.reader_sem);
        sems.writer_sem = sems.reader_sem = NULL;
    }

    return sems;
}

static inline shared_data_t* open_ipc_memory(ipc_open_mode_t mode)
{
    int fd;
    int flags = O_RDWR;
    if (mode == IPC_CREATE_NEW) {
        flags |= O_CREAT;
    }

    fd = open(IPC_BACKEND_FILE, flags, 0644);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    // ftruncate 用于将打开的文件截断到指定的长度（FILE_SIZE）。
    //   - 如果文件比这个长度长，则多余的部分会被删除；
    //   - 如果文件比这个长度短，则会扩展文件。
    if (mode == IPC_CREATE_NEW && ftruncate(fd, FILE_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        return NULL;
    }

    // mmap 用于将文件映射到内存中，返回映射后的内存地址。
    //   - MAP_LOCKED 锁定映射的内存区域，防止被交换到磁盘，实时性高。
    //   - MAP_SHARED 共享映射，对映射区的写入会反映到文件中。
    shared_data_t* shared = (shared_data_t*)
        mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);

    // map后可以关闭fd
    close(fd);

    if (shared == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return shared;
}

static inline void init_shared_memory(shared_data_t* shared)
{
    shared->seq = 0;
    shared->size = 0;
}

static inline void close_ipc_memory(shared_data_t* shared)
{
    if (shared) {
        munmap(shared, FILE_SIZE);
    }
}

// 关闭信号量
static inline void close_semaphores(ipc_sem_t* sems)
{
    if (sems->writer_sem)
        sem_close(sems->writer_sem);
    if (sems->reader_sem)
        sem_close(sems->reader_sem);
}

// 清理信号量（仅在最后一个进程退出时调用）
static inline void cleanup_semaphores()
{
    sem_unlink(SEM_WRITER_NAME);
    sem_unlink(SEM_READER_NAME);
}

#endif  // IPC_COMMON_H
