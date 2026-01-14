#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <cstdint>
#include <string>
#include <vector>

struct ElfImage {
  // ELF 的入口地址
  uint64_t entry;
};

ElfImage LoadElf(const std::string& path, uint64_t base, std::vector<uint8_t>& mem);

#endif
