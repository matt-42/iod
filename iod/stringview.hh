#pragma once

#include <cstring>
#include <string>
#include <cassert>

namespace iod
{
  
  struct stringview
  {
    stringview() : str(0), len(0) {}
    stringview(const std::string& _str) : str(&_str[0]), len(_str.size()) {}
    stringview(const char* _str, std::size_t _len) : str(_str), len(_len) {}
    stringview(const char* _begin, const char* _end) : str(_begin), len(_end - _begin) { assert(_end >= _begin); }
    stringview(const char* _str) : str(_str), len(strlen(_str)) {}
      
    bool operator==(const stringview& o) const { return len == o.len and !strncmp(o.str, str, len); }
    bool operator==(const std::string& o) const { return len == o.size() and !strncmp(&o[0], str, len); }
    bool operator==(const char* o) const { return len == strlen(o) and !strncmp(o, str, len); }

    bool operator<(const stringview& o) const { return strncmp(o.str, str, std::min(len, o.len)); }
    explicit operator std::string() const { return std::string(str, len); }

    auto& operator[](int p) { return str[p]; }
    const auto& operator[](int p) const { return str[p]; }

    int size() const { return len; }
    const char* data() const { return str; }
    auto to_std_string() const { return std::string(str, len); }

    auto substr(int start, int new_len) { return stringview(str + start, new_len); }

    const char* str;
    std::size_t len;
  };

}
