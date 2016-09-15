#include <map>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "stringview.hh"
#include "grammar.hh"


namespace iod
{
  
  void parse_command_line(int argc, const char** argv,
                          std::map<std::string, std::vector<stringview>>& args_map,
                          std::vector<stringview>& positionals)
  {

    stringview arg_name;
    for (int ai = 1; ai < argc; ai++)
    {
      const char* arg = argv[ai];
      int len = strlen(arg);

      int ndash = 0;
      
      if (arg[0] == '-')
        ndash = arg[1] == '-' ? 2 : 1;

      if (ndash > 0)
      {
        arg_name = stringview(arg + ndash, arg + len);

        // if we have an equal assignement -o=12.
        for (int i = 0; i < arg_name.size(); i++)
        {
          if (arg_name[i] == '=')
          {
            arg_name = stringview(arg_name.str, i);
            arg = arg_name.str + i + 1;
            len = strlen(arg);
            break;
          }
        }

        // Register swithes.
        if (ndash == 1)
        {
          for (int i = 0; i < arg_name.size(); i++)
            args_map[std::string(1, arg_name[i])].push_back("1");
          if (arg_name.size() > 1)
          {
            arg_name = stringview();
            ndash = 0;
          }
        }
      }


      if (arg[0] != '-')
      {
        auto value = stringview(arg, len);
        if (arg_name.data())
        {
          if (arg_name.size() > 1)
            args_map[arg_name.to_std_string()].push_back(value);
          else
          {
            assert(args_map[arg_name.to_std_string()].size() > 0);
            args_map[arg_name.to_std_string()].back() = value;
          }
        }
        else
          positionals.push_back(value);

        arg_name = stringview();
        ndash = 0;
        
      }

    }
    
  }

  template <typename S, typename V>
  decltype(auto) get_option_symbol(const assign_exp<S, V>& o)
  {
    return o.left;
  }

  template <typename S, typename V>
  decltype(auto) get_option_value(const assign_exp<S, V>& o)
  {
    return o.right;
  }

  template <typename V>
  void parse_option_value(stringview str, V& v)
  {
    v = boost::lexical_cast<V>(str.str);
  }

  template <typename V>
  void parse_option_value(stringview str, V* v)
  {
    *v = boost::lexical_cast<V>(str.str);
  }

  template <typename... T>
  auto positionals(T&&... s)
  {
    return std::make_tuple(s...);
  }

  
  template <typename... T, typename... P>
  auto parse_command_line(int argc, const char** argv,
                          std::tuple<P...> positionals,
                          T&&... opts)
  {

    std::map<std::string, std::vector<stringview>> args_map;
    std::vector<stringview> positional_values;
    parse_command_line(argc, argv, args_map, positional_values);

    auto options = D((get_option_symbol(opts) = get_option_value(opts))...);

    foreach(std::make_tuple(opts...)) | [&] (auto o)
    {
      auto symbol = get_option_symbol(o);
      auto short_symbol = get_option_symbol(o);

      auto it = args_map.find(symbol.name());
      if (it == args_map.end())
        it = args_map.find(short_symbol.name());

      if (it != args_map.end() and it->second.size() > 0)
      {
        parse_option_value(it->second[0], options[symbol]);
      }
      else // Positional ?
      {
        unsigned position = 0;
        foreach(positionals) | [&] (auto p)
        {
          if (p.name() == symbol.name() and
              position < positional_values.size())
          {
            parse_option_value(positional_values[position],
                               options[symbol]);
          }
          position++;
        };
      }
      
    };

    return options;
  }

  template <typename... T>
  auto parse_command_line(int argc, const char** argv, T&&... opts)
  {
    return parse_command_line(argc, argv, positionals(), opts...);
  }

}
