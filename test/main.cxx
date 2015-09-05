#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "reflectpp/reflect.h"

struct Perfect {
    int m;

    Perfect(const Perfect&) = delete;
    Perfect(Perfect&&)      = delete;
};

struct NonCopyable {
    int m;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&)      = default;

    NonCopyable& operator=(NonCopyable&&)      = default;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

static int tally = 0;

struct Foo {
    int i;
    bool b;
    char c;
    NonCopyable ncmem;

    void Bar() {
        std::cout << "Bar called" << std::endl;
        b = true;
    }

    int Baz() {
        std::cout << "Baz called" << std::endl;
        return i;
    }

    int Baz2(int x) {
        std::cout << "Baz2 called" << std::endl;
        return x;
    }

    void Perf(Perfect&& n) {
        std::cout << "Perf called" << std::endl;
    }

    void NC(NonCopyable nc) {
        std::cout << "NC called" << std::endl;
    }

    void Tally(int x) {
        tally += 1 + x % static_cast<int>(std::sqrt(tally + 10));
    }
};

REFLECT_ENABLE(Foo, i, b, c, ncmem, Bar, Baz, Baz2, Perf, NC, Tally)

static_assert(reflect::detail::class_reflection_info<Foo>::TypeInfo::get_type_id<int>() == 0, "");
static_assert(reflect::detail::class_reflection_info<Foo>::TypeInfo::get_type_id<bool>() == 1, "");
static_assert(reflect::detail::class_reflection_info<Foo>::TypeInfo::get_type_id<char>() == 2, "");
static_assert(reflect::detail::class_reflection_info<Foo>::TypeInfo::get_type_id<void()>() == 4, "");

static_assert(reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<bool>>::value, "");
static_assert(reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<int, bool>>::value, "");
static_assert(!reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<int>>::value, "");
static_assert(!reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<int, int>>::value, "");

static_assert(reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<bool, char, int>>::index == 0, "");
static_assert(reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<int, bool>>::index == 1, "");
static_assert(reflect::detail::find_duplicate_type<bool, reflect::detail::typelist<int, char, bool>>::index == 2, "");
static_assert(reflect::detail::find_duplicate_type<long, reflect::detail::typelist<int, char, bool>>::index == 3, "");

static_assert(reflect::detail::typelist_to_numberlist<reflect::detail::typeset<bool, int, bool>>::length == 2, "");
static_assert(reflect::detail::typelist_to_numberlist<reflect::detail::typeset<bool, bool, int>>::length == 2, "");
static_assert(reflect::detail::typelist_to_numberlist<reflect::detail::typeset<bool, int, int>>::length == 2, "");

int main() {

    Foo f{};

    auto reflected = reflect::reflect(f);

    reflected["i"] = 'c';

    reflected["i"] = 10;
    assert(f.i == 10);
    std::cout << f.i << std::endl;

    int a = reflected["i"];
    assert(a == 10);

    char c = reflected["i"];

    reflected["Bar"]();
    reflected["Bar"].invoke<void>();
    assert(f.b);

    try {
        reflected["i"]();
        assert(false);
    }
    catch (const reflect::bad_member_access& e) {
        std::cout << "caught: " << e.what() << std::endl;
    }

    int i = reflected["Baz"]();
    assert(i == 10);
    reflected["Baz"]();

    int x = reflected["Baz2"](42);
    assert(x == 42);
    double d = reflected["Baz2"](42.0);

    try {
        int x = reflected["Bar"]();
        assert(false);
    }
    catch (const reflect::bad_member_access& e) {
        std::cout << "caught: " << e.what() << std::endl;
    }

    reflected["Perf"](Perfect{10});

    NonCopyable nc{ 10 };
    reflected["NC"](std::move(nc));

    NonCopyable nc2{ 10 };
    reflected["ncmem"] = std::move(nc);

    constexpr int iters = 100000000;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iters; ++i) {
        f.Tally(42);
    }

    end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<long double, std::nano> delta = end - start;
    long double delta_per_iter = delta.count() / (long double)iters;
    std::cout.precision(15);

    std::cout << "direct: \t" << delta_per_iter << " ns/iter" << std::endl;

    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iters; ++i) {
        reflected["Tally"](42);
    }

    end = std::chrono::high_resolution_clock::now();

    delta = end - start;
    delta_per_iter = delta.count() / (long double)iters;
    std::cout.precision(15);

    std::cout << "reflected: \t" << delta_per_iter << " ns/iter" << std::endl;

    std::cout << tally << std::endl;

    std::cin >> c;
}
