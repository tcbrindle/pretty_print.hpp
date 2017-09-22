
# pretty_print.hpp #

`pretty_print.hpp` is (yet another) single-header C++14 library for printing
STL-compatible container types to IO streams.

It provides Python-style formatting for range-like types, map-like types and
tuple-like types. It also handles printing of `std::optional`s and `std::variants`
if using C++17.

It uses type traits to detect the properties of the containers passed to it, so
should be compatible with custom containers so long as they provide STL-compatible
interfaces.

## Usage ##

There is no installation required: simply copy the file `include/tcb/pretty_print.hpp`
into your own sources. Everything else in this repo is just the testing machinery.

By default, just including the header will do absolutely nothing. This is deliberate,
to avoid changing the behaviour of existing code in case the header gets transitively
included somewhere. In order to actually use the `operator<<()` overloads provided,
you'll need to bring them into scope via a `using` directive. Either of the following
will do so:

```cpp
using namespace tcb::pretty_print; // Bring in everything from this header
using tcb::pretty_print::operator<<; // Bring in just operator<<()
```

Think of this much like writing `using namespace std::string_literals` in order
to use the standard library's UDLs.

## Sample output ##

### Ranges ###

For range-like types such as `std::vector` or `std::list` (specifically, types
for which `std::begin()` returns a type whose `std::iterator_traits::iterator_category`
trait is derived from `std::input_iterator_tag`), each element is
streamed, separated by commas, between square brackets. For example:

```cpp
std::vector<int> vec{1, 2, 3};
std::cout << vec;
// Prints [1, 2, 3]
```

The range printing overload will be disabled if a type can already be streamed -- that
is, if `ostream << T` is a valid call. This is to avoid ambiguous calls, and also
allows `std::string`s to continue to be printed as strings, not ranges.

Raw C arrays can already be streamed just using `<iostream>`. Unfortunately, they
undergo pointer decay and so all you'll get is the memory address of the start of
the array. This library fixes this, and so C arrays of known size will be printed
as expected:

```cpp
const int arr[] = {1, 2, 3};
std::cout << arr;
// Prints [1, 2, 3];
```

However, `char` arrays continue to be treated as strings, not ranges.

### Maps ###

Range-like types which additionally have a `std::map`-like interface (specifically, if
their dereferenced iterator types have `first` and `second` members) are printed
like so:

```cpp
std::unordered_map<int, std::string> map{{1, "one"}, {2, "two"}};
std::cout << map;
// Prints {1: "one", 2: "two"} or {2: "two", 1: "one"}
// (see below for the details about string quoting)
```

This definition of "map-like" was chosen to include a vector of pairs, commonly
used as a "flat map". For example:

```cpp
std::vector<std::pair<int, std::string>> map{{1, "one"}, {2, "two"}};
std::cout << map;
// Prints {1: "one", 2: "two"}
```

### Tuples ###

Tuple-like types are printed between a pair of round parentheses. This includes
`std::tuple` and `std::pair`. For example:

```cpp
std::tuple<int, float> t{1, 3.14f};
std::cout << t;
// Prints (1, 3.14)
```

Custom types which implement `std::tuple_size`, `std::tuple_element` and
(non-member) `get()` specialisations for C++17 structured bindings will be
treated as tuples. Types which meet both the "range-like" and "tuple-like"
requirements (such as `std::array`) will be treated as ranges.


### Optionals ###

Types which have a dereference operator and which are explicitly convertible
to `bool` will be printed as follows:

```cpp
std::optional<int> o{}; // disengaged
std::cout << o;
// Prints --
o = 1; // now engaged
std::cout << o;
// Prints 1
```

This definition of "optional" includes `std::optional` from C++17,
`std::experimental::optional` from the library fundamentals TS, and `boost::optional`.
It also includes all pointer types, but don't be alarmed -- by default, pointers are
deliberately excluded (see "customisation" below).

### Variants ###

If using C++17, `std::variant`s can be printed as expected:

```cpp
std::variant<int, float, std::string> var{3.14f};
std::cout << var;
// Prints 3.14
```

Note that this requires that all alternatives within the variant are themselves streamable.

## Customisation ##

### String quoting ###

By default, standard strings (that is, specialisations of `std::basic_string`,
specialisations of `std::basic_string_view` and pointers to `char`) inside
containers will be quoted using C++14's [`std::quoted()`](http://en.cppreference.com/w/cpp/io/manip/quoted).

For example:

```cpp
std::vector<std::string> vec{"one", "two"};
std::cout << vec;
// Prints ["one", "two"] (with quotes)
```

This handles embedded newlines and embedded quotes, and is useful for debugging.
However, if you don't like it, string quoting can be disabled by defining the
preprocessor symbol `TCB_PRETTY_PRINT_NO_STRING_QUOTING` before `#include`-ing
the header. For example:

```cpp
#define TCB_PRETTY_PRINT_NO_STRING_QUOTING
#include "pretty_print.hpp"

using namespace tcb::pretty_print;

std::vector<std::string> vec{"one", "two};
std::cout << vec;
// Prints [one, two] (no quotes)
```

### Pointers as optionals ###

C++17's `std::optional` cannot contain references. Instead, this role is served
by raw pointers. As mentioned above, pointers can already be streamed by default
with the standard IO stream facilities, which simply prints the memory address.
If desired, this library can treat them as optionals instead, by defining the preprocessor symbol
`TCB_PRETTY_PRINT_POINTERS_ARE_OPTIONALS` before `#include`-ing the header. For
example:

```cpp
#define TCB_PRETTY_PRINT_POINTERS_ARE_OPTIONALS
#include "pretty_print.hpp"

using namespace tcb::pretty_print;

int *p = nullptr;
std::cout << p;
// Prints --

int i = 3;
p = &i;
std::cout << p;
// Prints 3
```

Note that `char` pointers will always be treated as strings, and never as optionals.
Do not attempt to print a `char*` which is equal to `nullptr`!

## Other facilities ##

This header provides an implementation of [`ostream_joiner`](http://en.cppreference.com/w/cpp/experimental/ostream_joiner)
from the Library Fundamentals V2 TS. This is pretty useful by itself even if you
don't want to use the rest of the library.

Also included is a `to_string()` function which is implemented as

```cpp
template <typename T>
std::string to_string(const T& t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}
```

this might be useful for logging or debugging, but isn't likely to be very efficient.


## Known Issues ##

* At present, this library has only been tested using narrow output streams.
  Everything is templated on the stream type, but the use of wide streams
  (e.g `std::wcout`) has not been tested and probably does not work.

* Detection of tuples currently only checks that a specialisation of
  `std::tuple_size` exists and has a static member named `value` of type `size_t`. 
  It ought to check for `std::tuple_element` and `get()` specialisations as well.


## Bugs? Patches? Suggestions? ##

Please use the Github issue tracker.

## Alternatives ##

STL container pretty-printers are ten a penny, so if you don't like this one
there are plenty of others. For example:

* [Stringify](https://github.com/asit-dhal/stringify) by Asit Kumar Dhal
* [printers](https://github.com/mnciitbhu/printers) by Govind Sahai
* [cxx-prettyprint](https://louisdx.github.io/cxx-prettyprint/) by Louis Delacroix
* [pretty_printer](https://onedrive.live.com/?id=E66E02DC83EFB165%21292&cid=E66E02DC83EFB165),
  the example source code from [this video](http://channel9.msdn.com/Shows/Going+Deep/C9-Lectures-Stephan-T-Lavavej-Advanced-STL-6-of-n)
  by Stephan T. Lavavej.



