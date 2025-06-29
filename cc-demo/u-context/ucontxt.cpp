#include <iostream>
#include <vector>
#include <functional>
#include <cstdint>
#include <memory>
#include <utility>

#include <ucontext.h> // 引入ucontext.h头文件，提供用户级上下文切换相关的数据结构和API

constexpr int STACK_SIZE = 128 * 1024;

class Scheduler;
static Scheduler* g_scheduler = nullptr;

struct Fiber {
    ucontext_t context; // ucontext_t结构体，保存协程的CPU上下文（寄存器、栈等信息）
    char* stack = nullptr;
    bool finished = false;
    std::function<void()> task;

    Fiber(std::function<void()> func) :
        task(std::move(func))
    {
        stack = new char[STACK_SIZE];
    }

    ~Fiber() { delete[] stack; }

    Fiber(const Fiber&) = delete;
    Fiber& operator=(const Fiber&) = delete;
    Fiber(Fiber&&) = delete;
    Fiber& operator=(Fiber&&) = delete;
};

class Scheduler {
private:
    std::vector<std::unique_ptr<Fiber>> fibers_;
    ucontext_t main_context_; // 主调度器的上下文，用于协程切换回来
    int current_fiber_index_ = -1;

public:
    Scheduler() { g_scheduler = this; }

    /**
     * @brief 创建一个新的协程（fiber），并将其加入调度器。
     *
     * @param func 协程要执行的任务。注意，这里的func是std::function<void()>类型，
     *        它不仅仅是一个8字节的函数指针，还可能包含捕获的上下文、lambda对象等，
     *        其内部结构可能远大于一个简单的函数地址。因此，为了避免不必要的拷贝，
     *        提高效率，使用std::move将func转为右值引用，将其内容转移到新创建的Fiber对象中。
     */
    void spawn(std::function<void()> func)
    {
        // 使用std::move将func转为右值引用，避免不必要的拷贝，提高效率
        auto new_fiber = std::make_unique<Fiber>(std::move(func));

        // 获取当前上下文，初始化new_fiber->context
        getcontext(&new_fiber->context); // getcontext会将当前CPU上下文信息保存到context结构体中

        // 设置协程的栈空间
        new_fiber->context.uc_stack.ss_sp = new_fiber->stack;
        new_fiber->context.uc_stack.ss_size = STACK_SIZE;
        new_fiber->context.uc_link = &main_context_; // 协程结束后返回到主调度器上下文

        // makecontext用于修改context，使其切换时执行fiber_entry函数
        uintptr_t task_ptr = reinterpret_cast<uintptr_t>(&new_fiber->task);
        makecontext(&new_fiber->context,
                    (void (*)())fiber_entry,
                    2,
                    static_cast<unsigned int>(task_ptr),
                    static_cast<unsigned int>(task_ptr >> 32));
        // makecontext会设置context，使其切换到该上下文时执行fiber_entry函数，并传递参数

        fibers_.push_back(std::move(new_fiber));
    }

    void run()
    {
        while (true) {
            bool all_finished = true;
            for (size_t i = 0; i < fibers_.size(); ++i) {
                if (fibers_[i] && !fibers_[i]->finished) {
                    all_finished = false;
                    current_fiber_index_ = i;
                    // swapcontext用于保存当前上下文到main_context_，并切换到fiber的上下文
                    swapcontext(&main_context_, &fibers_[i]->context); // 切换到协程
                }
            }
            if (all_finished) {
                break;
            }
        }
    }

    void yield()
    {
        int current_fiber = current_fiber_index_;
        // 协程主动让出CPU，保存当前上下文到fiber->context，切换回主调度器上下文
        swapcontext(&fibers_[current_fiber]->context, &main_context_);
    }

private:
    // 协程入口函数，makecontext会设置切换到协程时执行此函数
    static void fiber_entry(unsigned int task_ptr_low, unsigned int task_ptr_high)
    {
        uintptr_t task_ptr = (static_cast<uintptr_t>(task_ptr_high) << 32) | task_ptr_low;
        std::function<void()>* func = reinterpret_cast<std::function<void()>*>(task_ptr);

        (*func)();

        g_scheduler->fibers_[g_scheduler->current_fiber_index_]->finished = true;
    }
};

void task1()
{
    std::cout << "Task 1: started" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "Task 1: is running, step " << i << std::endl;
        g_scheduler->yield();
    }
    std::cout << "Task 1: finished" << std::endl;
}

void task2()
{
    std::cout << "Task 2: started" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "Task 2: is running, step " << i << std::endl;
        g_scheduler->yield();
    }
    std::cout << "Task 2: finished" << std::endl;
}

int main()
{
    Scheduler scheduler;

    scheduler.spawn(task1);
    scheduler.spawn(task2);

    std::cout << "Scheduler starting..." << std::endl;
    scheduler.run();
    std::cout << "Scheduler finished." << std::endl;

    return 0;
}
