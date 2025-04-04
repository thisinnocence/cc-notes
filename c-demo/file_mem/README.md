# 基于 mmap 的IPC

## 技术原理

| 机制 | 描述 |
|------|------|
| Memory Mapping | 使用 mmap 将文件或匿名区域映射到进程的虚拟地址空间，实现高效的文件 I/O 或共享内存 |
| 页表管理 | 内核通过页表管理进程的虚拟地址映射，并确保 MAP_SHARED 映射的多进程间数据一致性 |
| Zero-copy | 通过 mmap 直接访问内核页缓存，避免用户态与内核态之间的数据拷贝，提高 I/O 效率 |
| Page Cache | 依赖内核的页缓存机制，加速文件读取，减少磁盘 I/O |

## 核心API

### API说明

```c
/*
 * 将文件映射到内存空间
 * @param addr    建议的映射地址，通常设为NULL让系统自动选择
 * @param length  要映射的字节数，必须是页大小的整数倍
 * @param prot    内存保护标志，如PROT_READ|PROT_WRITE
 * @param flags   映射标志，如MAP_SHARED|MAP_PRIVATE
 * @param fd      要映射的文件描述符，如使用MAP_ANONYMOUS则设为-1
 * @param offset  文件偏移量，必须是页大小的整数倍
 * @return        成功返回映射的内存地址，失败返回MAP_FAILED
 * @errno         ENOMEM(内存不足)、EACCES(权限不足)、EINVAL(参数错误)
 */
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

/*
 * 解除内存映射
 * @param addr    mmap返回的内存地址
 * @param length  映射的字节数，必须与mmap时相同
 * @return        成功返回0，失败返回-1
 * @errno         EINVAL(参数无效)
 */
int munmap(void *addr, size_t length);

/*
 * 同步内存内容到文件
 * @param addr    要同步的内存地址
 * @param length  要同步的字节数
 * @param flags   同步标志(MS_SYNC立即同步/MS_ASYNC异步同步/MS_INVALIDATE丢弃缓存)
 * @return        成功返回0，失败返回-1
 * @errno         EIO(I/O错误)、EINVAL(参数错误)
 */
int msync(void *addr, size_t length, int flags);

/*
 * 锁定内存页，防止被换出
 * @param addr    要锁定的内存地址，必须页对齐
 * @param len     要锁定的字节数，自动扩展到页大小
 * @return        成功返回0，失败返回-1
 * @errno         ENOMEM(超出可锁定内存限制)、EPERM(权限不足)
 */
int mlock(const void *addr, size_t len);

/*
 * 修改内存映射的保护属性
 * @param addr    要修改的内存地址，必须页对齐
 * @param len     要修改的字节数，会自动扩展为页大小
 * @param prot    新的保护属性，如PROT_READ|PROT_WRITE|PROT_EXEC
 * @return        成功返回0，失败返回-1
 * @errno         EACCES(权限不足)、ENOMEM(超出进程地址空间)
 */
int mprotect(void *addr, size_t len, int prot);
```

### 内存保护标志

```c
// 内存保护标志定义
#define PROT_NONE  0x0    /* 页面不可访问 */
#define PROT_READ  0x1    /* 允许读取 */
#define PROT_WRITE 0x2    /* 允许写入 */
#define PROT_EXEC  0x4    /* 允许执行 */

/**
 * 常见组合:
 * PROT_READ | PROT_WRITE       // 读写权限，适用于数据段、堆、栈等
 * PROT_READ | PROT_EXEC        // 代码段权限 (受DEP/No-Execute保护影响)
 * PROT_NONE                    // 隔离内存区域，防止越界访问
 */
```

### 内存映射标志

```c
// 映射标志定义
#define MAP_SHARED    0x01     /* 共享映射，修改对其他进程可见 */
#define MAP_PRIVATE   0x02     /* 私有映射，写入时复制(Copy-on-Write)，修改不共享 */
#define MAP_FIXED     0x10     /* 精确指定映射地址，若地址被占用则映射失败 */
#define MAP_ANONYMOUS 0x20     /* 匿名映射，不依赖文件，fd 必须设为 -1 */
#define MAP_LOCKED    0x2000   /* 尝试锁定页面，防止换出 (需要 CAP_IPC_LOCK 权限) */

// 常见的组合有：

/* 共享内存方案 (需确保文件已创建) */
fd = open("file", O_RDWR);
addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (addr == MAP_FAILED) {
    perror("mmap failed");
}

/* 私有匿名内存 (适用于进程间通信) */
addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
if (addr == MAP_FAILED) {
    perror("mmap failed");
}

/* 只读文件映射 (共享访问) */
fd = open("file", O_RDONLY);
addr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
if (addr == MAP_FAILED) {
    perror("mmap failed");
}
```

##  性能优化

### 优化策略

| 策略 | 实现方式 | 效果 |
|------|----------|------|
| **内存锁定** | `MAP_LOCKED` / `mlock` | 尝试锁定物理内存，减少换出，提高实时性 |
| **进程间内存同步** | `msync` / `shm_open` + `shmctl` | 确保多进程间数据一致性 |
| **自旋锁优化** | `PAUSE` (x86) / `YIELD` (ARM) | 减少总线争用，降低高负载时功耗 |
| **页面对齐** | `posix_memalign()` / `mmap` | 降低 TLB 失效，提高大块数据访问效率 |

### 常见问题与解决方案

#### **1. 内存泄漏**
- **问题：** 进程结束时未释放 `mmap` 或共享内存，导致泄漏。
- **解决方案：** 使用 `munmap()` 或 `shmdt()` 及时释放，或注册 `atexit()` 进行清理。
- **示例：**  
  ```c
  void cleanup_mappings() {
      munmap(addr, size);
  }
  atexit(cleanup_mappings);
  ```

#### **2. 进程间一致性**
- **问题：** 多进程共享内存时，可能出现数据不一致。
- **解决方案：** 
  - 使用 `msync()` 确保修改的共享内存被同步到物理页。
  - 使用 `__sync_synchronize()` 或 `std::atomic_thread_fence()` 作为**内存屏障**，确保 CPU 访问顺序。
- **示例：**  
  ```c
  msync(addr, size, MS_SYNC);  // 立即同步到文件或共享内存
  __sync_synchronize();        // 内存屏障，确保访问顺序
  ```

#### **3. 并发访问**
- **问题：** 多线程或多进程访问共享数据时可能发生竞争条件 (Race Condition)。
- **解决方案：** 
  - **使用原子操作** (`__atomic_*` 或 `std::atomic<>`)，保证修改的原子性。
  - **使用互斥锁** (`pthread_mutex_t`) 保护关键区域，避免竞争。
- **示例：**
  ```c
  __atomic_store_n(&flag, 1, __ATOMIC_SEQ_CST);  // C 原子操作
  ```
  ```cpp
  std::atomic<int> flag{0};
  flag.store(1, std::memory_order_seq_cst); // C++ 原子操作
  ```

#### **4. 文件状态检查**
- **问题：** `mmap()` 需要确保映射文件存在且大小足够，否则可能导致访问越界。
- **解决方案：** 在 `mmap()` 之前使用 `fstat()` 检查文件大小。
- **示例：**
  ```c
  struct stat st;
  if (fstat(fd, &st) == -1 || st.st_size < size) {
      perror("File too small for mmap");
      exit(EXIT_FAILURE);
  }
  ```

## 5. 应用场景

### 5.1 最佳实践

| 场景 | 推荐方案 | 原因 |
|------|----------|------|
| 大数据共享 | Memory Mapping | Zero-copy优势明显 |
| 小数据传输 | Pipe/消息队列 | 开销更小 |
| 持久化需求 | Memory Mapping | 自带持久化 |
| 临时通信 | Unix Domain Socket | 更灵活 |

### 5.2 应用举例

| 软件 | 应用场景 | 实现方式 |
|------|----------|----------|
| PostgreSQL | 缓冲区管理 | 文件映射 |
| Nginx | 静态文件服务 | sendfile+mmap |
| Chrome | V8内存管理 | 匿名映射 |
| Redis | AOF文件处理 | 文件映射 |

## 6 调试工具

| 工具 | 用途 | 示例命令 |
|------|------|----------|
| pmap | 查看进程映射 | `pmap -x <pid>` |
| strace | 跟踪系统调用 | `strace -e mmap` |
| /proc | 查看内存映射 | `cat /proc/<pid>/maps` |
