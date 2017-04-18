#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include "dependencies/Catch/catch_with_main.hpp"

#include "../reflect.h"


struct Perfect {
   int m = 0;

   Perfect(const Perfect&) = delete;
   Perfect(Perfect&&) = delete;
};

struct NonCopyable {
   int m;

   NonCopyable(const NonCopyable&) = delete;
   NonCopyable(NonCopyable&&) = default;

   NonCopyable& operator=(NonCopyable&&) = default;
   NonCopyable& operator=(const NonCopyable&) = delete;
};

static int tally = 0;

struct Foo {
   int i = 0;
   bool b = false;
   char c = '\0';
   NonCopyable ncmem{ 0 };

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

struct Static {
   static int abc;
};

int Static::abc = 0;

REFLECT_ENABLE(Static, abc);

struct Const {
   const static int _static = 10;
   const int _const = 12;
};

const int Const::_static;

REFLECT_ENABLE(Const, _static, _const);

struct Arrays {
   int* iptr = new int(42);
   int iarr10[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

   ~Arrays() {
      delete iptr;
   }
};

REFLECT_ENABLE(Arrays, iptr, iarr10);

SCENARIO("members can be written to", "[reflection]") {
GIVEN("a reflected object") {
   Foo f{};
   auto reflected = reflect::reflect(f);

   REQUIRE(f.i == 0);
   REQUIRE(f.b == false);
   REQUIRE(f.c == 0);
   REQUIRE(f.ncmem.m == 0);

WHEN("an int literal is assigned to an int member") {
   CHECK_NOTHROW(reflected["i"] = 10);

THEN("the value is changed") {
   CHECK(10 == f.i);
}
}

WHEN("an int is assigned to an int member") {
   constexpr int ten = 10;
   CHECK_NOTHROW(reflected["i"] = ten);

THEN("the value is changed") {
   CHECK(ten == f.i);
}
}

WHEN("a char is assigned to an int member") {
   constexpr char c = 'c';
   CHECK_NOTHROW(reflected["i"] = c);

THEN("the value is changed") {
   CHECK(c == f.i);
}
}

WHEN("a size_t is assigned to an int member") {
   constexpr std::size_t eleven = 11;
   CHECK_NOTHROW(reflected["i"] = eleven);

THEN("the value is changed") {
   CHECK(eleven == f.i);
}
}

WHEN("a char is assigned to a char member") {
   constexpr char c = 'c';
   CHECK_NOTHROW(reflected["c"] = c);

THEN("the value is changed") {
   CHECK(c == f.c);
}
}

WHEN("a string is assigned to a char member") {
THEN("an exception is thrown") {
   static const std::string s = "hello world";
   CHECK_THROWS_AS(reflected["c"] = s, const reflect::invalid_assignment_type&);
}
}

WHEN("a string is assigned to a function member") {
THEN("an exception is thrown") {
   static const std::string s = "hello world";
   CHECK_THROWS_AS(reflected["Bar"] = s, const reflect::invalid_assignment_type&);
}
}
}

GIVEN("a reflected static instance with non-static members") {
   auto reflected = reflect::reflect<Foo>();

WHEN("a non-static member is accessed") {
THEN("an exception is thrown") {
   CHECK_THROWS_AS(reflected["i"] = 0, const reflect::member_access_error&);
}
}
}

GIVEN("a reflected static instance with static members") {
   auto reflected = reflect::reflect<Static>();
   REQUIRE(Static::abc == 0);

WHEN("a static member is accessed") {
   CHECK_NOTHROW(reflected["abc"] = 10);

THEN("the value is changed") {
   CHECK(10 == Static::abc);
}
}
}
}

/*
int main() {

   Foo f{};
   auto reflectfoo = reflect::reflect(f);
   {
      reflectfoo["i"] = 'c';

      reflectfoo["i"] = 10;
      assert(f.i == 10);
      std::cout << f.i << std::endl;

      int a = reflectfoo["i"];
      assert(a == 10);

      int& aref = reflectfoo["i"];
      assert(aref == 10);

      char c = reflectfoo["i"];

      reflectfoo["Bar"]();
      reflectfoo["Bar"].invoke<void>();
      assert(f.b);

      try {
         reflectfoo["i"]();
         assert(false);
      }
      catch (const reflect::invalid_function_call& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }

      reflectfoo["i"] = std::size_t{ 0 };
      assert(f.i == 0);

      try {
         reflectfoo["i"] = std::string();
         assert(false);
      }
      catch (const reflect::invalid_assignment_type& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }

      int i = reflectfoo["Baz"]();
      assert(i == f.i);

      reflectfoo["Baz"]();

      int x = reflectfoo["Baz2"](42);
      assert(x == 42);
      double d = reflectfoo["Baz2"](42.0);

      try {
         int x = reflectfoo["Bar"]();
         assert(false);
      }
      catch (const reflect::invalid_function_call& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }

      std::size_t sizei = reflectfoo["i"];
      assert(f.i == sizei);

      try {
         std::string s = reflectfoo["i"];
         assert(false);
      }
      catch (const reflect::invalid_requested_member_type& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }

      reflectfoo["Perf"](Perfect{ 10 });

      NonCopyable& ncref = reflectfoo["ncmem"];
      assert(ncref.m == 43);

      NonCopyable nc2{ 10 };
      reflectfoo["ncmem"] = std::move(nc2);
      assert(ncref.m == 10);

      NonCopyable nc{ 10 };
      reflectfoo["NC"](std::move(nc));
   }

   auto reflectstaticfoo = reflect::reflect<Foo>();
   {
      try {
         int  i = reflectstaticfoo["i"];
         assert(false);
      }
      catch (const reflect::member_access_error& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }
   }

   auto reflectstatic = reflect::reflect<Static>();
   {
      int abc = reflectstatic["abc"];
      assert(abc == Static::abc);

      reflectstatic["abc"] = 2;
      assert(Static::abc == 2);
   }

   Const _const;
   auto reflectconst = reflect::reflect(_const);
   {
      int c = reflectconst["_const"];
      assert(c == _const._const);

      try {
         reflectconst["_const"] = 10;
         assert(false);
      }
      catch (const reflect::invalid_assignment_to_const& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }

      int s = reflectconst["_static"];
      assert(s == Const::_static);

      try {
         reflectconst["_static"] = 10;
         assert(false);
      }
      catch (const reflect::invalid_assignment_to_const& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }
   }

   Arrays arrays;
   auto reflectarrays = reflect::reflect(arrays);
   {
      int* p = reflectarrays["iptr"];
      assert(p == arrays.iptr);

      int* arrptr = reflectarrays["iarr10"];
      assert(arrptr == arrays.iarr10);

      int (&arrref)[10] = reflectarrays["iarr10"];
      assert(arrref == arrays.iarr10);

      try {
         int (&badarref)[2] = reflectarrays["iarr10"];
         assert(false);
      }
      catch (const reflect::invalid_requested_member_type& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }
   }

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
      reflectfoo["Tally"](42);
   }

   end = std::chrono::high_resolution_clock::now();

   delta = end - start;
   delta_per_iter = delta.count() / (long double)iters;
   std::cout.precision(15);

   std::cout << "reflected: \t" << delta_per_iter << " ns/iter" << std::endl;

   std::cout << tally << std::endl;

   std::cin.ignore();
}
//*/
