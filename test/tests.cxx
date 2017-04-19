#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include "dependencies/Catch/catch.hpp"

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
   std::string string = {};
   NonCopyable ncmem = { 0 };

   void Bar() {
      std::cout << "Bar called" << std::endl;
      i = 10;
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

REFLECT_ENABLE(Foo, i, string, ncmem, Bar, Baz, Baz2, Perf, NC, Tally)

struct Static {
   static int abc;
   static std::string string;

   static void Funky() {
   }

   static void FunkyParams(int i) {
   }

   static int FunkyReturns() {
      return 42;
   }

   static int FunkyPassthrough(int i) {
      return i;
   }
};

int Static::abc = 0;
std::string Static::string;

#pragma warning(suppress: 4003)
REFLECT_ENABLE(Static, abc, string, Funky, FunkyParams, FunkyReturns, FunkyPassthrough);

struct Const {
   const static int _static = 10;
   const int _const = 12;
};

const int Const::_static;

REFLECT_ENABLE(Const, _static, _const);

struct Arrays {
   int* iptr = new int(42);
   int* iarr = new int[5]{ 0, 1, 2, 3, 4 };
   int iarr10[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
   static int static5[5];

   ~Arrays() {
      delete iptr;
      delete[] iarr;
   }
};

//TODO fails to compile if reflected
int Arrays::static5[5] = { 0, 1, 2, 3, 4 };

REFLECT_ENABLE(Arrays, iptr, iarr, iarr10);

TEST_CASE("basic member access", "[reflection][access][basic]") {
   Foo f{};
   auto reflected = reflect::reflect(f);

   SECTION("member pod variable access") {
      REQUIRE_NOTHROW(reflected["i"].get_type());
   }

   SECTION("member object variable access") {
      REQUIRE_NOTHROW(reflected["string"].get_type());
   }

   SECTION("member function access") {
      REQUIRE_NOTHROW(reflected["Bar"].get_type());
   }

   SECTION("non-existent member access") {
      REQUIRE_THROWS_AS(reflected["invalid identifier"], const reflect::member_access_error&);
   }

}

TEST_CASE("static member access", "[reflection][access][static]") {
   Static s;
   auto reflected = reflect::reflect(s);

   SECTION("static variable access") {
      REQUIRE_NOTHROW(reflected["abc"].get_type());
   }

   SECTION("static function access") {
      REQUIRE_NOTHROW(reflected["Funky"].get_type());
   }

}

TEST_CASE("const member access", "[reflection][access][const]") {
   Const const_;
   auto reflected = reflect::reflect(const_);

   SECTION("const member variable access") {
      REQUIRE_NOTHROW(reflected["_const"].get_type());
   }

   SECTION("static const member variable access") {
      REQUIRE_NOTHROW(reflected["_static"].get_type());
   }

}

TEST_CASE("array and pointer member access", "[reflection][access][arrays]") {
   Arrays arrays;
   auto reflected = reflect::reflect(arrays);

   SECTION("pointer member access") {
      REQUIRE_NOTHROW(reflected["iptr"].get_type());
   }

   SECTION("dynamic array member access") {
      REQUIRE_NOTHROW(reflected["iarr"].get_type());
   }

   SECTION("sized array member access") {
      REQUIRE_NOTHROW(reflected["iarr10"].get_type());
   }

}

TEST_CASE("empty object member access", "[reflection][access][empty]") {
   SECTION("non static member access") {
      auto reflected = reflect::reflect<Foo>();
      REQUIRE_NOTHROW(reflected["i"].get_type());
   }

   SECTION("static const member access") {
      auto reflected = reflect::reflect<Const>();
      REQUIRE_NOTHROW(reflected["_static"].get_type());
   }

   SECTION("static member access") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_NOTHROW(reflected["abc"].get_type());
   }

   SECTION("static function access") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_NOTHROW(reflected["Funky"].get_type());
   }

   SECTION("non-existent member access") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_THROWS_AS(reflected["invalid identifier"], const reflect::member_access_error&);
   }

}

TEST_CASE("basic member assignment", "[reflection][assignment][basic]") {
   Foo f;
   auto reflected = reflect::reflect(f);

   SECTION("pod member assignment") {
      REQUIRE(f.i != 10);
      REQUIRE_NOTHROW(reflected["i"] = 10);
      REQUIRE(f.i == 10);
   }

   SECTION("pod member assignment from constexpr var") {
      constexpr int ten = 10;
      REQUIRE(f.i != ten);
      REQUIRE_NOTHROW(reflected["i"] = ten);
      REQUIRE(f.i == ten);
   }

   SECTION("pod member assignment from var") {
      int ten = 10;
      REQUIRE(f.i != ten);
      REQUIRE_NOTHROW(reflected["i"] = ten);
      REQUIRE(f.i == ten);
   }

   SECTION("pod member assignment from const var") {
      const int ten = 10;
      REQUIRE(f.i != ten);
      REQUIRE_NOTHROW(reflected["i"] = ten);
      REQUIRE(f.i == ten);
   }

   SECTION("pod member assignment from smaller type") {
      REQUIRE(f.i != 'c');
      REQUIRE_NOTHROW(reflected["i"] = 'c');
      REQUIRE(f.i == 'c');
   }

   SECTION("pod member assignment from larger type") {
      REQUIRE(f.i != std::size_t(10));
      REQUIRE_NOTHROW(reflected["i"] = std::size_t(10));
      REQUIRE(f.i == std::size_t(10));
   }

   SECTION("pod member assignment from incompatible type") {
      REQUIRE_THROWS_AS(reflected["i"] = std::string{}, const reflect::invalid_assignment_type&);
   }

   SECTION("object member assignment") {
      REQUIRE(f.string == std::string{});
      REQUIRE_NOTHROW(reflected["string"] = std::string{ "test" });
      REQUIRE(f.string == "test");
   }

   SECTION("object member assignment from var") {
      std::string test = "test";
      REQUIRE(f.string == std::string{});

      REQUIRE_NOTHROW(reflected["string"] = test);
      REQUIRE(f.string == test);
   }

   SECTION("object member assignment from const var") {
      const std::string test = "test";
      REQUIRE(f.string == std::string{});

      REQUIRE_NOTHROW(reflected["string"] = test);
      REQUIRE(f.string == test);
   }

   SECTION("object member assignment from compatible type") {
      REQUIRE(f.string == std::string{});
      REQUIRE_NOTHROW(reflected["string"] = "test");
      REQUIRE(f.string == "test");
   }

   SECTION("object member assignment from incompatible type") {
      REQUIRE_THROWS_AS(reflected["string"] = 10, const reflect::invalid_assignment_type&);
   }

   SECTION("move only object member assignment") {
      REQUIRE(f.ncmem.m == 0);
      REQUIRE_NOTHROW(reflected["ncmem"] = NonCopyable{ 10 });
      REQUIRE(f.ncmem.m == 10);
   }

   SECTION("assignment to a member function") {
      REQUIRE_THROWS_AS(reflected["Bar"] = 10, const reflect::invalid_assignment_type&);
   }

}

TEST_CASE("static member assignment", "[reflection][assignment][static]") {
   Static::abc = 0;
   Static::string = std::string{};

   Static s;
   auto reflected = reflect::reflect(s);

   SECTION("static pod assignment") {
      REQUIRE(Static::abc != 10);
      REQUIRE_NOTHROW(reflected["abc"] = 10);
      REQUIRE(Static::abc == 10);
   }

   SECTION("static pod assignment from variable") {
      int ten = 10;
      REQUIRE(Static::abc != ten);
      REQUIRE_NOTHROW(reflected["abc"] = ten);
      REQUIRE(Static::abc == ten);
   }

   SECTION("static pod assignment from const variable") {
      const int ten = 10;
      REQUIRE(Static::abc != ten);
      REQUIRE_NOTHROW(reflected["abc"] = ten);
      REQUIRE(Static::abc == ten);
   }

   SECTION("static object assignment") {
      REQUIRE(Static::string != "test");
      REQUIRE_NOTHROW(reflected["string"] = "test");
      REQUIRE(Static::string == "test");
   }

   SECTION("static object assignment from incompatible type") {
      REQUIRE_THROWS_AS(reflected["string"] = 10, const reflect::invalid_assignment_type&);
   }

   SECTION("static function assignment") {
      REQUIRE_THROWS_AS(reflected["Funky"] = 10, const reflect::invalid_assignment_type&);
   }

}

TEST_CASE("constant member assignment", "[reflection][assignment][const]") {
   Const const_;
   auto reflected = reflect::reflect(const_);

   SECTION("const member assignment") {
      REQUIRE_THROWS_AS(reflected["_const"] = 10, const reflect::invalid_assignment_to_const&);
   }

   SECTION("const static member assignment") {
      REQUIRE_THROWS_AS(reflected["_static"] = 10, const reflect::invalid_assignment_to_const&);
   }

}

TEST_CASE("array and pointer member assignment", "[!mayfail][reflection][assignment][arrays]") {
   Arrays arrays;
   auto reflected = reflect::reflect(arrays);

   /*
   SECTION("pointer assignment") {
      int* old_ptr = arrays.iptr;
      std::unique_ptr<int> new_ptr = std::make_unique<int>(10);

      REQUIRE(old_ptr != new_ptr.get());
      REQUIRE(*old_ptr != *new_ptr);

      //TODO does not compile
      REQUIRE_NOTHROW(reflected["iptr"] = new_ptr.get());
      CHECK(arrays.iptr == new_ptr.get());
      CHECK(*arrays.iptr == *new_ptr);

      new_ptr.release();
      new_ptr.reset(old_ptr);
   }
   */

   /*
   SECTION("dynamic array assignment") {
      int* old_ptr = arrays.iarr;
      std::unique_ptr<int[]> new_ptr = std::make_unique<int[]>(5);
      new_ptr[0] = 100;

      REQUIRE(old_ptr != new_ptr.get());
      REQUIRE(*old_ptr != *new_ptr.get());

      //TODO does not compile
      REQUIRE_NOTHROW(reflected["iarr"] = new_ptr.get());
      CHECK(arrays.iptr == new_ptr.get());
      CHECK(*arrays.iptr == *new_ptr.get());

      new_ptr.release();
      new_ptr.reset(old_ptr);
   }
   */

   SECTION("sized array assignment") {
      const int newArray[10] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
      static_assert(sizeof(newArray) == sizeof(arrays.iarr10), "");
      REQUIRE(std::memcmp(newArray, arrays.iarr10, sizeof(newArray)) != 0);

      //TODO this throws an exception
      REQUIRE_NOTHROW(reflected["iarr10"] = newArray);
      REQUIRE(std::memcmp(newArray, arrays.iarr10, sizeof(newArray)) == 0);
   }

}

TEST_CASE("empty object member assignment", "[reflection][assignment][empty]") {
   SECTION("non static member assignment") {
      auto reflected = reflect::reflect<Foo>();
      REQUIRE_THROWS_AS(reflected["i"] = 0, const reflect::member_access_error&);
   }

   SECTION("static const member access") {
      auto reflected = reflect::reflect<Const>();
      REQUIRE_THROWS_AS(reflected["_static"] = 0, const reflect::member_access_error&);
   }

   SECTION("static member access") {
      Static::abc = 0;
      auto reflected = reflect::reflect<Static>();

      REQUIRE(Static::abc != 100);
      REQUIRE_NOTHROW(reflected["abc"] = 100);
      REQUIRE(Static::abc == 100);
   }

   SECTION("static function assignment") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_THROWS_AS(reflected["Funky"] = 0, const reflect::invalid_assignment_type&);
   }

   SECTION("non-existent member access") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_THROWS_AS(reflected["invalid identifier"], const reflect::member_access_error&);
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
