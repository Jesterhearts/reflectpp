#include "testclasses.h"

TEST_CASE("basic member touch", "[reflection][touch][basic]") {
   Foo f{};
   auto reflected = reflect::reflect(f);

   SECTION("member pod variable touch") {
      REQUIRE_NOTHROW(reflected["i"].get_type());
   }

   SECTION("member object variable touch") {
      REQUIRE_NOTHROW(reflected["string"].get_type());
   }

   SECTION("member function touch") {
      REQUIRE_NOTHROW(reflected["Bar"].get_type());
   }

   SECTION("non-existent member touch") {
      REQUIRE_THROWS_AS(reflected["invalid identifier"], const reflect::member_access_error&);
   }

}

TEST_CASE("static member touch", "[reflection][touch][static]") {
   Static s;
   auto reflected = reflect::reflect(s);

   SECTION("static variable touch") {
      REQUIRE_NOTHROW(reflected["abc"].get_type());
   }

   SECTION("static function touch") {
      REQUIRE_NOTHROW(reflected["Funky"].get_type());
   }

}

TEST_CASE("const member touch", "[reflection][touch][const]") {
   Const const_;
   auto reflected = reflect::reflect(const_);

   SECTION("const member variable touch") {
      REQUIRE_NOTHROW(reflected["_const"].get_type());
   }

   SECTION("static const member variable touch") {
      REQUIRE_NOTHROW(reflected["_static"].get_type());
   }

}

TEST_CASE("array and pointer member touch", "[reflection][touch][arrays]") {
   Arrays arrays;
   auto reflected = reflect::reflect(arrays);

   SECTION("pointer member touch") {
      REQUIRE_NOTHROW(reflected["iptr"].get_type());
   }

   SECTION("dynamic array member touch") {
      REQUIRE_NOTHROW(reflected["iarr"].get_type());
   }

   SECTION("sized array member touch") {
      REQUIRE_NOTHROW(reflected["iarr10"].get_type());
   }

}

TEST_CASE("empty object member touch", "[reflection][touch][empty]") {
   SECTION("non static member touch") {
      auto reflected = reflect::reflect<Foo>();
      REQUIRE_NOTHROW(reflected["i"].get_type());
   }

   SECTION("static const member touch") {
      auto reflected = reflect::reflect<Const>();
      REQUIRE_NOTHROW(reflected["_static"].get_type());
   }

   SECTION("static member touch") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_NOTHROW(reflected["abc"].get_type());
   }

   SECTION("static function touch") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_NOTHROW(reflected["Funky"].get_type());
   }

   SECTION("non-existent member touch") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_THROWS_AS(reflected["invalid identifier"], const reflect::member_access_error&);
   }

}

