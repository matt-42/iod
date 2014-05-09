#ifndef IOD_JSON_HH_
# define IOD_JSON_HH_

#include <vector>
#include <fstream>
#include <cassert>
#include <tuple>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "iod.hh"

// Json IOD api.

template <typename ...Tail>
inline void iod_from_json(iod_object<Tail...>& o, const std::string& str);

template <typename T, typename ...Tail>
inline std::string iod_to_json(const iod_object<T, Tail...>& o);

namespace iod_internals
{

  // Json IOD Internals.

  template <typename T>
  inline void iod_to_json(const T& t, std::stringstream& ss);

  inline void iod_to_json(const char* t, std::stringstream& ss);

  inline void iod_to_json(const std::string& t, std::stringstream& ss);

  template <typename T>
  inline void iod_to_json(const std::vector<T>& array, std::stringstream& ss);

  template <typename T, typename ...Tail>
  inline void iod_to_json(const iod_object<T, Tail...>& o, std::stringstream& ss);

  template <typename T, typename ...Tail>
  inline std::string iod_to_json(const iod_object<T, Tail...>& o);

  template <typename T>
  inline void iod_attr_to_json(const iod_object<T>& o, std::stringstream& ss);

  template <typename T, typename ...Tail>
  typename std::enable_if<(sizeof...(Tail) > 0), void>::type
  inline iod_attr_to_json(const iod_object<T, Tail...>& o, std::stringstream& ss);


  template <typename T>
  inline void iod_to_json(const T& t, std::stringstream& ss)
  {
    ss << t;
  }

  inline void iod_to_json(const char* t, std::stringstream& ss)
  {
    ss << '"'<< t << '"';
  }

  inline void iod_to_json(const std::string& t, std::stringstream& ss)
  {
    iod_to_json(t.c_str(), ss);
  }

  template <typename T>
  inline void iod_to_json(const std::vector<T>& array, std::stringstream& ss)
  {
    ss << '[';
    for (const auto& t : array)
    {
      iod_to_json(t, ss);
      if (&t != &array.back())
        ss << ',';
    }
    ss << ']';
  }

  template <typename T, typename ...Tail>
  inline void iod_to_json(const iod_object<T, Tail...>& o, std::stringstream& ss)
  {
    ss << '{';
    iod_attr_to_json(o, ss);
    ss << '}';
  }

  template <typename T, typename ...Tail>
  inline std::string iod_to_json(const iod_object<T, Tail...>& o)
  {
    std::stringstream ss;
    iod_to_json(o, ss);
    return ss.str();
  }

  template <typename T>
  inline void iod_attr_to_json(const iod_object<T>& o, std::stringstream& ss)
  {
    const T* attr = &(o);
    ss << '"' << attr->attribute_name() << "\":";
    iod_to_json(attr->value(), ss);
  }

  template <typename T, typename ...Tail>
  inline typename std::enable_if<(sizeof...(Tail) > 0), void>::type
  iod_attr_to_json(const iod_object<T, Tail...>& o, std::stringstream& ss)
  {
    const T* attr = &o;
    ss << '"' << attr->attribute_name() << "\":";
    iod_to_json(attr->value(), ss);
    ss << ',';
    iod_attr_to_json(*static_cast<const iod_object<Tail...>*>(&o), ss);
  }



  template <typename T>
  struct fill_ {
    inline fill_(T& _r) : r(_r) {}
    T& r;
  };

  template <typename T>
  inline fill_<T> fill(T& t) { return fill_<T>(t); }

  struct spaces_ {} spaces;

  struct json_parser
  {

    inline json_parser(const std::string& _str) : ss(_str), str(_str) {}

    inline char peak() { return ss.peek(); }

    inline std::runtime_error json_error()
    {
      std::stringstream err;

      int pos = ss.tellg();
      int w = 20;
      int b = pos > w ? pos - w : 0;
      int e = pos < int(str.size()) - w ? pos + w : int(str.size()) - 1;
      std::cout << b << " " << e <<  " "  << str.size() <<std::endl;
      std::string near(str.begin() + b, str.begin() + e);
      err << "Json parse error near " << near;
      return std::runtime_error(err.str());
    }

    inline json_parser& fill(std::string& t)
    {
      int start = ss.tellg();
      int end;
      char c, prev_c;
      do
      {
        ss.ignore(1000, '"');
        end = ss.tellg();
        c = str[end];
        prev_c = str[end - 1];
      } while (c == '"' and prev_c != '\\');

      ss.unget();
      t.clear();
      t = str.substr(start, end - start - 1);
      return *this;
    }

    template <typename T>
    inline json_parser& fill(T&& t)
    {
      ss >> t;
      if (ss.good()) return *this;
      else
        throw json_error();
    }

    template <typename T>
    inline json_parser& operator>>(fill_<T>&& t)
    {
      return fill(t.r);
    }

    inline json_parser& operator>>(char t)
    {
      char value;
      do
      {
        ss.get(value);
      }
      while(value == ' ');

      if (ss.good() && value == t) return *this;
      else
      {
        throw json_error();
      }
    }

    template <typename T>
    inline json_parser& operator>>(T t)
    {
      T value;
      ss >> value;
      if (ss.good() && value == t) return *this;
      else
      {
        std::cout << t << std::endl;
        std::cout << value << std::endl;
        assert(0);
        throw json_error();
      }
    }

    inline json_parser& operator>>(const char* t)
    {
      std::string value;
      fill(value);
      if (ss.good() && value == std::string(t)) return *this;
      else
        throw json_error();
    }

    inline json_parser& operator>>(spaces_ s)
    {
      char value;
      do
      {
        ss.get(value);
      }
      while(std::isspace(value));

      ss.unget();
      return *this;
    }

    int line_cpt, char_cpt;
    const char* cur;
    std::istringstream ss;
    const std::string& str;
  };

  inline void iod_attr_from_json(iod_object<>& o, json_parser& p)
  {
  }

  template <typename T>
  inline void iod_from_json(T& t, json_parser& p)
  {
    p >> fill(t);
  }

  inline void iod_from_json(std::string& t, json_parser& p)
  {
    p >> '"' >> fill(t) >> '"';
  }

  template <typename T, typename ...Tail>
  inline void iod_attr_from_json(iod_object<T, Tail...>& o, json_parser& p)
  {
    T* attr = &o;

    p >> spaces >> '"' >> attr->attribute_name() >> '"' >> spaces >> ':';

    iod_from_json(attr->value(), p);
    if (sizeof...(Tail) != 0)
      p >> spaces >> ',';

    iod_attr_from_json(*static_cast<iod_object<Tail...>*>(&o), p);
  }

  template <typename T>
  inline void iod_from_json(std::vector<T>& array, json_parser& p)
  {
    p >> '[' >> spaces;
    if (p.peak() == ']')
    {
      p >> ']';
      return;
    }

    array.clear();
    while (p.peak() != ']')
    {
      T t;
      iod_from_json(t, p);
      array.push_back(t);
      p >> spaces;
      if (p.peak() == ']')
        break;
      else
        p >> ',';
    }

    p >> ']';
  }

  template <typename ...Tail>
  inline void iod_from_json(iod_object<Tail...>& o, json_parser& p)
  {
    p >> spaces >> '{';
    iod_attr_from_json(o, p);
    p >> spaces >> '}';
  }

  template <typename ...Tail>
  inline void iod_from_json(iod_object<Tail...>& o, const std::string& str)
  {
    json_parser p(str);
    if (str.size() > 0)
      iod_from_json(o, p);
    else
      throw std::runtime_error("Empty string.");
  }
}


template <typename ...Tail>
inline void iod_from_json(iod_object<Tail...>& o, const std::string& str)
{
  iod_internals::iod_from_json(o, str);
}

template <typename T, typename ...Tail>
inline std::string iod_to_json(const iod_object<T, Tail...>& o)
{
  return iod_internals::iod_to_json(o);
}

#endif
