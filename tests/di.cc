#include <iostream>

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
  static B instantiate(C c) {
    std::cout << " instantiate B" << std::endl;
    return B();
  }
};

struct D_factory;

struct D {
  typedef D_factory factory_type;
};

struct D_factory
{
  D instantiate()
  {
    std::cout << " instantiate D" << std::endl;
    return D();
  }

};

void fun1()
{
}

void fun2(int x)
{
}

void fun3(float x, int y, A a)
{
  std::cout << x << " " << y << std::endl;
}

void fun4(B b)
{
  std::cout << "fun4" << std::endl;
}


void fun5(D d)
{
  std::cout << "fun5" << std::endl;
}


void fun6(float x)
{
}


void fun7(A a)
{
  std::cout << "fun7" << std::endl;
}

struct int_factory
{
  int instantiate(float f) { return f + 1; }
};

struct float_factory
{
  float instantiate() { return 2.f; }
};


int main()
{
  float x = 1;
  int y = 2;

  iod::di_call(&fun1);
  iod::di_call(&fun1, 1);
  iod::di_call(&fun2, 1);
  iod::di_call(&fun3, y, x);
  iod::di_call(&fun3, y, x, A());

  iod::di_call(&fun4, C(), B());
  iod::di_call(&fun4, D_factory());  

  iod::di_call(&fun5, D_factory());
  iod::di_call(fun6, 2.f);
  iod::di_call(fun7);


  

  auto f = [] (int x, float y) {};
  int_factory int_f;
  float_factory float_f;
  iod::di_call(f, int_f, float_f);
  
  //void* xx = iod::return_factory_type<D>(0);
}
