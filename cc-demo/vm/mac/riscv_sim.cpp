#include "riscv_sim.h"

#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr uint64_t kDefaultBase = 0x80000000ULL;
constexpr uint64_t kDefaultMemSize = 64 * 1024;
constexpr uint64_t kDefaultMaxSteps = 5'000'000;

[[noreturn]] void Die(const std::string& msg) {
  std::cerr << "error: " << msg << "\n";
  std::exit(1);
}

uint64_t ParseU64(const std::string& s) {
  std::size_t idx = 0;
  uint64_t v = std::stoull(s, &idx, 0);
  if (idx != s.size()) {
    Die("invalid number: " + s);
  }
  return v;
}

int64_t SignExtend(uint64_t val, unsigned bits) {
  const uint64_t shift = 64 - bits;
  return static_cast<int64_t>(val << shift) >> shift;
}

}  // namespace

Options ParseArgs(int argc, char** argv) {
  Options opt;
  opt.base = kDefaultBase;
  opt.mem_size = kDefaultMemSize;
  opt.max_steps = kDefaultMaxSteps;
  opt.has_halt = false;
  opt.halt_pc = 0;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("--base=", 0) == 0) {
      opt.base = ParseU64(arg.substr(7));
    } else if (arg.rfind("--mem=", 0) == 0) {
      opt.mem_size = ParseU64(arg.substr(6));
    } else if (arg.rfind("--max-steps=", 0) == 0) {
      opt.max_steps = ParseU64(arg.substr(12));
    } else if (arg.rfind("--halt=", 0) == 0) {
      opt.halt_pc = ParseU64(arg.substr(7));
      opt.has_halt = true;
    } else if (arg == "--help") {
      std::cout << "usage: riscv_sim <elf> [--base=0x80000000] [--mem=65536]"
                   " [--max-steps=5000000] [--halt=0x...]\n";
      std::exit(0);
    } else if (arg[0] == '-') {
      Die("unknown option: " + arg);
    } else if (opt.elf_path.empty()) {
      opt.elf_path = arg;
    } else {
      Die("unexpected argument: " + arg);
    }
  }
  if (opt.elf_path.empty()) {
    Die("missing ELF path (use --help)");
  }
  return opt;
}

RiscvSim::RiscvSim(uint64_t base, std::vector<uint8_t> mem)
    : base_(base), mem_(std::move(mem)), regs_(32, 0), pc_(0) {}

void RiscvSim::Run(uint64_t entry, const Options& opt) {
  pc_ = entry;
  for (uint64_t step = 0; step < opt.max_steps; ++step) {
    // 可选：到达指定地址就停机
    if (opt.has_halt && pc_ == opt.halt_pc) {
      std::cout << "halt: pc=0x" << std::hex << pc_ << std::dec << "\n";
      return;
    }
    uint32_t inst = Fetch32(pc_);
    Step(inst);
  }
  Die("max steps reached");
}

uint32_t RiscvSim::Fetch32(uint64_t addr) {
  // 只支持 32-bit 对齐取指
  CheckAlign(addr, 4, "instruction fetch");
  const uint64_t off = AddrToOff(addr);
  if (off + 4 > mem_.size()) {
    Die("pc out of range");
  }
  return static_cast<uint32_t>(mem_[off]) |
         (static_cast<uint32_t>(mem_[off + 1]) << 8) |
         (static_cast<uint32_t>(mem_[off + 2]) << 16) |
         (static_cast<uint32_t>(mem_[off + 3]) << 24);
}

uint64_t RiscvSim::Load(uint64_t addr, unsigned size, bool is_signed) {
  // RV64I 的基本 load，按字节拼装
  CheckAlign(addr, size, "load");
  const uint64_t off = AddrToOff(addr);
  if (off + size > mem_.size()) {
    Die("load out of range");
  }
  uint64_t v = 0;
  for (unsigned i = 0; i < size; ++i) {
    v |= static_cast<uint64_t>(mem_[off + i]) << (8 * i);
  }
  if (is_signed) {
    return static_cast<uint64_t>(SignExtend(v, size * 8));
  }
  return v;
}

void RiscvSim::Store(uint64_t addr, uint64_t value, unsigned size) {
  // RV64I 的基本 store，按字节写回
  CheckAlign(addr, size, "store");
  const uint64_t off = AddrToOff(addr);
  if (off + size > mem_.size()) {
    Die("store out of range");
  }
  for (unsigned i = 0; i < size; ++i) {
    mem_[off + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xff);
  }
}

void RiscvSim::CheckAlign(uint64_t addr, unsigned align, const char* op) {
  if (addr % align != 0) {
    std::cerr << "exception: unaligned " << op << " addr=0x" << std::hex << addr
              << std::dec << "\n";
    std::exit(1);
  }
}

uint64_t RiscvSim::AddrToOff(uint64_t addr) {
  if (addr < base_) {
    Die("address below base");
  }
  return addr - base_;
}

void RiscvSim::Step(uint32_t inst) {
  // 把 0x00000000 作为“干净停机”的约定
  if (inst == 0) {
    std::cout << "halt: illegal 0x0 at pc=0x" << std::hex << pc_ << std::dec << "\n";
    std::exit(0);
  }
  const uint32_t opcode = inst & 0x7f;
  const uint32_t rd = (inst >> 7) & 0x1f;
  const uint32_t funct3 = (inst >> 12) & 0x7;
  const uint32_t rs1 = (inst >> 15) & 0x1f;
  const uint32_t rs2 = (inst >> 20) & 0x1f;
  const uint32_t funct7 = (inst >> 25) & 0x7f;

  const int64_t imm_i = SignExtend(inst >> 20, 12);
  const int64_t imm_s = SignExtend(((inst >> 25) << 5) | ((inst >> 7) & 0x1f), 12);
  const int64_t imm_b = SignExtend(((inst >> 31) << 12) |
                                      (((inst >> 7) & 0x1) << 11) |
                                      (((inst >> 25) & 0x3f) << 5) |
                                      (((inst >> 8) & 0xf) << 1),
                                    13);
  const int64_t imm_u = static_cast<int64_t>(inst & 0xfffff000);
  const int64_t imm_j = SignExtend(((inst >> 31) << 20) |
                                      (((inst >> 12) & 0xff) << 12) |
                                      (((inst >> 20) & 0x1) << 11) |
                                      (((inst >> 21) & 0x3ff) << 1),
                                    21);

  uint64_t next_pc = pc_ + 4;

  switch (opcode) {
    case 0x37:  // LUI
      regs_[rd] = static_cast<uint64_t>(imm_u);
      break;
    case 0x17:  // AUIPC
      regs_[rd] = pc_ + static_cast<uint64_t>(imm_u);
      break;
    case 0x6f:  // JAL
      regs_[rd] = next_pc;
      next_pc = pc_ + static_cast<uint64_t>(imm_j);
      break;
    case 0x67:  // JALR
      regs_[rd] = next_pc;
      next_pc = (regs_[rs1] + static_cast<uint64_t>(imm_i)) & ~1ULL;
      break;
    case 0x63:  // Branch
      switch (funct3) {
        case 0x0:  // BEQ
          if (regs_[rs1] == regs_[rs2]) {
            next_pc = pc_ + static_cast<uint64_t>(imm_b);
          }
          break;
        case 0x1:  // BNE
          if (regs_[rs1] != regs_[rs2]) {
            next_pc = pc_ + static_cast<uint64_t>(imm_b);
          }
          break;
        case 0x4:  // BLT
          if (static_cast<int64_t>(regs_[rs1]) < static_cast<int64_t>(regs_[rs2])) {
            next_pc = pc_ + static_cast<uint64_t>(imm_b);
          }
          break;
        case 0x5:  // BGE
          if (static_cast<int64_t>(regs_[rs1]) >= static_cast<int64_t>(regs_[rs2])) {
            next_pc = pc_ + static_cast<uint64_t>(imm_b);
          }
          break;
        case 0x6:  // BLTU
          if (regs_[rs1] < regs_[rs2]) {
            next_pc = pc_ + static_cast<uint64_t>(imm_b);
          }
          break;
        case 0x7:  // BGEU
          if (regs_[rs1] >= regs_[rs2]) {
            next_pc = pc_ + static_cast<uint64_t>(imm_b);
          }
          break;
        default:
          Illegal(inst);
      }
      break;
    case 0x03:  // Load
      switch (funct3) {
        case 0x0:
          regs_[rd] = Load(regs_[rs1] + imm_i, 1, true);
          break;
        case 0x1:
          regs_[rd] = Load(regs_[rs1] + imm_i, 2, true);
          break;
        case 0x2:
          regs_[rd] = Load(regs_[rs1] + imm_i, 4, true);
          break;
        case 0x3:
          regs_[rd] = Load(regs_[rs1] + imm_i, 8, true);
          break;
        case 0x4:
          regs_[rd] = Load(regs_[rs1] + imm_i, 1, false);
          break;
        case 0x5:
          regs_[rd] = Load(regs_[rs1] + imm_i, 2, false);
          break;
        case 0x6:
          regs_[rd] = Load(regs_[rs1] + imm_i, 4, false);
          break;
        default:
          Illegal(inst);
      }
      break;
    case 0x23:  // Store
      switch (funct3) {
        case 0x0:
          Store(regs_[rs1] + imm_s, regs_[rs2], 1);
          break;
        case 0x1:
          Store(regs_[rs1] + imm_s, regs_[rs2], 2);
          break;
        case 0x2:
          Store(regs_[rs1] + imm_s, regs_[rs2], 4);
          break;
        case 0x3:
          Store(regs_[rs1] + imm_s, regs_[rs2], 8);
          break;
        default:
          Illegal(inst);
      }
      break;
    case 0x13:  // OP-IMM
      switch (funct3) {
        case 0x0:  // ADDI
          regs_[rd] = regs_[rs1] + imm_i;
          break;
        case 0x7:  // ANDI
          regs_[rd] = regs_[rs1] & static_cast<uint64_t>(imm_i);
          break;
        case 0x6:  // ORI
          regs_[rd] = regs_[rs1] | static_cast<uint64_t>(imm_i);
          break;
        case 0x4:  // XORI
          regs_[rd] = regs_[rs1] ^ static_cast<uint64_t>(imm_i);
          break;
        default:
          Illegal(inst);
      }
      break;
    case 0x33:  // OP
      switch (funct3) {
        case 0x0:
          if (funct7 == 0x20) {
            regs_[rd] = regs_[rs1] - regs_[rs2];
          } else if (funct7 == 0x00) {
            regs_[rd] = regs_[rs1] + regs_[rs2];
          } else {
            Illegal(inst);
          }
          break;
        case 0x7:
          regs_[rd] = regs_[rs1] & regs_[rs2];
          break;
        case 0x6:
          regs_[rd] = regs_[rs1] | regs_[rs2];
          break;
        case 0x4:
          regs_[rd] = regs_[rs1] ^ regs_[rs2];
          break;
        default:
          Illegal(inst);
      }
      break;
    case 0x73:  // SYSTEM
      Die("system instruction not supported");
      break;
    default:
      Illegal(inst);
  }

  regs_[0] = 0;
  pc_ = next_pc;
}

[[noreturn]] void RiscvSim::Illegal(uint32_t inst) {
  std::cerr << "illegal instruction 0x" << std::hex << inst << " at pc=0x" << pc_
            << std::dec << "\n";
  std::exit(1);
}

int main(int argc, char** argv) {
  const Options opt = ParseArgs(argc, argv);
  std::vector<uint8_t> mem(opt.mem_size, 0);
  const ElfImage image = LoadElf(opt.elf_path, opt.base, mem);

  RiscvSim sim(opt.base, std::move(mem));
  sim.Run(image.entry, opt);
  return 0;
}
