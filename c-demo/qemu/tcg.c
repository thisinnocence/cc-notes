#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

typedef enum { TCG_OP_ADD_I64, TCG_OP_SUB_I64 } TCGOpcode;

typedef struct {
    TCGOpcode op;
    int dst, src1, src2;
} TCGInst;

#define MAX_IR 16
static TCGInst g_ir_buf[MAX_IR];
static int g_ir_count = 0;

/* IR 生成 */
void tcg_gen_add_i64(int dst, int src1, int src2)
{
    printf("[IR ] ADD dst=%d src1=%d src2=%d\n", dst, src1, src2);
    g_ir_buf[g_ir_count++] = (TCGInst){TCG_OP_ADD_I64, dst, src1, src2};
}

void tcg_gen_sub_i64(int dst, int src1, int src2)
{
    printf("[IR ] SUB dst=%d src1=%d src2=%d\n", dst, src1, src2);
    g_ir_buf[g_ir_count++] = (TCGInst){TCG_OP_SUB_I64, dst, src1, src2};
}

/* frontend: AArch64 -> IR */
void trans_aarch64()
{
    printf("[IN ] ADD X0, X0, X1\n");
    tcg_gen_add_i64(0, 0, 1);
    printf("[IN ] SUB X0, X0, X1\n");
    tcg_gen_sub_i64(0, 0, 1);
}

/* backend: TCG -> x86_64 code */
uint8_t *emit(uint8_t *p, const char *instr, size_t n, ...)
{
    va_list ap;
    va_start(ap, n);
    printf("[OUT] %s :", instr);
    for (size_t i = 0; i < n; i++) {
        int v = va_arg(ap, int);
        uint8_t b = (uint8_t)v;
        printf(" 0x%02X", b);
        *p++ = b;
    }
    printf("\n");
    va_end(ap);
    return p;
}

uint8_t *emit_rex64(uint8_t *p)
{
    return emit(p, "REX.W", 1, 0x48);
}

size_t tcg_gen_code(uint8_t *buf)
{
    uint8_t *p = buf;
    int i;

    for (i = 0; i < g_ir_count; i++) {
        switch (g_ir_buf[i].op) {
        case TCG_OP_ADD_I64: {
            p = emit_rex64(p);
            p = emit(p, "add rdi,rsi", 2, 0x01, 0xF7);
            break;
        }
        case TCG_OP_SUB_I64: {
            p = emit_rex64(p);
            p = emit(p, "sub rdi,rsi", 2, 0x29, 0xF7);
            break;
        }
        }
    }

    /* mov rax,rdi */
    p = emit_rex64(p);
    p = emit(p, "mov rax,rdi", 2, 0x89, 0xF8);
    /* ret */
    *p++ = 0xC3;
    printf("[OUT] ret : 0xC3\n");
    return p - buf;
}

typedef uint64_t (*jit_func_t)(uint64_t, uint64_t);

int main(void)
{
    size_t buf_size = 4096;

    /* 分配可执行缓冲区 */
    uint8_t *code_gen_buffer =
        mmap(NULL, buf_size, PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (code_gen_buffer == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* 前端翻译 */
    trans_aarch64();

    /* 后端翻译 */
    // (gdb) disass /r code_gen_buffer,+code_size
    size_t code_size = tcg_gen_code(code_gen_buffer);
    printf("Generated %zu bytes of x86_64 code.\n", code_size);

    /* 5. 执行 */
    jit_func_t func = (jit_func_t)code_gen_buffer;
    uint64_t result = func(2, 3);
    printf("result=%lu\n", result);

    munmap(code_gen_buffer, buf_size);
    return 0;
}