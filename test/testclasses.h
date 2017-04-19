#pragma once
#include "../reflect.h"

#include "dependencies/Catch/catch.hpp"

#include <complex>
#include <iostream>
#include <string>

namespace {
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
}

REFLECT_ENABLE(Foo, i, string, ncmem, Bar, Baz, Baz2, Perf, NC, Tally)

namespace {
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
}

#pragma warning(suppress: 4003)
REFLECT_ENABLE(Static, abc, string, Funky, FunkyParams, FunkyReturns, FunkyPassthrough);

namespace {
struct Const {
   const static int _static = 10;
   const int _const = 12;
};

const int Const::_static;
}

REFLECT_ENABLE(Const, _static, _const);

namespace {
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
}

REFLECT_ENABLE(Arrays, iptr, iarr, iarr10);
