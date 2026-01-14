#ifndef CC_NOTES_CC_DEMO_VM_MAC_ELF_LOADER_H_
#define CC_NOTES_CC_DEMO_VM_MAC_ELF_LOADER_H_

#include <cstdint>
#include <string>
#include <vector>

struct ElfImage {
  // ELF 的入口地址
  uint64_t entry;
};

ElfImage LoadElf(const std::string& path, uint64_t base,
                 std::vector<uint8_t>& mem);

#endif  // CC_NOTES_CC_DEMO_VM_MAC_ELF_LOADER_H_
