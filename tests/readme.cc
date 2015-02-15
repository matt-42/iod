#include <iostream>
#include <iod/symbol.hh>

iod_define_symbol(a); // Refer to members and methods a with symbol _a
iod_define_symbol(b); // Refer to members and methods b with symbol _b
iod_define_symbol(c); // Refer to members and methods b with symbol _c

int main() {

  // Symbols are located in namespace s to avoid name clash.
  using s::_a;
  using s::_b;
  using s::_c;

  auto print_member = [] (auto& obj, auto& m)
                      {
                        std::cout << "obj." << m.name()
                                  << " == " << m.member_access(obj) << std::endl;
                      };

  struct { int a; int b; int c; } obj{21, 42, 84};
  print_member(obj, _a); // Prints "obj.a == 21"
  print_member(obj, _b); // Prints "obj.b == 42"
  print_member(obj, _c); // Prints "obj.c == 84"
}
