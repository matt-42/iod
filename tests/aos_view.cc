
#include <iostream>
#include <iod/aos_view.hh>
#include "symbols.hh"
#include <cassert>

// aos_view allows to view several arrays as a unique array of structures.
// Any class with a size() method and operator[](int i) is supported.

// The view only hold references to the arrays so no deep array copy is involved.

int main()
{
  using namespace iod;
  using namespace s;

  // Basic usage.
  {
    std::vector<int> A = {1,2,3,4};
    std::vector<char> B = {'a', 'b', 'c', 'd'};

    // Map array A to struct member a and array B to member b.
    auto v = iod::aos_view(_a = A, _b = B);

    assert(v[2].a == 3);
    assert(v[2].b == 'c');

    // For range loop 
    for (auto x : v)
      std::cout << x.a << " " << x.b << std::endl;
    
    for (size_t i = 0; i < v.size(); i++)
      std::cout << v[i].a << " " << v[i].b << std::endl;

    // Views are writable.
    v[2].a = 42; // Write to A[2]
    v[2].b = 'z'; // Write to B[2]

    assert(A[2] == 42);
    assert(B[2] == 'z');
  }

  // Test if it does not copy arrays.
  {

    struct Arr1
    {
      Arr1() {};
      Arr1(const Arr1& e) = delete;
      Arr1& operator=(const Arr1& e) = delete;

      decltype(auto) operator[](int) { return 42; }
      int size() { return 1; }
    };
    
    Arr1 A;
    Arr1 B;
    auto v = iod::aos_view(s::_a = A);

    assert(v[0].a == 42);
  }

  // const vectors lead to read only views.
  {
    const std::vector<int> A = {1,2,3,4};
    const std::vector<char> B = {'a', 'b', 'c', 'd'};

    auto v = iod::aos_view(s::_a = A, s::_b = B);

    assert(v[2].a == 3);
    assert(v[2].b == 'c');

    // cannot assign to non-static data member 'a' with const-qualified type 'value_type
    // ' (aka 'const int &')
    // v[2].a = 42;
  }

  // Mix computed vectors and in-memory vectors.
  {
    std::vector<int> A = {1,2,3,4};

    auto v = aos_view(_a = A,
                      _b = [&] (int i) { return A[i] * 2; });

    for (auto x : v)
      std::cout << x.a << " " << x.b << std::endl;

    v[3].a = 12;
    assert(v[3].a == 12);

    // error: expression is not assignable.
    // v[3].b = 12;
  }


  // Attach code.
  {
    std::vector<int> A = {1,2,3,4};

    auto v = aos_view(_a = A,
                      _b = [&] (int i, std::string s) {
                        std::cout << A[i] << s << std::endl ; });

    v[2].b("xx");

    for (auto x : v)
      x.b(" test");
  }

  // inplace sort. Not implemented.
  // {
  //   std::vector<int> A = {4,3,1,2};
  //   std::vector<char> B = {'a', 'b', 'c', 'd'};

  //   auto v = aos_view(_a = A , _b = B);

  //   // Sort wrt a member of the SOA.
  //   sort(v, _a);
  //   // Sort wrt a function.
  //   sort(v, [] (auto x, auto y) { return x.a < y.a; });
    
  //   for (auto x : v)
  //     std::cout << x.a << " " << x.b << std::endl;
  // }
}
