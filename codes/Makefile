# 获取当前目录下的所有 .cpp 和 .c 文件
CPPSRCS := $(wildcard *.cpp)
CSRCS := $(wildcard *.c)

# 将源文件替换为 .out 文件
CPPOUTS := $(CPPSRCS:.cpp=.out)
COUTS := $(CSRCS:.c=.out)

# 默认目标，编译所有源文件为 .out 文件
all: $(CPPOUTS) $(COUTS)

# 规则：将每个 .cpp 文件编译为对应的 .out 文件
%.out: %.cpp
	g++ -g $< -o $@

# 规则：将每个 .c 文件编译为对应的 .out 文件
%.out: %.c
	gcc -g $< -o $@

# 清理生成的 .out 文件
clean:
	rm -vf *.out