#include <iostream>
#include <iod/symbol.hh>

iod_define_symbol(a, _A); // Refer to members and methods a with symbol _A
iod_define_symbol(b, _B); // Refer to members and methods b with symbol _B
iod_define_symbol(c, _C); // Refer to members and methods b with symbol _C

int main() {

  // Symbols are located in namespace s to avoid name clash.
  using s::_A;
  using s::_B;
  using s::_C;

  auto print_member = [] (auto& obj, auto& m)
                      {
                        std::cout << "obj." << m.name()
                                  << " == " << m.member_access(obj) << std::endl;
                      };

  struct { int a; int b; int c; } obj{21, 42, 84};
  print_member(obj, _A); // Prints "obj.a == 21"
  print_member(obj, _B); // Prints "obj.b == 42"
  print_member(obj, _C); // Prints "obj.c == 84"
}
