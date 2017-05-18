#include "testclasses.h"

TEST_CASE("basic member read", "[reflection][read][basic]") {
   Foo f{};
   auto reflected = reflect::reflect(f);

   SECTION("member pod read") {
      int i = f.i + 1;
      REQUIRE(i != f.i);

      REQUIRE_NOTHROW(i = reflected["i"]);
      REQUIRE(i == f.i);
   }

   SECTION("member object read") {
      std::string test = std::string{};
      f.string = "test";
      REQUIRE(test != f.string);

      //TODO it should(?) be possible to do `test = reflected["string"]`
      // compilation currently fails
      REQUIRE_NOTHROW([&]() {
         std::string test2 = reflected["string"];
         test = test2;
      }());
      REQUIRE(test == f.string);
   }

   SECTION("member pod read to smaller type") {
      char c = f.i + 1;
      REQUIRE(c != f.i);

      REQUIRE_NOTHROW(c = reflected["i"]);
      REQUIRE(c == f.i);
   }

   SECTION("member pod read to larger type") {
      std::size_t s = f.i + 1;
      REQUIRE(s != f.i);

      REQUIRE_NOTHROW(s = reflected["i"]);
      REQUIRE(s == f.i);
   }

   SECTION("member read to compatible type") {
      NonCopyable nc{ f.ncmem.m + 1 };
      REQUIRE(nc.m != f.ncmem.m);

      REQUIRE_NOTHROW([&]() {
         NonCopyable nc2{ reflected["i"] };
         nc = std::move(nc2);
      }());
      REQUIRE(nc.m == f.ncmem.m);
   }

   SECTION("member read to incompatible type") {
      int i;
      REQUIRE_THROWS_AS(
         i = reflected["string"],
         const reflect::invalid_requested_member_type&
      );
   }

}

TEST_CASE("static member read", "[reflection][read][static]") {
   Static s;
   auto reflected = reflect::reflect(s);

   Static::abc = 1000;
   Static::string = "reflection.read.static";

   SECTION("static pod read") {
      int value = Static::abc + 1;
      REQUIRE(value != Static::abc);

      REQUIRE_NOTHROW(value = reflected["abc"]);
      REQUIRE(value == Static::abc);
   }

   SECTION("static object read") {
      std::string value;
      REQUIRE(value != Static::string);

      //TODO it should(?) be possible to do `value = reflected["string"]`
      // compilation currently fails
      REQUIRE_NOTHROW([&]() {
         std::string value2 = reflected["string"];
         value = value2; }()
            );
      REQUIRE(value == Static::string);
   }

   SECTION("static object read to incompatible type") {
      int i;
      REQUIRE_THROWS_AS(
         i = reflected["string"],
         const reflect::invalid_requested_member_type&
      );
   }

}

TEST_CASE("const member read", "[reflection][read][const]") {
   Const const_;
   auto reflected = reflect::reflect(const_);

   SECTION("const member variable read") {
      int value = const_._const + 1;
      REQUIRE(value != const_._const);

      REQUIRE_NOTHROW(value = reflected["_const"]);
      REQUIRE(value == const_._const);
   }

   SECTION("static const member variable read") {
      int value = Const::_static + 1;
      REQUIRE(value != Const::_static);

      REQUIRE_NOTHROW(value = reflected["_static"]);
      REQUIRE(value == Const::_static);
   }

}

TEST_CASE("array and pointer member read", "[reflection][read][arrays]") {
   Arrays arrays;
   auto reflected = reflect::reflect(arrays);

   SECTION("pointer member read") {
      int* value = nullptr;
      REQUIRE(value != arrays.iptr);

      REQUIRE_NOTHROW(value = reflected["iptr"]);
      REQUIRE(value == arrays.iptr);
   }

   SECTION("dynamic array member read") {
      int* value = nullptr;
      REQUIRE(value != arrays.iarr);

      REQUIRE_NOTHROW(value = reflected["iarr"]);
      REQUIRE(value == arrays.iarr);
   }

   SECTION("sized array member read") {
      int(*value)[10] = nullptr;
      REQUIRE(value != &arrays.iarr10);

      REQUIRE_NOTHROW([&]() {
         int(&value2)[10] = reflected["iarr10"];
         value = &value2;
      }());
      REQUIRE(value == &arrays.iarr10);
   }

   SECTION("static sized array member read") {
      int(*value)[5] = nullptr;
      REQUIRE(value != &arrays.static5);

      REQUIRE_NOTHROW([&]() {
         int(&value2)[5] = reflected["static5"];
         value = &value2;
      }());
      REQUIRE(value == &arrays.static5);
   }

   SECTION("invalid sized array member read") {
      REQUIRE_THROWS_AS(
         [&]() { int(&value)[2] = reflected["iarr10"]; }(),
         const reflect::invalid_requested_member_type&
      );
   }

}

TEST_CASE("empty object member read", "[reflection][read][empty]") {
   SECTION("non static member read") {
      auto reflected = reflect::reflect<Foo>();

      int value = 42;
      REQUIRE_THROWS_AS(
         value = reflected["i"],
         const reflect::member_access_error&
      );
      REQUIRE(value == 42);
   }

   SECTION("static const member read") {
      auto reflected = reflect::reflect<Const>();

      int value = Const::_static + 1;
      REQUIRE(value != Const::_static);

      REQUIRE_NOTHROW(value = reflected["_static"]);
      REQUIRE(value == Const::_static);
   }

   SECTION("static member read") {
      auto reflected = reflect::reflect<Static>();

      int value = Static::abc + 1;
      REQUIRE(value != Static::abc);

      REQUIRE_NOTHROW(value = reflected["abc"]);
      REQUIRE(value == Static::abc);
   }

   SECTION("static member read from incompatible type") {
      auto reflected = reflect::reflect<Static>();

      int value;
      REQUIRE_THROWS_AS(
         value = reflected["string"],
         const reflect::member_access_error&
      );
   }

}
