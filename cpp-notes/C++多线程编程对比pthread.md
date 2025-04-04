# C++多线程编程对比pthread

多线程编程是现代软件开发中的重要部分。C++标准库自C++11起引入了`<thread>`库，而在此之前，开发者通常使用POSIX线程（pthread）进行多线程编程。本文将对比C++标准库的多线程API与pthread的API设计，并结合代码示例进行说明。

## API设计对比

### 1. 线程创建与管理

- **C++标准库**：C++提供了`std::thread`类，线程的创建和管理更加面向对象，代码简洁且易于维护。
- **pthread**：pthread提供了`pthread_create`函数，基于C语言风格，使用函数指针作为线程入口，代码较为繁琐。

#### 示例代码

**C++标准库：**

```cpp
#include <iostream>
#include <thread>

void threadFunction() {
    std::cout << "Hello from C++ thread!" << std::endl;
}

int main() {
    std::thread t(threadFunction); // 创建线程
    t.join(); // 等待线程完成
    return 0;
}
```

**pthread：**

```cpp
#include <iostream>
#include <pthread.h>

void* threadFunction(void* arg) {
    std::cout << "Hello from pthread!" << std::endl;
    return nullptr;
}

int main() {
    pthread_t t;
    pthread_create(&t, nullptr, threadFunction, nullptr); // 创建线程
    pthread_join(t, nullptr); // 等待线程完成
    return 0;
}
```

### 2. 线程同步

- **C++标准库**：提供了`std::mutex`和`std::lock_guard`等工具，简化了锁的使用，减少了死锁的风险。
- **pthread**：提供了`pthread_mutex_t`，需要手动初始化和销毁，使用时容易出错。

#### 示例代码

**C++标准库：**

```cpp
#include <iostream>
#include <thread>
#include <mutex>

// 定义一个全局的互斥锁
std::mutex mtx;

// 打印消息的函数，使用std::lock_guard自动管理锁
void printMessage(const std::string& message) {
    // std::lock_guard在构造时会自动加锁
    std::lock_guard<std::mutex> lock(mtx); 
    // 临界区：只有一个线程可以进入
    std::cout << message << std::endl;
    // std::lock_guard在析构时会自动解锁（作用域结束时）
}

int main() {
    // 创建两个线程，分别调用printMessage函数
    std::thread t1(printMessage, "Hello from thread 1");
    std::thread t2(printMessage, "Hello from thread 2");

    // 等待两个线程执行完毕
    t1.join();
    t2.join();

    return 0;
}
```

**pthread：**

```cpp
#include <iostream>
#include <pthread.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* printMessage(void* arg) {
    const char* message = static_cast<const char*>(arg);
    pthread_mutex_lock(&mtx); // 手动加锁
    std::cout << message << std::endl;
    pthread_mutex_unlock(&mtx); // 手动解锁
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, printMessage, (void*)"Hello from thread 1");
    pthread_create(&t2, nullptr, printMessage, (void*)"Hello from thread 2");

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_mutex_destroy(&mtx); // 手动销毁锁
    return 0;
}
```

### 3. 条件变量

- **C++标准库**：提供了`std::condition_variable`，结合`std::unique_lock`使用，接口更现代化。
- **pthread**：提供了`pthread_cond_t`，需要手动初始化和销毁，使用较为复杂。

#### 示例代码

**C++标准库：**

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable condition; // 条件变量，用于线程间的同步
bool ready = false;

// 工作线程函数
void worker() {
    // 创建一个 unique_lock 并锁定互斥锁
    std::unique_lock<std::mutex> lock(mtx);

    // 等待条件变量，直到 ready 为 true
    // wait 会自动释放锁并挂起线程，直到条件满足时重新加锁并继续执行
    condition.wait(lock, [] { return ready; });

    // 当条件满足时，线程被唤醒并继续执行
    std::cout << "Worker thread is running!" << std::endl;

    // unique_lock 在作用域结束时会自动解锁
}

int main() {
    // 创建一个线程，运行 worker 函数
    std::thread t(worker);

    {
        // 使用 lock_guard 自动管理锁的生命周期
        // lock_guard 在构造时加锁，在作用域结束时自动解锁
        std::lock_guard<std::mutex> lock(mtx);

        // 修改全局标志，表示条件已满足
        ready = true;
    } // lock_guard 在此处作用域结束，自动解锁互斥锁

    // 通知一个等待线程（worker 线程），条件已满足
    condition.notify_one();

    // 等待线程 t 执行完毕
    t.join();

    return 0;
}
```

**pthread：**

```cpp
#include <iostream>
#include <pthread.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool ready = false;

void* worker(void* arg) {
    pthread_mutex_lock(&mtx);
    // 条件变量的等待通常需要在循环中检查条件
    // 避免虚假唤醒（spurious wakeup） 和 条件竞争 的问题
    //    1. 虚假唤醒是指线程在没有任何线程调用 pthread_cond_signal 或 pthread_cond_broadcast 的情况下
    //       从 pthread_cond_wait 中返回。这种情况虽然很少发生，但在某些平台和实现中是可能的。
    //    2.  条件竞争是指多个线程在访问共享资源时，由于线程调度的顺序不同，可能导致程序行为不一致的情况。
    while (!ready) {
        pthread_cond_wait(&cond, &mtx); // 等待条件满足
    }
    pthread_mutex_unlock(&mtx);
    std::cout << "Worker thread is running!" << std::endl;
    return nullptr;
}

int main() {
    pthread_t t;
    pthread_create(&t, nullptr, worker, nullptr);

    pthread_mutex_lock(&mtx);
    ready = true;
    pthread_cond_signal(&cond); // 通知线程
    pthread_mutex_unlock(&mtx);

    pthread_join(t, nullptr);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
    return 0;
}
```

## 总结

| 特性            | C++标准库                | pthread                  |
|-----------------|-------------------------|--------------------------|
| 线程创建与管理   | 面向对象，简单易用       | 基于C语言，较为繁琐       |
| 线程同步         | 自动管理锁，安全性更高   | 手动管理锁，易出错       |
| 条件变量         | 接口现代化，易于使用     | 接口复杂，需手动管理      |

C++标准库的多线程API设计更加现代化，适合C++开发者使用，而pthread更适合需要跨平台兼容的C语言项目。根据项目需求选择合适的工具是关键。
