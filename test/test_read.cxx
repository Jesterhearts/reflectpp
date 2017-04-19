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
      REQUIRE_NOTHROW([&]() { std::string test2 = reflected["string"]; test = test2; }());
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

      REQUIRE_NOTHROW([&]() { NonCopyable nc2{ reflected["i"] }; nc = std::move(nc2); }());
      REQUIRE(nc.m == f.ncmem.m);
   }

   SECTION("member read to incompatible type") {
      int i;
      REQUIRE_THROWS_AS(i = reflected["string"], const reflect::invalid_requested_member_type&);
   }

}

/*
TODO
TEST_CASE("static member read", "[reflection][read][static]") {
   Static s;
   auto reflected = reflect::reflect(s);

   SECTION("static variable read") {
      REQUIRE_NOTHROW(reflected["abc"].get_type());
   }

   SECTION("static function read") {
      REQUIRE_NOTHROW(reflected["Funky"].get_type());
   }

}

TEST_CASE("const member read", "[reflection][read][const]") {
   Const const_;
   auto reflected = reflect::reflect(const_);

   SECTION("const member variable read") {
      REQUIRE_NOTHROW(reflected["_const"].get_type());
   }

   SECTION("static const member variable read") {
      REQUIRE_NOTHROW(reflected["_static"].get_type());
   }

}

TEST_CASE("array and pointer member read", "[reflection][read][arrays]") {
   Arrays arrays;
   auto reflected = reflect::reflect(arrays);

   SECTION("pointer member read") {
      REQUIRE_NOTHROW(reflected["iptr"].get_type());
   }

   SECTION("dynamic array member read") {
      REQUIRE_NOTHROW(reflected["iarr"].get_type());
   }

   SECTION("sized array member read") {
      REQUIRE_NOTHROW(reflected["iarr10"].get_type());
   }

}

TEST_CASE("empty object member read", "[reflection][read][empty]") {
   SECTION("non static member read") {
      auto reflected = reflect::reflect<Foo>();
      REQUIRE_NOTHROW(reflected["i"].get_type());
   }

   SECTION("static const member read") {
      auto reflected = reflect::reflect<Const>();
      REQUIRE_NOTHROW(reflected["_static"].get_type());
   }

   SECTION("static member read") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_NOTHROW(reflected["abc"].get_type());
   }

   SECTION("static function read") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_NOTHROW(reflected["Funky"].get_type());
   }

   SECTION("non-existent member read") {
      auto reflected = reflect::reflect<Static>();
      REQUIRE_THROWS_AS(reflected["invalid identifier"], const reflect::member_access_error&);
   }

}
*/
