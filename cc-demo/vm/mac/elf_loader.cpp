#include "elf_loader.h"

#include <elf.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

[[noreturn]] void Die(const std::string& msg) {
  std::cerr << "error: " << msg << "\n";
  std::exit(1);
}

std::vector<uint8_t> ReadFile(const std::string& path) {
  // 读取整个 ELF 文件到内存
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    Die("failed to open: " + path);
  }
  in.seekg(0, std::ios::end);
  std::streamoff size = in.tellg();
  if (size <= 0) {
    Die("empty file: " + path);
  }
  in.seekg(0, std::ios::beg);
  std::vector<uint8_t> data(static_cast<size_t>(size));
  if (!in.read(reinterpret_cast<char*>(data.data()), size)) {
    Die("failed to read: " + path);
  }
  return data;
}

}  // namespace

ElfImage LoadElf(const std::string& path, uint64_t base, std::vector<uint8_t>& mem) {
  // 仅解析 ELF64 little-endian 的 PT_LOAD 段
  const std::vector<uint8_t> file = ReadFile(path);
  if (file.size() < sizeof(Elf64_Ehdr)) {
    Die("ELF too small");
  }

  Elf64_Ehdr ehdr{};
  std::memcpy(&ehdr, file.data(), sizeof(ehdr));
  if (std::memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
    Die("bad ELF magic");
  }
  if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
    Die("expected ELF64");
  }
  if (ehdr.e_ident[EI_DATA] != ELFDATA2LSB) {
    Die("expected little-endian");
  }
  if (ehdr.e_machine != EM_RISCV) {
    Die("expected EM_RISCV");
  }
  if (ehdr.e_type != ET_EXEC) {
    Die("expected non-PIE ET_EXEC ELF");
  }

  if (ehdr.e_phoff == 0 || ehdr.e_phentsize != sizeof(Elf64_Phdr)) {
    Die("invalid program header table");
  }
  const uint64_t ph_end = ehdr.e_phoff + static_cast<uint64_t>(ehdr.e_phnum) * sizeof(Elf64_Phdr);
  if (ph_end > file.size()) {
    Die("program headers out of range");
  }

  for (uint16_t i = 0; i < ehdr.e_phnum; ++i) {
    Elf64_Phdr ph{};
    std::memcpy(&ph, file.data() + ehdr.e_phoff + i * sizeof(Elf64_Phdr), sizeof(ph));
    if (ph.p_type != PT_LOAD) {
      continue;
    }
    if (ph.p_memsz == 0) {
      continue;
    }
    if (ph.p_vaddr < base) {
      Die("segment vaddr below base");
    }
    const uint64_t seg_start = ph.p_vaddr - base;
    const uint64_t seg_end = seg_start + ph.p_memsz;
    if (seg_end > mem.size()) {
      Die("segment exceeds memory size");
    }
    if (ph.p_offset + ph.p_filesz > file.size()) {
      Die("segment data out of range");
    }
    std::memcpy(mem.data() + seg_start, file.data() + ph.p_offset, ph.p_filesz);
    if (ph.p_memsz > ph.p_filesz) {
      std::memset(mem.data() + seg_start + ph.p_filesz, 0, ph.p_memsz - ph.p_filesz);
    }
  }

  ElfImage image;
  image.entry = ehdr.e_entry;
  if (image.entry < base || image.entry - base >= mem.size()) {
    Die("entry out of range");
  }
  return image;
}
