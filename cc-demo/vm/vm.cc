#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

// A minimal RISC-V64 interpreter that supports basic instructions
class RiscvInterpreter {
public:
    RiscvInterpreter() : pc_(0) {
        // Initialize registers with zero
        regs_.fill(0);
        // Set stack pointer (x2) to end of memory
        regs_[2] = kMemSize - 8;
    }

    // Execute a single instruction
    void ExecuteInstruction(uint32_t inst) {
        uint32_t opcode = inst & 0x7F;
        uint32_t rd = (inst >> 7) & 0x1F;
        uint32_t rs1 = (inst >> 15) & 0x1F;
        uint32_t rs2 = (inst >> 20) & 0x1F;
        int32_t imm_i = static_cast<int32_t>(inst) >> 20;
        int32_t imm_s = ((inst >> 20) & 0xFE0) | ((inst >> 7) & 0x1F);
        int32_t imm_b = ((inst >> 19) & 0x1000) | ((inst >> 20) & 0x7E0) | ((inst >> 7) & 0x1E) |
                        ((inst << 4) & 0x800);
        int32_t imm_j = ((inst >> 11) & 0x100000) | (inst & 0xFF000) | ((inst >> 9) & 0x800) |
                        ((inst >> 20) & 0x7FE);

        switch (opcode) {
            case 0x33:  // R-type (add, sub, etc.)
                switch (inst >> 25) {
                    case 0:  // add
                        regs_[rd] = regs_[rs1] + regs_[rs2];
                        break;
                    case 32:  // sub
                        regs_[rd] = regs_[rs1] - regs_[rs2];
                        break;
                }
                break;

            case 0x13:  // I-type (addi, etc.)
                regs_[rd] = regs_[rs1] + imm_i;
                break;

            case 0x23:  // S-type (store)
                memory_[regs_[rs1] + imm_s] = regs_[rs2] & 0xFF;
                break;

            case 0x03:  // Load
                regs_[rd] = memory_[regs_[rs1] + imm_i];
                break;

            case 0x63:  // B-type (branches)
                if (regs_[rs1] == regs_[rs2]) {
                    pc_ += (imm_b - 4);  // -4 because pc will be incremented
                }
                break;

            case 0x6F:  // J-type (jal)
                regs_[rd] = pc_ + 4;
                pc_ += (imm_j - 4);  // -4 because pc will be incremented
                break;

            case 0x67:  // jalr
                uint64_t temp = pc_ + 4;
                pc_ = (regs_[rs1] + imm_i - 4) & ~1;  // -4 because pc will be incremented
                regs_[rd] = temp;
                break;
        }

        // Keep x0 as zero
        regs_[0] = 0;
        // Increment program counter
        pc_ += 4;
    }

    // Get register value
    uint64_t GetReg(int index) const { return regs_[index]; }

    // Get program counter
    uint64_t GetPc() const { return pc_; }

private:
    static constexpr size_t kRegCount = 32;   // Number of registers
    static constexpr size_t kMemSize = 1024;  // Memory size in bytes

    std::array<uint64_t, kRegCount> regs_;  // Registers
    std::array<uint8_t, kMemSize> memory_;  // Memory
    uint64_t pc_;                           // Program counter
};

int main() {
    RiscvInterpreter vm;

    // Example program that demonstrates function call and basic arithmetic
    std::vector<uint32_t> program = {
        0x00500513,  // addi x10, x0, 5      # Load argument 5 into a0 (x10)
        0x00600593,  // addi x11, x0, 6      # Load argument 6 into a1 (x11)
        0x018000EF,  // jal  x1, 24          # Call function (PC + 24)
        0x00000013,  // nop                   # Return point
        0x00000013,  // nop                   # Program end
        // Function: add two numbers and return result in a0
        0xFF810113,  // addi sp, sp, -8      # Adjust stack pointer
        0x00112023,  // sw   x1, 0(sp)       # Save return address
        0x00B50533,  // add  x10, x10, x11   # Add arguments
        0x00012083,  // lw   x1, 0(sp)       # Restore return address
        0x00810113,  // addi sp, sp, 8       # Restore stack pointer
        0x00008067   // jalr x0, x1, 0       # Return
    };

    // Execute the program
    for (size_t i = 0; i < program.size(); ++i) {
        std::cout << "Executing instruction " << i << ": 0x" << std::hex << program[i] << std::dec
                  << std::endl;
        vm.ExecuteInstruction(program[i]);
    }

    // Print result
    std::cout << "Result in a0 (x10): " << vm.GetReg(10) << std::endl;

    return 0;
}
