#include <iostream>
#include <cassert>

#include <iod/di.hh>


struct A {
  static A instantiate() {
    std::cout << " instantiate A" << std::endl;
    return A();
  }
};

struct C {
  static C instantiate() {
    std::cout << " instantiate C " << std::endl;
    return C();
  }
};

struct B {
  typedef B self;

  B() {}
  
  static B instantiate(C) {
    std::cout << " instantiate B" << std::endl;
    return B();
  }
};


int mref_int = 42;

struct Mref {
  int& instantiate() {
    return mref_int;
  }
};

struct D_factory;

struct D {
  typedef D_factory factory_type;
};

struct D_factory
{
  D_factory() {}
  
  D& instantiate()
  {
    std::cout << " instantiate D" << std::endl;
    return d;
  }

  D d;
};

struct NC
{
  NC() : copy(false) {}

  NC(const NC& n) = delete;
  NC& operator=(const NC& n) = delete;
  
  bool copy;
};



void fun1()
{
}

void fun2(int)
{
}

void fun3(float x, int y, A)
{
  std::cout << x << " " << y << std::endl;
}

void fun4(B)
{
  std::cout << "fun4" << std::endl;
}


void fun5(D)
{
  std::cout << "fun5" << std::endl;
}


void fun5_2(D&)
{
  std::cout << "fun5" << std::endl;
}

void fun5_3(const D&)
{
  std::cout << "fun5" << std::endl;
}

void fun5_4(const D)
{
  std::cout << "fun5" << std::endl;
}


void fun6(float)
{
}

void fun7(A)
{
  std::cout << "fun7" << std::endl;
}


void fun8(NC&)
{
  std::cout << "fun8" << std::endl;
}

struct int_factory
{
  int instantiate(float f) { return f + 1; }
};

struct float_factory
{
  float instantiate() { return 2.f; }
};


struct with_data_instance
{
  with_data_instance() { s = "trest"; };
  with_data_instance(std::string x) { s = x; };
  std::string s;
};

struct with_data
{

  with_data_instance instantiate() {
    std::cout << "instantiate " << s << std::endl;
    return with_data_instance{s};

  }
  std::string s;
};


struct with_data_instance2
{

  static with_data_instance2 instantiate(with_data& wd) {
    std::cout << "instantiate2 " << wd.s << std::endl;
    return with_data_instance2{wd};
  }

  with_data& w;
};


struct with_data_instance3
{

  static with_data_instance3 instantiate(with_data& wd) {
    std::cout << "instantiate3 " << wd.s << std::endl;
    return with_data_instance3{wd};
  }

  with_data& w;
};


void fun9(with_data_instance& wd)
{
  std::cout << "fun9: " << wd.s << std::endl;
}


void fun10(with_data_instance2 wd)
{
  std::cout << "fun10: " << wd.w.s << std::endl;
}

int main()
{
  float x = 1;
  int y = 2;

  // auto t=  std::make_tuple([&] () -> decltype(auto) { return y; });
  // typedef decltype(t) T2;
  // static_assert(iod::di::tuple_embeds_fun_with_return_type<T2, int>::value, "");
  iod::di_call(&fun1);
  iod::di_call(&fun1, 1);
  iod::di_call(&fun2, 1);
  iod::di_call(&fun3, y, x);
  iod::di_call(&fun3, y, x, A());

  iod::di_call(&fun4, B());
  iod::di_call(&fun4, D_factory());  

  iod::di_call(&fun5, D_factory());
  iod::di_call(fun6, 2.f);
  iod::di_call(fun7);

  //Check if no copy happend.
  NC nc;
  iod::di_call(fun8, nc);
 
  auto f = [] (int, float) {};
  int_factory int_f;
  float_factory float_f;
  iod::di_call(f, int_f, float_f);

  auto wd = with_data{"toto"};
  std::cout << "before: " << wd.s << std::endl;
  iod::di_call(fun9, with_data{"toto"});

  iod::di_call(fun10, with_data{"toto"});

  y = 10;
  auto f2 = [] (int& x) { x+= 20; };
  iod::di_call(f2, y);
  assert(y == 30);

  // test reference.
  auto f3 = [] (int& x) { assert(&x == &mref_int); };
  iod::di_call(f3, Mref());


  auto f4 = [] (with_data_instance2, with_data_instance3) {};
  iod::di_call(f4, wd);
}
