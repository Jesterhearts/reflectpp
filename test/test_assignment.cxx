#include "testclasses.h"

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