#include <Catch/catch_with_main.hpp>

#include "testclasses.h"

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
