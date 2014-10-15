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
#include <map>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <iod/iod.hh>
#include <iod/symbols.hh>

// Json IOD api.

namespace iod
{

  using s::json_symbol;
  using s::optional;

  // Decode \o from a json string \str.
  template <typename ...T>
  inline void json_decode(iod_object<T...>& o, const std::string& str);

  // Encode \o into a json string.
  template <typename ...T>
  inline std::string json_encode(const iod_object<T...>& o);

  // Encode \o into a stream.
  template <typename S, typename ...Tail>
  inline void json_encode(const iod_object<Tail...>& o, S& stream);


  namespace json_internals
  {

    // Json encoder.
    // =============================================
    template <typename T, typename S>
    inline void json_encode_(const T& t, S& ss)
    {
      ss << t;
    }

    template <typename S>
    inline void json_encode_(const char* t, S& ss)
    {
      ss << '"' << t << '"';
    }

    template <typename S>
    inline void json_encode_(const std::string& t, S& ss)
    {
      json_encode_(t.c_str(), ss);
    }

    // Forward declaration.
    template <typename S, typename ...Tail>
    inline void json_encode_(const iod_object<Tail...>& o, S& ss);

    template <typename T, typename S>
    inline void json_encode_(const std::vector<T>& array, S& ss)
    {
      ss << '[';
      for (const auto& t : array)
      {
        json_encode_(t, ss);
        if (&t != &array.back())
          ss << ',';
      }
      ss << ']';
    }

    template <typename S, typename ...Tail>
    inline void json_encode_(const iod_object<Tail...>& o, S& ss)
    {
      ss << '{';
      int i = 0;
      foreach_attribute([&] (const auto& m)
                        {
                          json_encode_(m.symbol_name(), ss);
                          ss << ':';
                          json_encode_(m.value(), ss);
                          if (i != o.size() - 1) ss << ',';
                          i++;
                        }, o);
      ss << '}';
    }

    // Json decoder.
    // =============================================

    template <typename T>
    struct fill_ {
      inline fill_(T& _r) : r(_r) {}
      T& r;
    };

    template <typename T>
    inline fill_<T> fill(T& t) { return fill_<T>(t); }


    struct json_parser
    {
      struct spaces_ {} spaces;

      inline json_parser(const std::string& _str) : ss(_str), str(_str) {}

      inline char peak() { return ss.peek(); }
      inline char eof() { return ss.eof(); }

      template <typename E>
      inline void format_error(E& err) {}

      template <typename E, typename T1, typename... T>
      inline void format_error(E& err, T1 a, T... args)
      {
        err << a;
        format_error(err, args...);
      }

      template <typename... T>
      inline std::runtime_error json_error(T... message)
        {
          std::stringstream err;

          int pos = ss.tellg();
          int w = 20;
          int b = pos > w ? pos - w : 0;
          int e = pos < int(str.size()) - w ? pos + w : int(str.size()) - 1;
          std::string near(str.begin() + b, str.begin() + e);
          err << std::endl << "Json parse error near " << near << std::endl;
          err << "                      ";
          for (int i = 0; i < pos - b - 1; i++) err << ' ';
          err << "^^^"<< std::endl;
          format_error(err, message...);
          // assert(0);
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
            throw json_error("Expected ", t, " got ", value);
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
            throw json_error();
          }
        }

      inline json_parser& operator>>(const char* t)
        {
          std::string value;
          fill(value);
          if (ss.good() && value == std::string(t)) return *this;
          else
            throw json_error("Expected ", t, " got ", value);
        }

      inline json_parser& operator>>(spaces_)
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

    inline void iod_attr_from_json(iod_object<>&, json_parser&)
    {
    }

    template <typename T>
    inline void iod_from_json_(T& t, json_parser& p)
    {
      p >> fill(t);
    }

    inline void iod_from_json_(std::string& t, json_parser& p)
    {
      p >> '"' >> fill(t) >> '"';
    }

    // Parse a json hashmap ordered the field in the object \o.
    template <typename T, typename ...Tail>
    inline void iod_attr_from_json_strict(iod_object<T, Tail...>& o, json_parser& p)
    {
      T* attr = &o;

      p >> p.spaces >> '"' >> attr->attribute_name() >> '"' >> p.spaces >> ':';

      iod_from_json_(attr->value(), p);
      if (sizeof...(Tail) != 0)
        p >> p.spaces >> ',';

      iod_attr_from_json(*static_cast<iod_object<Tail...>*>(&o), p);
    }

    // Parse a json hashmap.
    template <typename T, typename ...Tail>
    inline void iod_attr_from_json(iod_object<T, Tail...>& o, json_parser& p)
    {
      p >> p.spaces;

      std::map<std::string, bool> filled;
      std::map<std::string, int> symbol_map;
      int i = 0;
      foreach_attribute([&] (auto& m) {
          std::string name = m.symbol().name();
          if (m.attributes().has(json_symbol))
            name = m.attributes().get(json_symbol, json_symbol).name();
          symbol_map[name] = i++;
          filled[name] = false;
        }, o);

      while (p.peak() != '}')
      {
        std::string attr_name;
        p >> p.spaces >> '"' >> fill(attr_name) >> '"' >> p.spaces >> ':' >> p.spaces;
        filled[attr_name] = true;
        auto it = symbol_map.find(attr_name);
        if (it != symbol_map.end())
        {
          switch (it->second)
          {
#define IOD_ATTR_CASE(z, I, X) case I: iod_from_json_(o.template get_nth<I>(), p); break;
            BOOST_PP_REPEAT(50, IOD_ATTR_CASE, x);
            #undef IOD_ATTR_CASE
          default:
            throw std::runtime_error(std::string("json_decode error: field does not exists: ") +
                                     it->first);
            break;
          }
        }
        p >> p.spaces;
        if (p.peak() == ',')
          p >> ',';
        else
          break;
      }

      if (p.peak() != '}')
      {
        throw p.json_error("Expected } got ", p.peak());
      }

      foreach_attribute([&] (auto& m) {
          typename std::remove_reference_t<decltype(m)>::attributes_type attrs;
          if (!m.attributes().has(optional) and !filled[m.symbol().name()])
            throw std::runtime_error(std::string("json_decode error: missing field ") +
                                     m.symbol().name());
        }, o);
    }

    // Parse an array.
    template <typename T>
    inline void iod_from_json_(std::vector<T>& array, json_parser& p)
    {
      p >> '[' >> p.spaces;
      if (p.peak() == ']')
      {
        p >> ']';
        return;
      }

      array.clear();
      while (p.peak() != ']')
      {
        T t;
        iod_from_json_(t, p);
        array.push_back(t);
        p >> p.spaces;
        if (p.peak() == ']')
          break;
        else
          p >> ',';
      }

      p >> ']';
    }

    template <typename ...Tail>
    inline void iod_from_json_(iod_object<Tail...>& o, json_parser& p)
    {
      p >> p.spaces >> '{';
      iod_attr_from_json(o, p);
      p >> p.spaces >> '}';
    }

    template <typename ...Tail>
    inline void iod_from_json_(iod_object<Tail...>& o, const std::string& str)
    {
      json_parser p(str);
      if (str.size() > 0)
        iod_from_json_(o, p);
      else
        throw std::runtime_error("Empty string.");
    }
  }

  template <typename ...Tail>
  inline void json_decode(iod_object<Tail...>& o, const std::string& str)
  {
    json_internals::iod_from_json_(o, str);
  }

  template <typename ...Tail>
  inline std::string json_encode(const iod_object<Tail...>& o)
  {
    std::stringstream ss;
    json_internals::json_encode_(o, ss);
    return ss.str();
  }

  template <typename S, typename ...Tail>
  inline void json_encode(const iod_object<Tail...>& o, S& stream)
  {
    json_internals::json_encode_(o, stream);
  }

}

#endif
