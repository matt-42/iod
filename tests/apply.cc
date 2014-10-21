#include <iostream>
#include <iod/apply.hh>

iod_define_symbol(name);
iod_define_symbol(age);

int main()
{
  auto t = std::make_tuple(1, 2, 3);
  const auto u = std::make_tuple(1, 2, 3);
  iod::apply(t, [] (int& x, int y, int s) { std::cout << s << std::endl; });
  iod::apply(u, [] (const int& x, int y, int s) { std::cout << s << std::endl; });
  
  auto o1 = iod::D(s::name = "xxx", s::age = 12);
  const auto o2 = iod::D(s::name = "xxx", s::age = 12);
  iod::apply(o1, [] (std::string& s, int& a) { std::cout << s << std::endl; });
  iod::apply(o2, [] (const std::string& s, const int& a) { std::cout << s << std::endl; });

  iod::proxy_apply(t, [] (int x) { return x + 2; }, [] (int x, int y, int z) { std::cout << z << std::endl; });

}
