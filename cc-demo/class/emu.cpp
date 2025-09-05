// Simplified PV emulator

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

// Opcodes
enum Opcode : uint8_t {
    kOpNop = 0x00,
    kOpLoadImm = 0x10,  // load immediate into acc_
    kOpStore = 0x11,    // store acc_ to memory address
    kOpAddImm = 0x20,   // acc_ += imm
    kOpSubImm = 0x21,   // acc_ -= imm
    kOpJmp = 0x30,      // pc = addr
    kOpHalt = 0xFF,     // halt
    kOpIret = 0xFE,     // return from interrupt (pop pc)
    kOpPrint = 0xF0     // debug: print value at addr
};

// IMemory interface
class IMemory {
  public:
    virtual ~IMemory() = default;
    virtual uint8_t ReadByte(uint32_t addr) = 0;
    virtual void WriteByte(uint32_t addr, uint8_t value) = 0;
    virtual uint32_t Read32(uint32_t addr) = 0;
    virtual void Write32(uint32_t addr, uint32_t value) = 0;
};

// Simple vector-backed memory implementation
class Memory : public IMemory {
  public:
    explicit Memory(uint32_t size) : size_(size), data_(size, 0) {}

    uint8_t ReadByte(uint32_t addr) override
    {
        if (addr >= size_) return 0;
        return data_[addr];
    }

    void WriteByte(uint32_t addr, uint8_t value) override
    {
        if (addr >= size_) return;
        data_[addr] = value;
    }

    uint32_t Read32(uint32_t addr) override
    {
        if (addr + 4 > size_) return 0;
        uint32_t v = 0;
        std::memcpy(&v, &data_[addr], sizeof(v));
        return v;
    }

    void Write32(uint32_t addr, uint32_t value) override
    {
        if (addr + 4 > size_) return;
        std::memcpy(&data_[addr], &value, sizeof(value));
    }

    uint32_t size() const { return size_; }

  private:
    uint32_t size_;
    std::vector<uint8_t> data_;
};

// IRQ: manages pending (IP) and enable (IE) bits for multiple IRQs
class InterruptController {
  public:
    explicit InterruptController(uint32_t irq_count)
        : pending_(irq_count, false), enabled_(irq_count, false)
    {
    }

    void RequestInterrupt(uint32_t irq_id)
    {
        if (irq_id < pending_.size()) pending_[irq_id] = true;
    }

    void EnableInterrupt(uint32_t irq_id)
    {
        if (irq_id < enabled_.size()) enabled_[irq_id] = true;
    }

    void DisableInterrupt(uint32_t irq_id)
    {
        if (irq_id < enabled_.size()) enabled_[irq_id] = false;
    }

    // Return the lowest-numbered pending+enabled interrupt, or -1 if none.
    int32_t GetPendingEnabledInterrupt()
    {
        for (uint32_t i = 0; i < pending_.size(); ++i) {
            if (pending_[i] && enabled_[i]) return static_cast<int32_t>(i);
        }
        return -1;
    }

    void ClearPending(uint32_t irq_id)
    {
        if (irq_id < pending_.size()) pending_[irq_id] = false;
    }

  private:
    std::vector<bool> pending_;
    std::vector<bool> enabled_;
};

// Timer: counts ticks and requests an interrupt when period elapses
class Timer {
  public:
    Timer() : period_ticks_(0), irq_id_(0), ic_ptr_(nullptr), tick_count_(0) {}

    Timer(uint32_t period_ticks, uint32_t irq_id, InterruptController& ic)
        : period_ticks_(period_ticks), irq_id_(irq_id), ic_ptr_(&ic), tick_count_(0)
    {
    }

    void Tick()
    {
        if (!ic_ptr_) return;
        ++tick_count_;
        if (tick_count_ >= period_ticks_) {
            tick_count_ = 0;
            // 触发中断
            ic_ptr_->RequestInterrupt(irq_id_);
        }
    }

  private:
    uint32_t period_ticks_;
    uint32_t irq_id_;
    InterruptController* ic_ptr_;
    uint32_t tick_count_;
};

// DMA: performs a blocking transfer in one call
class Dma {
  public:
    void Transfer(IMemory& mem, uint32_t src, uint32_t dst, uint32_t len)
    {
        for (uint32_t i = 0; i < len; ++i) {
            uint8_t b = mem.ReadByte(src + i);
            mem.WriteByte(dst + i, b);
        }
    }
};

// CPU: simple PV CPU that executes one instruction per Step().
class Cpu {
  public:
    Cpu(IMemory& mem, InterruptController& ic)
        : mem_(mem), ic_(ic), pc_(0), sp_(0), acc_(0), halted_(false), trace_(false)
    {
    }

    void Initialize(uint32_t pc, uint32_t sp)
    {
        pc_ = pc;
        sp_ = sp;
    }

    bool IsHalted() const { return halted_; }

    void Step()
    {
        // 在每条指令执行前检查中断控制器
        int32_t maybe_irq = ic_.GetPendingEnabledInterrupt();
        if (maybe_irq != -1) {
            uint32_t irq = static_cast<uint32_t>(maybe_irq);
            if (trace_) std::cout << "[CPU] IRQ " << irq << " taken at pc=" << pc_ << '\n';
            // 保存 PC 到栈
            Push32(pc_);
            // 跳转到中断向量地址 (假设 irq_id * 4)
            pc_ = irq * 4;
            // 清除对应中断挂起位
            ic_.ClearPending(irq);
            // continue to execute the handler instruction at new PC
        }

        uint8_t opcode = mem_.ReadByte(pc_);

        switch (opcode) {
            case kOpNop: {
                if (trace_) std::cout << "[CPU] EXEC NOP pc=" << pc_ << '\n';
                ++pc_;
                break;
            }
            case kOpLoadImm: {
                // 功能模拟: 将接下来的 4 字节立即数加载到 acc_
                uint32_t imm = mem_.Read32(pc_ + 1);
                acc_ = imm;
                if (trace_)
                    std::cout << "[CPU] EXEC LOAD_IMM pc=" << pc_ << " imm=" << imm
                              << " -> acc=" << acc_ << '\n';
                pc_ += 1 + 4;
                break;
            }
            case kOpStore: {
                // 存储 acc_ 到指定地址
                uint32_t addr = mem_.Read32(pc_ + 1);
                mem_.Write32(addr, acc_);
                if (trace_)
                    std::cout << "[CPU] EXEC STORE pc=" << pc_ << " addr=" << addr
                              << " value=" << acc_ << '\n';
                pc_ += 1 + 4;
                break;
            }
            case kOpAddImm: {
                uint32_t imm = mem_.Read32(pc_ + 1);
                acc_ += imm;
                if (trace_)
                    std::cout << "[CPU] EXEC ADD_IMM pc=" << pc_ << " imm=" << imm
                              << " -> acc=" << acc_ << '\n';
                pc_ += 1 + 4;
                break;
            }
            case kOpSubImm: {
                uint32_t imm = mem_.Read32(pc_ + 1);
                acc_ -= imm;
                if (trace_)
                    std::cout << "[CPU] EXEC SUB_IMM pc=" << pc_ << " imm=" << imm
                              << " -> acc=" << acc_ << '\n';
                pc_ += 1 + 4;
                break;
            }
            case kOpJmp: {
                uint32_t addr = mem_.Read32(pc_ + 1);
                if (trace_) std::cout << "[CPU] EXEC JMP pc=" << pc_ << " -> " << addr << '\n';
                pc_ = addr;
                break;
            }
            case kOpPrint: {
                uint32_t addr = mem_.Read32(pc_ + 1);
                uint32_t v = mem_.Read32(addr);
                std::cout << "[CPU] PRINT mem[" << addr << "] = " << v << '\n';
                if (trace_) std::cout << "[CPU] EXEC PRINT pc=" << pc_ << " addr=" << addr << '\n';
                pc_ += 1 + 4;
                break;
            }
            case kOpIret: {
                // 弹出 PC 并返回
                uint32_t ret = Pop32();
                if (trace_) std::cout << "[CPU] EXEC IRET returning to pc=" << ret << '\n';
                pc_ = ret;
                break;
            }
            case kOpHalt: {
                if (trace_) std::cout << "[CPU] EXEC HALT pc=" << pc_ << '\n';
                halted_ = true;
                ++pc_;
                break;
            }
            default: {
                std::cerr << "[CPU] Unknown opcode 0x" << std::hex << int(opcode)
                          << " at pc=" << std::dec << pc_ << '\n';
                halted_ = true;
                break;
            }
        }
    }

    // Push/pop helpers (32-bit)
    void Push32(uint32_t value)
    {
        // stack grows down
        sp_ -= 4;
        mem_.Write32(sp_, value);
    }

    void SetTrace(bool enable) { trace_ = enable; }

    uint32_t Pop32()
    {
        uint32_t v = mem_.Read32(sp_);
        sp_ += 4;
        return v;
    }

  private:
    IMemory& mem_;
    InterruptController& ic_;
    uint32_t pc_;
    uint32_t sp_;
    uint32_t acc_;  // accumulator / single general register
    bool halted_;
    bool trace_;
};

// Top-level emulator
class Emulator {
  public:
    // Non-copyable and non-movable: values bind lifetime to Emulator.
    Emulator(const Emulator&) = delete;
    Emulator& operator=(const Emulator&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(Emulator&&) = delete;

    Emulator(uint32_t memory_size, uint32_t irq_count)
        : mem_(memory_size),
          ic_(irq_count),
          cpu_(mem_, ic_),
          dma_(),
          timer_enabled_(false),
          timer_(0, 0, ic_)
    {
    }

    void CreateTimer(uint32_t period_ticks, uint32_t irq_id)
    {
        timer_ = Timer(period_ticks, irq_id, ic_);
        timer_enabled_ = true;
    }

    void LoadProgram(const std::vector<uint8_t>& image, uint32_t load_addr)
    {
        for (size_t i = 0; i < image.size(); ++i) mem_.WriteByte(load_addr + i, image[i]);
    }

    void InitializeCpu(uint32_t pc, uint32_t sp) { cpu_.Initialize(pc, sp); }

    void EnableCpuTrace(bool enable) { cpu_.SetTrace(enable); }

    void Run(uint32_t max_cycles = 100000)
    {
        ic_.EnableInterrupt(1);  // enable irq 1 for demo; user can change as needed
        uint32_t cycles = 0;
        while (!cpu_.IsHalted() && cycles < max_cycles) {
            if (timer_enabled_) timer_.Tick();
            cpu_.Step();
            ++cycles;
        }
        if (cycles >= max_cycles) std::cerr << "[Emulator] Reached max cycles\n";
    }

    IMemory& memory() { return mem_; }
    Dma& dma() { return dma_; }
    InterruptController& interrupt_controller() { return ic_; }

  private:
    Memory mem_;
    InterruptController ic_;
    Cpu cpu_;
    Dma dma_;
    bool timer_enabled_;
    Timer timer_;
};

// (no helper functions needed)

int main()
{
    constexpr uint32_t kMemSize = 1024;
    constexpr uint32_t kStackTop = kMemSize;  // stack grows down from top

    Emulator emu(kMemSize, 8);  // 8 IRQs available

    // Create a timer that triggers IRQ 1 every 5 ticks
    emu.CreateTimer(5, 1);

    // Enable CPU trace for demo
    emu.EnableCpuTrace(true);

    // Build a small memory image.
    // Layout:
    // 0x0000 - interrupt vector / handler for IRQ 1 (at addr 4)
    // 0x0064 (100) - main program

    std::vector<uint8_t> image(kMemSize, 0);

    // IRQ 1 handler at address 4: LOAD_IMM 123; STORE 200; IRET
    uint32_t handler_addr = 4;
    uint32_t p = handler_addr;
    image[p++] = kOpLoadImm;
    // imm 123
    for (int i = 0; i < 4; ++i) image[p++] = static_cast<uint8_t>((123 >> (8 * i)) & 0xFF);
    image[p++] = kOpStore;
    uint32_t store_addr = 200;
    for (int i = 0; i < 4; ++i) image[p++] = static_cast<uint8_t>((store_addr >> (8 * i)) & 0xFF);
    image[p++] = kOpIret;

    // Main program at address 100: exercise all instructions
    uint32_t main_addr = 100;
    uint32_t q = main_addr;
    // NOP
    image[q++] = kOpNop;
    // LOAD_IMM 10
    image[q++] = kOpLoadImm;
    for (int i = 0; i < 4; ++i) image[q++] = static_cast<uint8_t>((10 >> (8 * i)) & 0xFF);
    // ADD_IMM 5
    image[q++] = kOpAddImm;
    for (int i = 0; i < 4; ++i) image[q++] = static_cast<uint8_t>((5 >> (8 * i)) & 0xFF);
    // SUB_IMM 3
    image[q++] = kOpSubImm;
    for (int i = 0; i < 4; ++i) image[q++] = static_cast<uint8_t>((3 >> (8 * i)) & 0xFF);
    // STORE to store_addr
    image[q++] = kOpStore;
    for (int i = 0; i < 4; ++i) image[q++] = static_cast<uint8_t>((store_addr >> (8 * i)) & 0xFF);
    // PRINT mem[store_addr]
    image[q++] = kOpPrint;
    for (int i = 0; i < 4; ++i) image[q++] = static_cast<uint8_t>((store_addr >> (8 * i)) & 0xFF);
    // JMP to HALT (place HALT at addr after)
    uint32_t halt_addr = q + 1 + 4;  // after JMP instruction
    image[q++] = kOpJmp;
    for (int i = 0; i < 4; ++i) image[q++] = static_cast<uint8_t>((halt_addr >> (8 * i)) & 0xFF);
    // HALT
    image[q++] = kOpHalt;

    // HALT after some prints (not strictly necessary)
    // Place HALT later but our loop will keep running; we'll rely on max_cycles
    // guard.

    // Load image into memory
    emu.LoadProgram(image, 0);

    // Initialize CPU: PC = main_addr, SP = stack top
    emu.InitializeCpu(main_addr, kStackTop);

    std::cout << "[Main] Starting emulator. Timer will request IRQ 1 every 5 ticks.\n";
    emu.Run(200);

    // After run, inspect memory at store_addr
    uint32_t result = emu.memory().Read32(store_addr);
    std::cout << "[Main] mem[" << store_addr << "] = " << result
              << " (expected 123 if handler ran)\n";

    return 0;
}
