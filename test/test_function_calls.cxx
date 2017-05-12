#include "../reflect.h"

#include <Catch/catch.hpp>

namespace {

struct NonMovableNonCopyable {
   int value = 0;

   NonMovableNonCopyable() = default;

   NonMovableNonCopyable(const NonMovableNonCopyable&) = delete;
   NonMovableNonCopyable& operator=(const NonMovableNonCopyable&) = delete;

   NonMovableNonCopyable(NonMovableNonCopyable&&) = delete;
   NonMovableNonCopyable& operator=(NonMovableNonCopyable&&) = delete;
};

struct NonCopyable {
   int value = 0;

   NonCopyable() = default;

   NonCopyable(const NonCopyable&) = delete;
   NonCopyable& operator=(const NonCopyable&) = delete;

   NonCopyable(NonCopyable&&) = default;
   NonCopyable& operator=(NonCopyable&&) = default;
};

struct Functions {
   int value = 0;
   std::string string_value = {};
   NonCopyable nc_value = {};
   NonMovableNonCopyable ncnm_value = {};

   bool void0_called = false;
   void Void0() {
      void0_called = true;
   }

   void Void1_Int(int i) {
      value = i;
   }

   bool int0_called = false;
   int Int0() {
      int0_called = true;
      return value;
   }

   int Int1_Int(int i) {
      return i;
   }

   void Void1_String(std::string s) {
      string_value = s;
   }

   void Void1_StringRef(std::string& s) {
      s = string_value;
   }

   void Void1_ConstStringRef(const std::string& s) {
      string_value = s;
   }

   bool string0_called = false;
   std::string String0() {
      string0_called = true;
      return string_value;
   }

   std::string& StringRef0() {
      return string_value;
   }

   const std::string& ConstStringRef0() {
      return string_value;
   }

   std::string String1_String(std::string s) {
      return s;
   }

   void Void1_NonCopyable(NonCopyable nc) {
      nc_value = std::move(nc);
   }

   void Void1_NonCopyableRefRef(NonCopyable&& nc) {
      nc_value = std::move(nc);
   }

   NonCopyable NonCopyable0() {
      return std::move(nc_value);
   }

   NonCopyable&& NonCopyableRefRef0() {
      return std::move(nc_value);
   }

   void Void1_NonMovableNonCopyable(NonMovableNonCopyable& ncnm) {
      ncnm_value.value = ncnm.value;
   }

   void Void2_ConstStringRef_ConstStringRef(
      const std::string& s1,
      const std::string& s2)
   {
      string_value = s1 + s2;
   }

   std::string String2_ConstStringRef_ConstStringRef(
      const std::string& s1,
      const std::string& s2)
   {
      return s1 + s2;
   }

};

}

REFLECT_ENABLE(
   Functions,
   Void0,
   Void1_Int,
   Int0,
   Int1_Int,
   Void1_String,
   Void1_StringRef,
   Void1_ConstStringRef,
   String0,
   StringRef0,
   ConstStringRef0,
   String1_String,
   Void1_NonCopyable,
   Void1_NonCopyableRefRef,
   NonCopyable0,
   NonCopyableRefRef0,
   Void1_NonMovableNonCopyable,
   Void2_ConstStringRef_ConstStringRef,
   String2_ConstStringRef_ConstStringRef
);

TEST_CASE("basic function calls", "[reflection][functions][basic]") {
   Functions f;
   auto reflected = reflect::reflect(f);

   SECTION("void() call") {
      REQUIRE_FALSE(f.void0_called);
      REQUIRE_NOTHROW(reflected["Void0"]());
      REQUIRE(f.void0_called);
   }

   SECTION("void() via invoke<void>") {
      REQUIRE_FALSE(f.void0_called);
      REQUIRE_NOTHROW(reflected["Void0"].invoke<void>());
      REQUIRE(f.void0_called);
   }

   SECTION("void() via invoke<int>") {
      REQUIRE_THROWS_AS(
         reflected["Void0"].invoke<int>(),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.void0_called);
   }

   SECTION("void() supplied arguments") {
      REQUIRE_THROWS_AS(
         reflected["Void0"](10),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.void0_called);
   }

   SECTION("void() via invoke<void> supplied arguments") {
      REQUIRE_THROWS_AS(
         reflected["Void0"].invoke<void>(10),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.void0_called);
   }

   SECTION("void() via invoke<int> supplied arguments") {
      REQUIRE_THROWS_AS(
         reflected["Void0"].invoke<int>(10),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.void0_called);
   }

}

TEST_CASE(
   "basic function call returns",
   "[reflection][functions][return][basic][int]")
{
   Functions f;
   auto reflected = reflect::reflect(f);

   SECTION("int() call without value capture") {
      REQUIRE_FALSE(f.int0_called);
      REQUIRE_NOTHROW(reflected["Int0"]());
      REQUIRE(f.int0_called);
   }

   SECTION("int() call with value capture") {
      int value = f.value + 1;
      REQUIRE(value != f.value);

      REQUIRE_NOTHROW(value = reflected["Int0"]());
      REQUIRE(value == f.value);
   }

   SECTION("int() call with double value capture") {
      double value = f.value + 1;
      REQUIRE(value != f.value);

      REQUIRE_NOTHROW(value = reflected["Int0"]());
      REQUIRE(value == f.value);
   }

   SECTION("int() call without value capture via invoke<int>") {
      REQUIRE_FALSE(f.int0_called);
      REQUIRE_NOTHROW(reflected["Int0"].invoke<int>());
      REQUIRE(f.int0_called);
   }

   SECTION("int() call via invoke<int> with value capture") {
      int value = f.value + 1;
      REQUIRE(value != f.value);

      REQUIRE_NOTHROW(value = reflected["Int0"].invoke<int>());
      REQUIRE(value == f.value);
   }

   SECTION("int() call without value capture via invoke<void>") {
      REQUIRE_FALSE(f.int0_called);
      REQUIRE_NOTHROW(reflected["Int0"].invoke<void>());
      REQUIRE(f.int0_called);
   }

   SECTION("int() call via invoke<double> with value capture") {
      double value = f.value + 1;
      REQUIRE(value != f.value);

      REQUIRE_NOTHROW(value = reflected["Int0"].invoke<double>());
      REQUIRE(value == f.value);
   }

   SECTION("int() call via invoke<string>") {
      REQUIRE_THROWS_AS(
         reflected["Int0"].invoke<std::string>(),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.int0_called);
   }

}

TEST_CASE(
   "function call returns with objects",
   "[reflection][functions][return][basic][objects]")
{
   Functions f;
   auto reflected = reflect::reflect(f);

   f.string_value = "test";
   f.nc_value.value = -10;

   SECTION("std::string() call without value capture") {
      REQUIRE_FALSE(f.string0_called);
      REQUIRE_NOTHROW(reflected["String0"]());
      REQUIRE(f.string0_called);
   }

   SECTION("std::string() call with value capture") {
      std::string value;
      REQUIRE(value != f.string_value);

      REQUIRE_NOTHROW([&]() {
         std::string value2 = reflected["String0"]();
         value = value2;
      }());
      REQUIRE(value == f.string_value);
   }

   SECTION("std::string() call with double value capture") {
      double value;
      REQUIRE_THROWS_AS(
         value = reflected["String0"](),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.string0_called);
   }

   SECTION("std::string() call without value capture via invoke<std::string>") {
      REQUIRE_FALSE(f.string0_called);
      REQUIRE_NOTHROW(reflected["String0"].invoke<std::string>());
      REQUIRE(f.string0_called);
   }

   SECTION("std::string() call via invoke<std::string> with value capture") {
      std::string value;
      REQUIRE(value != f.string_value);

      REQUIRE_NOTHROW(value = reflected["String0"].invoke<std::string>());
      REQUIRE(value == f.string_value);
   }

   SECTION("std::string() call without value capture via invoke<void>") {
      REQUIRE_FALSE(f.string0_called);
      REQUIRE_NOTHROW(reflected["String0"].invoke<void>());
      REQUIRE(f.string0_called);
   }

   SECTION("std::string() call via invoke<double> with value capture") {
      double value;
      REQUIRE_THROWS_AS(
         value = reflected["String0"].invoke<double>(),
         const reflect::invalid_function_arguments&
      );
      REQUIRE_FALSE(f.string0_called);
   }

   SECTION("std::string&() call via invoke<std::string&> with value capture") {
      std::string* value = nullptr;
      REQUIRE(value != &f.string_value);

      REQUIRE_NOTHROW([&]() {
         std::string& value2 = reflected["StringRef0"].invoke<std::string&>();
         value = &value2;
       }());
      REQUIRE(value == &f.string_value);
   }

   SECTION("const std::string&() call with value capture") {
      REQUIRE_NOTHROW([&]() {
         const std::string& value = reflected["ConstStringRef0"]();
         REQUIRE(value == f.string_value);
      }());
   }

   SECTION("const std::string&() call via invoke<const std::string&> with value capture") {
      const std::string* value = nullptr;
      REQUIRE(value != &f.string_value);

      REQUIRE_NOTHROW([&]() {
         const std::string& value2 = reflected["ConstStringRef0"].invoke<
            const std::string&
         >();
         value = &value2;
      }());
      REQUIRE(value == &f.string_value);
   }

   SECTION("NonCopyable() call with value capture") {
      NonCopyable value;
      REQUIRE(value.value != f.nc_value.value);

      REQUIRE_NOTHROW(value = reflected["NonCopyable0"]());
      REQUIRE(value.value == f.nc_value.value);
   }

   SECTION("NonCopyable&&() call with value capture") {
      NonCopyable value;
      REQUIRE(value.value != f.nc_value.value);

      REQUIRE_NOTHROW(value = reflected["NonCopyableRefRef0"]());
      REQUIRE(value.value == f.nc_value.value);
   }

   SECTION("NonCopyable&&() call with const& value capture") {
      REQUIRE_NOTHROW([&]() {
         const NonCopyable& value = reflected["NonCopyableRefRef0"]();
         REQUIRE(value.value == f.nc_value.value);
      }());
   }
}

TEST_CASE(
   "function calls with pod and arity 1",
   "[reflection][functions][arguments][basic]")
{
   Functions f;
   auto reflected = reflect::reflect(f);

   SECTION("void(int) call with int") {
      constexpr int value = 42;
      REQUIRE(f.value != value);
      REQUIRE_NOTHROW(reflected["Void1_Int"](value));
      REQUIRE(f.value == value);
   }

   SECTION("void(int) call with compatible type") {
      constexpr double value = 42;
      REQUIRE(f.value != (int)value);
      REQUIRE_NOTHROW(reflected["Void1_Int"](value));
      REQUIRE(f.value == (int)value);
   }

   SECTION("void(int) call with incompatible type") {
      REQUIRE_THROWS_AS(
         reflected["Void1_Int"](std::string{}),
         const reflect::invalid_function_arguments&
      );
   }

   SECTION("int(int) call with int") {
      constexpr int value = 42;
      int result = 0;
      REQUIRE_NOTHROW(result = reflected["Int1_Int"](value));
      REQUIRE(value == result);
   }

   SECTION("int(int) call with compatible type") {
      constexpr double value = 42;
      int result = 0;
      REQUIRE_NOTHROW(result = reflected["Int1_Int"](value));
      REQUIRE((int)value == result);
   }

   SECTION("int(int) call with incompatible type") {
      REQUIRE_THROWS_AS(
         reflected["Int1_Int"](std::string{}),
         const reflect::invalid_function_arguments&
      );
   }

   SECTION("void(int) called via invoke<void> with int") {
      constexpr int value = 42;
      REQUIRE(f.value != value);
      REQUIRE_NOTHROW(reflected["Void1_Int"].invoke<void>(value));
      REQUIRE(f.value == value);
   }

   SECTION("void(int) called via invoke<void> with compatible type") {
      constexpr double value = 42;
      REQUIRE(f.value != (int)value);
      REQUIRE_NOTHROW(reflected["Void1_Int"].invoke<void>(value));
      REQUIRE(f.value == (int)value);
   }

   SECTION("void(int) called via invoke<void> with incompatible type") {
      REQUIRE_THROWS_AS(
         reflected["Void1_Int"].invoke<void>(std::string{}),
         const reflect::invalid_function_arguments&
      );
   }

   SECTION("void(int) called via invoke<int>") {
      constexpr int value = 42;
      REQUIRE(f.value != value);
      REQUIRE_THROWS_AS(
         reflected["Void1_Int"].invoke<int>(value),
         const reflect::invalid_function_arguments&
      );
      REQUIRE(f.value != value);
   }

   SECTION("int(int) called via invoke<void> with int") {
      constexpr int value = 42;
      REQUIRE_NOTHROW(reflected["Int1_Int"].invoke<void>(value));
   }

   SECTION("int(int) called via invoke<double> with int") {
      constexpr int value = 42;
      double result = 0;
      REQUIRE_NOTHROW(result = reflected["Int1_Int"].invoke<double>(value));
      REQUIRE(value == (int)result);
   }

   SECTION("int(int) called via invoke<double> with compatible type") {
      constexpr double value = 42;
      double result = 0;
      REQUIRE_NOTHROW(result = reflected["Int1_Int"].invoke<double>(value));
      REQUIRE(value == result);
   }

   SECTION("int(int) called via invoke<std::string>") {
      REQUIRE_THROWS_AS(
         reflected["Int1_Int"].invoke<std::string>(10),
         const reflect::invalid_function_arguments&
      );
   }

}

TEST_CASE(
   "function calls with objects and arity 1",
   "[reflection][functions][arguments][basic][objects]")
{
   Functions f;
   auto reflected = reflect::reflect(f);

   SECTION("void(std::string) call with std::string") {
      const std::string test = "test";
      REQUIRE(f.string_value != test);
      REQUIRE_NOTHROW(reflected["Void1_String"](std::string{ test }));
      REQUIRE(f.string_value == test);
   }

   SECTION("void(std::string) call with compatible type") {
      const auto test = "test";
      REQUIRE(f.string_value != test);
      REQUIRE_NOTHROW(reflected["Void1_String"](test));
      REQUIRE(f.string_value == test);
   }

   SECTION("void(std::string) call with incompatible type") {
      const char *const value = f.string_value.data();
      REQUIRE(f.string_value.empty());
      REQUIRE_THROWS_AS(
         reflected["Void1_String"](10.0),
         const reflect::invalid_function_arguments&
      );
      REQUIRE(f.string_value.empty());
      REQUIRE(value == f.string_value.data());
   }

   SECTION("void(std::string&) call with std::string&") {
      std::string test = "test";
      REQUIRE(f.string_value != test);
      REQUIRE_NOTHROW(reflected["Void1_StringRef"](test));
      REQUIRE(f.string_value == test);
   }

   SECTION("void(std::string&) call with incompatible type") {
      const char *const value = f.string_value.data();
      REQUIRE(f.string_value.empty());
      REQUIRE_THROWS_AS(
         reflected["Void1_StringRef"](10.0),
         const reflect::invalid_function_arguments&
      );
      REQUIRE(f.string_value.empty());
      REQUIRE(value == f.string_value.data());
   }

   SECTION("void(const std::string&) call with const std::string&") {
      const std::string test = "test";
      REQUIRE(f.string_value != test);
      REQUIRE_NOTHROW(reflected["Void1_ConstStringRef"](test));
      REQUIRE(f.string_value == test);
   }

   SECTION("void(const std::string&) call with incompatible type") {
      const char *const value = f.string_value.data();
      REQUIRE(f.string_value.empty());
      REQUIRE_THROWS_AS(
         reflected["Void1_ConstStringRef"](10.0),
         const reflect::invalid_function_arguments&
      );
      REQUIRE(f.string_value.empty());
      REQUIRE(value == f.string_value.data());
   }

   SECTION("std::string(std::string) call with string") {
      const std::string test = "test";
      std::string result;
      REQUIRE_NOTHROW([&]() {
         std::string value = reflected["String1_String"](std::string{ test });
         result = value;
      }());
      REQUIRE(result == test);
   }

   SECTION("std::string(std::string) call with incompatible type") {
      REQUIRE_THROWS_AS(
         reflected["String1_String"](10.0),
         const reflect::invalid_function_arguments&
      );
   }

}
