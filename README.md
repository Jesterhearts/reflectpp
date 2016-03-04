# reflectpp
Runtime reflection in c++

## Motivation
It's useful to be able to treat objects as dictionaries for many reasons. A
prime example being serialization. This library provides a relatively efficient
way of doing so, allowing you to write code like this:
```
struct MyCalc {
    int add(int, int);
    int sub(int, int);
    int mul(int, int);
    int div(int, int);
};

REFLECT_ENABLE(MyCalc, add, sub, mul, div)

//...
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
- Full object reflection of members and functions.
- Reflected members can be interacted with using `operator=` and `operator()`,
  allowing them to be used as if they were an actual value.
- Relatively low runtime overhead:
    - 1 byte per distinct member type, up to 255 types. Size expands to
      accomodate additional types as-needed.
    - Data member assignment/access is 1 virtual function call + 1 or more
      branches to locate the correct function type.
        - The number of branches will typically be very low, since branch count
          scales with the number of unique member types that the arg type is
          implicitly convertible to.
    - Function calls use perfect forwarding on their arguments, so the overhead
      should only be 1 virtual function call + 1 or more branches.
        - The maximum number of branches should also typically be fairly low,
          since the branch count will scale with the number of unique function
          signatures on the object that can implicitly match the arguments and
          the return type.

## Limitations
- Requires a C++14 compliant compiler
    - Tested on VS2015 and mingw/g++ 4.9.2
- No support for reflecting/accessing base class members
- No support for reflecting const objects
- No support for static members
- No exposure of reflected members/names so they can be iterated over

## Future features
1. Support for inheritance and reflecting base-objects
2. Reflection of const objects, preserving const/mutableness of members
3. Exposure of member info via iterators
4. Support for static data/interacting with static data without needing a class
   instance

See test/main.cxx for more example usages.
