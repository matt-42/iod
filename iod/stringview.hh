#pragma once

#include <cstring>
#include <string>

namespace iod
{
  
  struct stringview
  {
    stringview() {}
    stringview(const std::string& _str) : str(&_str[0]), len(_str.size()) {}
    stringview(const char* _str, int _len) : str(_str), len(_len) {}
    stringview(const char* _str) : str(_str), len(strlen(_str)) {}
      
    bool operator==(const stringview& o) const { return len == o.len and !strncmp(o.str, str, len); }
    bool operator==(const std::string& o) const { return len == int(o.size()) and !strncmp(&o[0], str, len); }
    bool operator==(const char* o) const { return len == int(strlen(o)) and !strncmp(o, str, len); }

    bool operator<(const stringview& o) const { return strncmp(o.str, str, std::min(len, o.len)); }
    explicit operator std::string() const { return std::string(str, len); }

    auto& operator[](int p) { return str[p]; }
    const auto& operator[](int p) const { return str[p]; }

    int size() const { return len; }
    const char* data() const { return str; }
    auto to_std_string() const { return std::string(str, len); }

    const char* str;
    int len;
  };

}
