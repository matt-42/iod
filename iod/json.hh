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
#include <boost/lexical_cast.hpp>

#include <iod/sio.hh>
#include <iod/foreach.hh>
#include <iod/symbols.hh>
#include <iod/pow_10.hh>
#include <iod/stringview.hh>

// Json IOD api.

namespace iod
{

  using namespace s;

  // Decode \o from a json string \str.
  template <typename ...T>
  inline void json_decode(sio<T...>& o, const stringview& str);

  // Encode \o into a json string.
  template <typename ...T>
  inline std::string json_encode(const sio<T...>& o);

  // Encode \o into a json string.
  template <typename T>
  inline std::string json_encode(const std::vector<T>& v);
  
  // Encode \o into a stream.
  template <typename S, typename ...Tail>
  inline void json_encode(const sio<Tail...>& o, S& stream);

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
    inline void json_encode_(const stringview& s, S& ss)
    {
      ss << '"';
      for (int i = 0; i < s.len; i++)
        ss << s.str[i];
      ss << '"';
    }
    
    template <typename S>
    inline void json_encode_(const std::string& t, S& ss)
    {
      json_encode_(t.c_str(), ss);
    }

    // Forward declaration.
    template <typename S, typename ...Tail>
    inline void json_encode_(const sio<Tail...>& o, S& ss);

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
    inline void json_encode_(const sio<Tail...>& o, S& ss)
    {
      ss << '{';
      int i = 0;
      bool first = true;
      foreach(o) | [&] (const auto& m)
      {
        if (!m.attributes().has(_json_skip))
        {
          if (!first) { ss << ','; }
          first = false; 
          json_encode_(m.attributes().get(_json_key, m.symbol()).name(), ss);
          ss << ':';
          json_encode_(m.value(), ss);
        }
        i++;
      };
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

      inline json_parser(std::istringstream& _stream) : str(_stream.str()), pos(0) {}
      inline json_parser(const std::string& _str) : str(_str.c_str(), _str.size()), pos(0) {}
      inline json_parser(const stringview& _str) : str(_str), pos(0) {}

      inline char peak() { return str[pos]; }
      inline char eof() { return pos == str.size(); }
      inline char eat_one() { return pos++; }

      template <typename E>
      inline void format_error(E&) {}

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

        int w = 20;
        int b = pos > w ? pos - w : 0;
        int e = pos < int(str.size()) - w ? pos + w : int(str.size()) - 1;
        std::string near(str.data() + b, str.data() + e);
        err << std::endl << "Json parse error near " << near << std::endl;
        err << "                      ";
        for (int i = 0; i < pos - b - 1; i++) err << ' ';
        err << "^^^"<< std::endl;
        format_error(err, message...);
        return std::runtime_error(err.str());
      }
      
      inline json_parser& fill(std::string& t)
      {
        int start = pos;
        int end = pos;
        t.clear();

        char buffer[128];
        int buffer_pos = 0;
        auto flush = [&] () { t.append(buffer, buffer_pos); buffer_pos = 0; };
        auto append_char = [&] (char c)
        {
          if (buffer_pos == sizeof(buffer)) flush();
            
          buffer[buffer_pos] = c;
          buffer_pos++;
        };
        auto append_str = [&] (const char* str, int len)
        {
          if (buffer_pos + len > int(sizeof(buffer))) flush();
          memcpy(buffer + buffer_pos, str, len);
          buffer_pos += len;
        };
        
        while (true)
        {
          while (!eof() and str[end] != '"' and str[end] != '\\')
            end++;

          if (eof()) throw json_error("Unexpected end of string when parsing a string.");
          append_str(str.data() + start, end - start);

          if (str[end] == '"') break;

          end++;
          switch (str[end])
          {
          case '\'': append_char('\''); break;
          case '"': append_char('"'); break;
          case '\\': append_char('\\'); break;
          case '/': append_char('/'); break;
          case 'n': append_char('\n'); break;
          case 'r': append_char('\r'); break;
          case 't': append_char('\t'); break;
          case 'b': append_char('\b'); break;
          case 'f': append_char('\f'); break;
          case 'v': append_char('\v'); break;
          case '0': append_char('\0'); break;
          case 'u':
            while (true)
            {
              if (str.size() < end + 4)
                throw json_error("Unexpected end of string when decoding an utf8 character");
              end++;

              auto decode_hex_c = [this] (char c) {
                if (c >= '0' and c <= '9') return c - '0';
                else return (10 + c - 'A');
              };
              
              const char* str2 = str.data() + end;
              char x = (decode_hex_c(str2[0]) << 4) + decode_hex_c(str2[1]);
              if (x) append_char(x);
              append_char((decode_hex_c(str2[2]) << 4) + decode_hex_c(str2[3]));

              end += 4;
              
              if (str[end] == '\\' and str[end + 1] == 'u')
                end += 1;
              else break;
            }
            break;
          }

          start = end;
        }
        flush();
        pos = end;
        return *this;
      }
      
      inline json_parser& fill(stringview& t)
      {
        int start = pos;
        int end = pos;

        while (!eof() and str[end] != '"' and str[end - 1] != '\\') end++;
        t.str = str.data() + start;
        t.len = end - start;
        pos = end;
        return *this;
      }

      template <typename I, int N>
      inline json_parser& fill_int(I& val)
      {
        int sign = 1;
        if (std::is_signed<I>::value and str[pos] == '-') { sign = -1; eat_one(); }
        else if (str[pos] == '+') { eat_one(); }

        int end = pos;
          
        val = 0;

        const char* s = str.data() + pos;

        
        for (int i = 0; i < N; i++)
        {
          if (s[i] < '0' or s[i] > '9') break;
          val = val * 10 + (s[i] - '0');
          end++;
        }
        val *= sign;

        if (end == pos) throw json_error("Could not find the expected number.");
        
        pos = end;
        return *this;
      }
      
      inline json_parser& fill(float& val)
      {
        float res = 0;
          
        int ent = 0;
        fill_int<int, 10>(ent);

        res = ent;

        if (str[pos] == '.')
        {
          eat_one();
          unsigned int floating = 0;
          int start = pos;
          fill_int<unsigned int, 10>(floating);
          int end = pos;
          res += float(floating) / pow_10(end - start);
        }

        if (str[pos] == 'e')
        {
          eat_one();
          int exp = 0;
          fill_int<int, 10>(exp);
          res *= pow_10(exp);
        }

        val = res;
        return *this;
      }
      
      inline json_parser& fill(int& val) { return fill_int<int, 10>(val); }
      inline json_parser& fill(unsigned int& val) { return fill_int<unsigned int, 10>(val); }
      
      template <typename T>
      inline json_parser& fill(T& t)
      {
        int end = pos;
        while(!eof() and str[end] != ',' and str[end] != '}' and str[end] != ']') end++;
        t = boost::lexical_cast<std::remove_reference_t<T>>(str.data() + pos, end - pos);
        pos = end;
        return *this;
      }

      template <typename T>
      inline json_parser& operator>>(fill_<T>&& t)
      {
        return fill(t.r);
      }

      inline json_parser& operator>>(char t)
      {
        if (!eof() and str[pos] == t)
        {
          pos++;
          return *this;
        }
        else
        {
          if (eof())
            throw json_error("Expected ", t, " got eof");
          else
            throw json_error("Expected ", t, " got ", str[pos]);
        }
      }

      inline json_parser& operator>>(const char* t)
      {
        int start = pos;
        int end = pos;
        while (!eof() and (t[end - start] == str[end] or str[end] != '"')) end++;

        if (t[end - start] == '\0')
        {
          pos = end;
          return *this;
        }
        else
          throw json_error("Expected ", t, " got something else.");
      }


      inline json_parser& operator>>(spaces_)
      {
        while (!eof() and std::isspace(str[pos])) pos++;
        return *this;
      }

      int line_cpt, char_cpt;
      const char* cur;
      stringview str;
      int pos;
    };

    inline void iod_attr_from_json(sio<>&, json_parser&)
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

    inline void iod_from_json_(stringview& t, json_parser& p)
    {
      p >> '"' >> fill(t) >> '"';
    }
    
    // Parse a json hashmap ordered the field in the object \o.
    template <typename T, typename ...Tail>
    inline void iod_attr_from_json_strict(sio<T, Tail...>& o, json_parser& p)
    {
      T* attr = &o;

      p >> p.spaces >> '"' >> attr->symbol().name() >> '"' >> p.spaces >> ':';

      iod_from_json_(attr->value(), p);
      if (sizeof...(Tail) != 0)
        p >> p.spaces >> ',';

      iod_attr_from_json(*static_cast<sio<Tail...>*>(&o), p);
    }

    // Parse a json hashmap.
    template <typename T, typename ...Tail>
    inline void iod_attr_from_json(sio<T, Tail...>& o, json_parser& p)
    {
      p >> p.spaces;

      struct attr_info { bool filled; stringview name; };

      attr_info A[std::remove_reference_t<decltype(o)>::size()];
      
      int i = 0;
      foreach(o) | [&] (auto& m)
      {
        A[i].filled = false;
        stringview name(m.symbol().name(), strlen(m.symbol().name()));
        if (m.attributes().has(_json_key))
        {
          const char* new_name = m.attributes().get(_json_key, _json_key).name();
          name = stringview(new_name, strlen(new_name));
        }
        A[i].name = name;
        i++;
      };

      while (p.peak() != '}')
      {
        stringview attr_name;
        p >> p.spaces >> '"' >> fill(attr_name) >> '"' >> p.spaces >> ':' >> p.spaces;

        int i = 0;
        bool attr_found = false;
        foreach(o) | [&] (auto& m)
        {
          if (!m.attributes().has(_json_skip) and !attr_found and attr_name == A[i].name)
          {
            iod_from_json_(m.value(), p);
            A[i].filled = true;
            attr_found = true;
          }
          i++;
        };
        // Fixme: if !attr_found, skip the json value.
        p >> p.spaces;
        if (p.peak() == ',')
          p.eat_one();
        else
          break;
      }

      if (p.peak() != '}')
      {
        throw p.json_error("Expected } got ", p.peak());
      }

      i = 0;
      foreach(o) | [&] (auto& m) {
        if (!m.attributes().has(_json_skip) and !m.attributes().has(_optional) and !A[i].filled)
          throw std::runtime_error(std::string("json_decode error: missing field ") +
                                   m.symbol().name());
        i++;
      };
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
    inline void iod_from_json_(sio<Tail...>& o, json_parser& p)
    {
      p >> p.spaces >> '{';
      iod_attr_from_json(o, p);
      p >> p.spaces >> '}';
    }

    template <typename ...Tail>
    inline void iod_from_json_(sio<Tail...>& o, const std::string& str)
    {
      json_parser p(str);
      if (str.size() > 0)
        iod_from_json_(o, p);
      else
        throw std::runtime_error("Empty string.");
    }
  }

  template <typename ...Tail>
  inline void json_decode(sio<Tail...>& o, const stringview& str, int& n_read)
  {
    if (o.size() == 0) return;
    json_internals::json_parser p(str);
    if (str.size() > 0)
      iod_from_json_(o, p);
    else
      throw std::runtime_error("Empty string.");
    n_read = p.pos;
  }

  template <typename ...Tail>
  inline void json_decode(sio<Tail...>& o, const stringview& str)
  {
    if (o.size() == 0) return;
    json_internals::json_parser p(str);
    if (str.size() > 0)
      iod_from_json_(o, p);
    else
      throw std::runtime_error("Empty string.");
  }
  
  template <typename ...Tail>
  inline void json_decode(sio<Tail...>& o, std::istringstream& stream)
  {
    if (o.size() == 0) return;
    json_internals::json_parser p(stream);
    if (stream.str().size() > 0)
      iod_from_json_(o, p);
    else
      throw std::runtime_error("Empty string.");
  }
  
  template <typename ...Tail>
  inline std::string json_encode(const sio<Tail...>& o)
  {
    std::stringstream ss;
    json_internals::json_encode_(o, ss);
    return ss.str();
  }

  template <typename S, typename ...Tail>
  inline void json_encode(const sio<Tail...>& o, S& stream)
  {
    json_internals::json_encode_(o, stream);
  }

  template <typename T>
  inline std::string json_encode(const std::vector<T>& v)
  {
    std::stringstream ss;
    json_internals::json_encode_(v, ss);
    return ss.str();    
  }
  
}

#endif
