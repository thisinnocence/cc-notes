#include <stdint.h>

/* 让编译器保留结果，避免被优化掉。 */
volatile uint64_t sink;

/* 触发基本的 add/sub + if/else 控制流。 */
static uint64_t add_sub(uint64_t a, uint64_t b) {
  uint64_t x = a + b;
  if (x & 1) {
    x = x - b;
  } else {
    x = x + a;
  }
  return x;
}

int main(void) {
  /* for + if/else + add/sub 的最小组合。 */
  uint64_t acc = 0;
  for (uint64_t i = 0; i < 10; ++i) {
    if (i & 1) {
      acc += i;
    } else {
      acc -= i;
    }
  }
  acc += add_sub(7, 3);
  sink = acc;
  return 0;
}
