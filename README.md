# reflectpp
Runtime reflection in c++

## Motivation
It's useful to be able to treat objects as dictionaries for many reasons. A
prime example being serialization. This library provides a relatively efficient
way of doing so, allowing you to write code like this:
```
MyCalc mycalc;

auto reflected = reflect::reflect(mycalc);

std::cout << "Enter an operation (add, sub, mul, div):" << std::endl;

std::string op;
std::cin >> op;

std::cout << "Enter two numbers:" << std::endl;

int a;
std::cin >> a;

int b;
std::cin >> b;


try {
    int result = reflected[op](a, b);
    std::cout << "result: " << result << std::endl;
}
catch (const reflect::bad_member_access&) {
    std::cout << "invalid operation" << std::endl;
}
```

## Features
- Full object reflection of members and functions
- Relatively low runtime overhead
    - 1 byte per distinct member type, up to 255 types. Size expands to
      accomodate additional types as-needed.
    - Data member assignment/access is 1 branch + 1 virtual function call
    - Function call via `.invoke<T>` is 1 branch + 1 virtual function call
    - Function call with `(...)` forwards its arguments through a tuple, and may
      be more expensive

## Limitations
- No support for reflecting const objects
- No automatic type conversion
- No support for static members
- No exposure of reflected members/names so they can be iterated over
- Requires a C++14 compliant compiler
    - Tested on VS2015 RC and mingw/g++ 4.9.2
    - VS2015 RC has a bug that prevents reflecting private members

## Future features
1. Implicit conversion should be relatively straightforward to implement and is
   planned next
2. Reflection of const objects, preserving const/mutableness of members
3. Exposure of member info via iterators
4. Support for static data/interacting with static data without needing a class
   instance

See test/main.cxx for more example usages.
