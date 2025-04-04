CPPSRCS := $(wildcard *.cpp)

# 将源文件替换为 .out 文件
CPPOUTS := $(CPPSRCS:.cpp=.out)

# 默认目标，编译所有源文件为 .out 文件
all: $(CPPOUTS) $(COUTS)

# 规则：将每个 .cpp 文件编译为对应的 .out 文件
%.out: %.cpp
	g++ -g $< -o $@

# 清理生成的 .out 文件
clean:
	rm -vf *.out