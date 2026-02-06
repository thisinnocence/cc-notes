#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>

// ===== 1) Function template =====
// 用一个实现支持多种 type，编译器会按调用点进行 instantiation。
template <typename T>
T MaxOf(T a, T b) {
  return (a > b) ? a : b;
}

// ===== 2) Class template =====
// 用 template parameter T 复用同一套 class 逻辑。
template <typename T>
class Box {
 public:
  explicit Box(T value) : value_(value) {}

  const T& Get() const { return value_; }

 private:
  T value_;
};

// ===== 3) Specialization（full specialization + partial specialization）=====
// primary template：默认实现。
template <typename T>
struct TypeName {
  static constexpr const char* kValue = "unknown";
};

// full specialization：对特定 type（int）提供定制实现。
template <>
struct TypeName<int> {
  static constexpr const char* kValue = "int";
};

// 再给一个 partial specialization 示例：识别 pointer type。
template <typename T>
struct IsPointer {
  // 主模板：任何不匹配 T* 形态的类型，默认都不是指针。
  static constexpr bool kValue = false;
};
//
template <typename T>
struct IsPointer<T*> {
  // 偏特化：当类型形如 T* 时匹配这个版本，判定为指针。
  static constexpr bool kValue = true;
};

// ===== 4) Non-type template parameter =====
// 模板参数不只是 type，也可以是编译期常量（这里是 N）。
template <typename T, std::size_t N>
struct FixedArray {
  std::array<T, N> data{};

  constexpr std::size_t Size() const { return N; }
};

// ===== 5) Variadic template + fold expression =====
// Ts... / values... 表示参数包（parameter pack）。
template <typename... Ts>
auto Sum(Ts... values) {
  // fold expression：把 parameter pack 折叠成一个表达式。
  return (values + ...);
}

// ===== 6) Type constraint（C++17 写法）+ if constexpr =====
// C++17 没有 concept，这里用 enable_if 做 type constraint。
template <typename T,
          typename = std::enable_if_t<std::is_integral<T>::value ||
                                      std::is_floating_point<T>::value>>
T SafeAbs(T value) {
  // if constexpr 在编译期分支：不满足分支不会参与实例化。
  if constexpr (std::is_unsigned_v<T>) {
    return value;
  } else {
    return value < 0 ? -value : value;
  }
}

int main() {
  // Function template
  std::cout << "MaxOf(int): " << MaxOf(3, 7) << "\n";
  std::cout << "MaxOf(double): " << MaxOf(2.3, 1.9) << "\n";

  // Class template
  Box<std::string> box("hello template");
  std::cout << "Box: " << box.Get() << "\n";

  // Specialization
  std::cout << "TypeName<int>: " << TypeName<int>::kValue << "\n";
  std::cout << "TypeName<double>: " << TypeName<double>::kValue << "\n";
  std::cout << std::boolalpha;
  std::cout << "IsPointer<int>: " << IsPointer<int>::kValue << "\n";
  std::cout << "IsPointer<int*>: " << IsPointer<int*>::kValue << "\n";

  // Non-type template parameter
  FixedArray<int, 5> fixed_array;
  std::cout << "FixedArray size: " << fixed_array.Size() << "\n";

  // Variadic template
  std::cout << "Sum: " << Sum(1, 2, 3, 4) << "\n";

  // Type constraint + if constexpr
  std::cout << "SafeAbs(-9): " << SafeAbs(-9) << "\n";
  std::cout << "SafeAbs(8u): " << SafeAbs(8u) << "\n";

  return 0;
}
