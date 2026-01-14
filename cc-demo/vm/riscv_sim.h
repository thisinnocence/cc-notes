#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Options {
  // 输入 ELF 路径
  std::string elf_path;
  // 内存基址与大小（默认模拟 0x80000000 起的 DDR）
  uint64_t base;
  uint64_t mem_size;
  // 最大执行步数（防止死循环）
  uint64_t max_steps;
  // 可选的停机地址
  bool has_halt;
  uint64_t halt_pc;
};

Options ParseArgs(int argc, char** argv);

class RiscvSim {
 public:
  RiscvSim(uint64_t base, std::vector<uint8_t> mem);
  void Run(uint64_t entry, const Options& opt);

 private:
  uint32_t Fetch32(uint64_t addr);
  uint64_t Load(uint64_t addr, unsigned size, bool is_signed);
  void Store(uint64_t addr, uint64_t value, unsigned size);
  void CheckAlign(uint64_t addr, unsigned align, const char* op);
  uint64_t AddrToOff(uint64_t addr);
  void Step(uint32_t inst);
  [[noreturn]] void Illegal(uint32_t inst);

  uint64_t base_;
  std::vector<uint8_t> mem_;
  std::vector<uint64_t> regs_;
  uint64_t pc_;
};

