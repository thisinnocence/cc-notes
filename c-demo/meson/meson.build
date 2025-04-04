project('demo', 'c',
  default_options : [
    'c_std=c11',  # 设置 C 标准为 C11
    'buildtype=debug',  # 设置构建类型为 debug
    'warning_level=3',  # 设置警告级别为 3
    'c_args=-Wall -Wextra -Wpedantic -Werror',  # 设置编译器参数
  ]
)

# 添加头文件搜索目录，下面函数是meson内置函数，返回的是一个对象，对象内部细节不对外暴漏
inc = include_directories(
    'include',
)

# 搜集 math 子目录的源文件, 返回一个对象
math_sources = files(
  'math/add.c'  # 直接指定 math 子目录中的源文件
)

executable('demo',
  ['main.c'] + math_sources,
  include_directories : inc,
)
