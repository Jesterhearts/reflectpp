#include "testclasses.h"

#include <memory>

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

