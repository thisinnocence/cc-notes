# 获取当前目录下的所有 .cpp 文件
SRCS := $(wildcard *.cpp)

# 将 .cpp 文件替换为 .out 文件
OUTS := $(SRCS:.cpp=.out)

# 默认目标，编译所有 .cpp 文件为 .out 文件
all: $(OUTS)

# 规则：将每个 .cpp 文件编译为对应的 .out 文件
%.out: %.cpp
	g++ -g $< -o $@

# 清理生成的 .out 文件
clean:
	rm -vf *.out