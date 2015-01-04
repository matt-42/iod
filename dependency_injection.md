Non Intrusive Static Recursive Dependency Injection for C++14
=========================================

The IOD library implements a generic recursive dependency injection pattern.
Given a C++ function f tacking a list of N arguments of types Ai with i >= 0 and i < N.
and a set S of objects to inject, the di_call primitive calls f after building its argument
list using the following, for each i:

  - If S contains an element of type Ai, place it in the list.
  - Else if S contains an element e such that the method e.make returns the type Ai,
         use this same di algorithm on e.make to build an element of type Ai.
  - Else if the static method Ai::make exists,
         use this same di algorithm on Ai::make to build an element of type Ai.
  - Else return an error.

This algorithm runs at compile time so its cost at compile time is negligible.

# How to use it

Pull argument list from a set of values:

```c++

#include <iod/di.hh>

[...]

auto f = [] (int x, float y) {};

int a = 0;
float b = 1.f;
double c = 2.;
di_call(f, a, b, c); // f(0, 1.f)

```

Use factories to fulfill the dependencies.

```c++

struct int_factory
{
  int make(float f) { return f + 1; }
};

struct float_factory
{
  float make() { return 2.f; }
};

auto f = [] (int x, float y) {};
int_factory int_f;
float_factory float_f;
iod::di_call(f, int_f, float_f);
// Instantiate one float f with ff.make(), pass it to if.make to instantiate an int i,
// and finally call f(i, f);
```

Or the static make method:

```c++

struct A {
  static A make() {
    std::cout << " make " << std::endl;
    return A();
  }
};

auto f2 = [] (A a) {};
iod::di_call(f2);
// Instantiate A with A::make and pass it to f2.
```

# Why another C++ DI library ?

Many other C++ dependencies injection already exists, but this implementation
has several advantage that may better suit your needs:
   - Tiny: less than 300 lines of C++14 code
   - Non intrusive
   - Easy to use
   - Recursive
   - Compile-time

