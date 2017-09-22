
#include "catch.hpp"

#define TCB_PRETTY_PRINT_POINTERS_ARE_OPTIONALS
#include <tcb/pretty_print.hpp>

#include <array>
#include <deque>
#include <list>
#include <forward_list>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <map>
#include <unordered_map>

#ifdef TCB_PRETTY_PRINT_HAVE_CPP17
#include <optional>
#include <variant>
#endif

using namespace tcb::pretty_print;

TEST_CASE("Array-like STL types can be printed")
{
    std::ostringstream ss;

    SECTION("std::vector")
    {
        ss << std::vector<int>{1, 2, 3};
    }

    SECTION("std::array")
    {
        ss << std::array<int, 3>{1, 2, 3};
    }

    SECTION("C arrays")
    {
        int arr[3] = {1, 2, 3};
        ss << arr;
    }

    SECTION("std::list")
    {
        ss << std::list<int>{1, 2, 3};
    }

    SECTION("std::forward_list")
    {
        ss << std::forward_list<int>{1, 2, 3};
    }

    SECTION("std::deque")
    {
        ss << std::deque<int>{1, 2, 3};
    }

    SECTION("std::set")
    {
        ss << std::set<int>{1, 2, 3};
    }

    SECTION("std::multiset")
    {
        ss << std::multiset<int>{1, 2, 3};
    }

    SECTION("std::initializer_list")
    {
        const auto il = {1, 2, 3};
        ss << il;
    }

    REQUIRE(ss.str() == "[1, 2, 3]");
}

TEST_CASE("Empty containers are handled correctly")
{
    std::ostringstream ss;

    SECTION("empty std::vector")
    {
        ss << std::vector<int>{};
    }

    SECTION("empty std::array")
    {
        ss << std::array<int, 0>{};
    }

    REQUIRE(ss.str() == "[]");
}

TEST_CASE("std::unordered_sets can be printed")
{
    std::ostringstream ss;

    SECTION("std::unordered_set")
    {
        ss << std::unordered_set<int>{1, 2};
    }

    SECTION("std::unordered_multiset")
    {
        ss << std::unordered_multiset<int>{1, 2};
    }

    const bool ok = ss.str() == "[1, 2]" || ss.str() == "[2, 1]";
    REQUIRE(ok);
}

TEST_CASE("Strings are printed as strings, not arrays")
{
    std::ostringstream ss;

    SECTION("String literals")
    {
        const char str[] = "test";
        ss << "test";

    }

    SECTION("std::string")
    {
        ss << std::string("test");
    }

    REQUIRE(ss.str() == "test");
}

TEST_CASE("Map-like STL types can be printed")
{
    std::ostringstream ss;

    SECTION("std::map")
    {
        ss << std::map<int, std::string>{{1, "one"}, {2, "two"}};
    }

    SECTION("std::multimap")
    {
        ss << std::multimap<int, std::string>{{1, "one"}, {2, "two"}};
    }

    SECTION("std::vector<std::pair>")
    {
        ss << std::vector<std::pair<int, std::string>>{{1, "one"}, {2, "two"}};
    }

    REQUIRE(ss.str() == "{1: \"one\", 2: \"two\"}");
}

TEST_CASE("Unordered map-like STL types can be printed")
{
    std::stringstream ss;

    SECTION("std::unordered_map")
    {
        ss << std::unordered_map<int, std::string>{{1, "one"}, {2, "two"}};
    }

    SECTION("std::unordered_multimap")
    {
        ss << std::unordered_multimap<int, std::string>{{1, "one"}, {2, "two"}};
    }

    const bool ok = ss.str() == R"({1: "one", 2: "two"})"
                     || ss.str() == R"({2: "two", 1: "one"})";
    REQUIRE(ok);
}

#ifdef TCB_PRETTY_PRINT_HAVE_CPP17
// This is a custom type designed to work with structured bindings
struct tuple_like {
    int get_i() const { return i; }
    float get_f() const { return f; }

private:
    int i = 1;
    float f = 3.14;
};

template <>
struct std::tuple_size<tuple_like> : std::integral_constant<std::size_t, 2> {};

template <>
struct std::tuple_element<0, tuple_like>
{
    using type = int;
};

template <>
struct std::tuple_element<1, tuple_like>
{
    using type = float;
};

template <std::size_t I>
decltype(auto) get(const tuple_like& t)
{
    if constexpr (I == 0) {
        return t.get_i();
    } else if (I == 1) {
        return t.get_f();
    }
}
#endif

TEST_CASE("Tuple-like types can be printed")
{
    std::stringstream ss;

    SECTION("std::tuple") {
        ss << std::tuple<int, float>{1, 3.14f};
    }

    SECTION("std::pair") {
        ss << std::pair<int, float>{1, 3.14f};
    }

#ifdef TCB_PRETTY_PRINT_HAVE_CPP17
    SECTION("tuple_like") {
        ss << tuple_like{};
    }
#endif

    REQUIRE(ss.str() == "(1, 3.14)");
}

TEST_CASE("Empty tuples are handled correctly")
{
    std::stringstream ss;
    ss << std::tuple<>{};
    REQUIRE(ss.str() == "()");
}

TEST_CASE("Nested array-like types can printed")
{
    std::ostringstream ss;

    SECTION("std::vector<std::vector<int>>")
    {
        using mat_t = std::vector<std::vector<int>>;
        ss << mat_t{{1, 2, 3}, {4, 5, 6}};
    }

    SECTION("nested C arrays")
    {
        int arr[][3] = {1, 2, 3, 4, 5, 6};
        ss << arr;
    }

    REQUIRE(ss.str() == "[[1, 2, 3], [4, 5, 6]]");
}

TEST_CASE("Optional-like types can be printed")
{
    std::ostringstream ss;

    SECTION("...when disengaged")
    {
#ifdef TCB_PRETTY_PRINT_HAVE_CPP17
        SECTION("std::optional")
        {
            ss << std::optional<int>{};
        }
#endif
        SECTION("pointers")
        {
            int *p = nullptr;
            ss << p;
        }

        REQUIRE(ss.str() == "--");
    }

    SECTION("...when engaged")
    {
#ifdef TCB_PRETTY_PRINT_HAVE_CPP17
        SECTION("std::optional")
        {
            ss << std::optional<int>{3};
        }
#endif

        SECTION("pointers")
        {
            int i = 3;
            ss << &i;
        }

        REQUIRE(ss.str() == "3");
    }
}

#ifdef TCB_PRETTY_PRINT_HAVE_CPP17
TEST_CASE("std::variants can be printed")
{
    std::ostringstream ss;
    std::variant<int, float, std::string> v{3.14f};
    ss << v;
    REQUIRE(ss.str() == "3.14");
}
#endif
