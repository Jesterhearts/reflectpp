# reflectpp
Runtime reflection in c++

## Motivation
It's useful to be able to treat objects as dictionaries for many reasons. A
prime example being serialization. This library provides a relatively efficient
way of doing so, allowing you to write code like this:
```
struct MyCalc {
    static int add(int, int);
    static int sub(int, int);
    static int mul(int, int);
    static int div(int, int);

    void plus(int);
    void minus(int);
    void times(int);
    void divided_by(int);

    int value;
};

REFLECT_ENABLE(MyCalc, add, sub, mul, div, plus, minus, times, divided_by)

//...
auto static_calc = reflect::reflect<MyCalc>();

std::cout << "Enter an operation (add, sub, mul, div):" << std::endl;

std::string op;
std::cin >> op;

std::cout << "Enter two numbers:" << std::endl;

int a;
std::cin >> a;

int b;
std::cin >> b;

try {
    int result = static_calc[op](a, b);
    std::cout << "result: " << result << std::endl;
}
catch (const reflect::member_access_error&) {
    std::cout << "invalid operation" << std::endl;
}

MyCalc mycalc;
auto dynamic_calc = reflect::reflect(mycalc);

std::cout << "Enter an operation (plus, minus, times, divided_by):" << std::endl;

std::string op;
std::cin >> op;

std::cout << "Enter a number:" << std::endl;
std::cin >> a;

try {
    reflected[op](a);
    double value = reflected["value"];

    std::cout << "current value: " << value << std::endl;
}
catch (const reflect::member_access_error&) {
    std::cout << "invalid operation" << std::endl;
}

```

## Features
- Full object reflection of members and functions.
- Reflected members can be interacted with using `operator=` and `operator()`,
  allowing them to be used as if they were an actual value.
- Support for non-copyable and non-movable objects
- Preserves const-ness of member variables.
- Relatively low runtime overhead.

## Limitations
- Requires a C++14 compliant compiler
    - Tested on VS2015, VS2015 + clang front-end
- No support for overloaded member functions
- No support for reflecting/accessing base class members
- No support for reflecting const objects
- No exposure of reflected members/names so they can be iterated over

## Future features
1. Support for REPL-style code
  1. Variable creation/assignment
  2. Limited sandboxing of executed commands
2. Exposure of member info via iterators
3. Reflection of const objects, preserving mutableness of members
4. Support for inheritance and reflecting base-objects

See test/main.cxx for more example usages.
