#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

#include <reflect.h>

struct Perfect {
   int m;

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
   int i;
   bool b;
   char c;
   NonCopyable ncmem{ 43 };

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

int Static::abc = 1;

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

      try {
         reflectfoo["i"] = std::string();
         assert(false);
      }
      catch (const reflect::invalid_assignment_type& e) {
         std::cout << "caught: " << e.what() << std::endl;
      }

      int i = reflectfoo["Baz"]();
      assert(i == 10);
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
