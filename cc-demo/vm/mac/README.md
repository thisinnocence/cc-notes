# RISC-V RV64I 最小解释器

这是一个最小的 RISC-V RV64I 解释执行模型，目标是能运行极简的 bare-metal 程序。
当前项目包含一个最小 demo（汇编引导 + C 小函数），以及一个简单的 ELF64 裸机加载器和解释器。

## 目录结构

- `riscv_sim.cpp` / `riscv_sim.h`：解释器与执行逻辑
- `elf_loader.cpp` / `elf_loader.h`：ELF64 裸机加载器
- `demo/`：bare-metal 示例程序（汇编入口 + C 代码 + linker 脚本）
- `run_all.sh`：一键构建并运行

## 功能范围

- ISA：RV64I 子集（足够跑 demo）
- ELF：ELF64 little-endian，ET_EXEC，非 PIE，无重定位
- 内存：平坦物理内存，默认基址 `0x80000000`
- 终止：遇到 `0x00000000` 指令视为正常停机；非法指令/未对齐/越界会打印并退出
- I/O：仅标准输出日志

## 构建与运行

### 一键运行

```bash
./run_all.sh
```

### 分步构建

```bash
cmake -S . -B build
cmake --build build
make -C demo
./build/riscv_sim demo/demo.elf
```

## demo 说明

`demo/` 内包含一个极简 bare-metal 程序：

- `start.S`：设置栈指针并调用 `main`
- `main.c`：包含 for/if/else/add/sub 等最基础逻辑
- `linker.ld`：将代码放在 `0x80000000`，定义栈顶

`main` 返回后，`start.S` 执行一条 `0x00000000` 的非法指令，
解释器约定将其视为“正常停机”。

## 运行参数

```bash
./build/riscv_sim demo/demo.elf \
  --base=0x80000000 \
  --mem=65536 \
  --max-steps=5000000 \
  --halt=0x8000000c
```

- `--base`：内存基址
- `--mem`：内存大小（字节）
- `--max-steps`：最大执行步数
- `--halt`：PC 到达该地址时停止

