#include <iod/sio.hh>
#include <iod/sio_utils.hh>
#include <iod/json.hh>
#include "symbols.hh"

int main()
{
  using namespace iod;
  auto a = D(_age = 1,
             _c = 23,
             _name = D(_a = 23));
  auto b = D(_age = 42,
             _cp = 23,
             _name = D(_b = 23));

  
  auto c = deep_merge_2_sios(a, b);
  //auto d = iod::apply(std::make_tuple(a, b), deep_merge_2_sios);
  //auto d = iod::apply(a, b, [] (auto a, auto b) { deep_merge_2_sios(a, b); });
  //auto d = iod::apply(std::make_tuple(a, b), [] (auto a, auto b) { return deep_merge_2_sios(a, b); });
  auto d = deep_merge_sios_in_tuple(std::make_tuple(a, b));
  std::cout << json_encode(d) << std::endl;
}
