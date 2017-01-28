#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <boost/lexical_cast.hpp>

#include "stringview.hh"
#include "symbol_definitions.hh"
#include "grammar.hh"

namespace iod
{
  using namespace s;

  void parse_command_line(int argc, const char* argv[],
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
        else
          args_map[arg_name.to_std_string()].push_back("1");

      }


      if (arg[0] != '-')
      {
        auto value = stringview(arg, len);
        if (arg_name.data())
        {
          assert(args_map[arg_name.to_std_string()].size() > 0);
          args_map[arg_name.to_std_string()].back() = value;
        }
        else
          positionals.push_back(value);

        arg_name = stringview();
        ndash = 0;
        
      }

    }
    
  }


  template <typename S, typename V>
  auto get_option_short_symbol(const assign_exp<S, V>& o)
  {
    return o.left;
  }

  template <typename S, typename V>
  auto get_option_symbol(const assign_exp<S, V>& o)
  {
    return o.left;
  }
  
  template <typename S1, typename S2, typename V>
  auto get_option_symbol(const assign_exp<logical_or_exp<S1, S2>, V>&)
  {
    return S1();
  }

  template <typename S1, typename S2, typename V>
  auto get_option_short_symbol(const assign_exp<logical_or_exp<S1, S2>, V>&)
  {
    return S2();
  }

  template <typename S, typename V>
  decltype(auto) get_option_value(const assign_exp<S, V>& o)
  {
    return o.right;
  }

  template <typename V>
  void parse_option_value(stringview str, std::vector<V>& v)
  {
    v.push_back(boost::lexical_cast<V>(str.str));
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

  namespace cl
  {
    template <typename... T>
    auto positionals(T&&... s)
    {
      return s::_iod_pcl_positionals = std::make_tuple(s...);
    }

    template <typename... T>
    auto required(T&&... s)
    {
      return s::_iod_pcl_required = std::make_tuple(s...);
    }

    template <typename... T>
    auto description(const std::string& gs, T&&... s)
    {
      return s::_iod_pcl_description = D(_description = gs,
                                         _options = D(s...));
    }
  }

  inline const std::string pcl_type_string(const void*) { return "void"; }
  inline const std::string pcl_type_string(const std::string*) { return "string"; }
  inline const std::string pcl_type_string(const int*) { return "int"; }
  inline const std::string pcl_type_string(const float*) { return "float"; }
  inline const std::string pcl_type_string(const double*) { return "double"; }
  inline const std::string pcl_type_string(const bool*) { return ""; }
  template <typename T>
  inline const std::string pcl_type_string(const std::vector<T>*) {
    return std::string("vector<") + pcl_type_string((T*)0) + ">";
  }
  
  template <typename T>
  inline const std::string pcl_type_string(const T*) { return "value"; }
  template <typename T>
  inline const std::string pcl_type_string(const T&) { return pcl_type_string((const T*)0); }
  
  template <typename... A, typename... T>
  auto parse_command_line(std::tuple<A...> attrs, int argc, const char* argv[],
                          T&&... opts)
  {

    std::map<std::string, std::vector<stringview>> args_map;
    std::vector<stringview> positional_values;
    parse_command_line(argc, argv, args_map, positional_values);

    auto options = D((get_option_symbol(opts) = get_option_value(opts))...);
    auto attrs_sio = iod::apply(attrs, D_caller());
    auto positionals = attrs_sio.get(_iod_pcl_positionals, std::make_tuple());

    auto required = attrs_sio.get(_iod_pcl_required, std::make_tuple());
    auto description = attrs_sio.get(_iod_pcl_description, D(_description = "", _options = D()));

    // Generation of the help message
    auto print_help = [&] ()
    {
      std::cout << "Usage: " << argv[0] << " [options...]";
      foreach(positionals) | [] (auto p)
      {
        std::cout << " [" << p.name() << "]";
      };
      std::cout << std::endl;
      std::cout << description.description << std::endl << std::endl;
      std::cout << "Available options:" << std::endl << std::endl;
      foreach(std::make_tuple(opts...)) | [&] (auto o)
      {
        auto opt_symbol = get_option_symbol(o);
        auto symbol = std::string(get_option_symbol(o).name());
        auto short_symbol = std::string(get_option_short_symbol(o).name());

        std::string symbols_str = (symbol.size() > 1 ? "--" : "-") + symbol;
        if (short_symbol != symbol)
          symbols_str += std::string("|") + (short_symbol.size() > 1 ? "--" : "-") + short_symbol;

        std::string req;
        foreach(required) | [&] (auto r) { if (r.equals(opt_symbol)) req += "[REQUIRED] "; };

        std::string desc = description.options.get(opt_symbol, "");
        std::string type_s = pcl_type_string(options[opt_symbol]);
        if (type_s.size()) type_s = " " + type_s;
        std::cout << "  " << std::setw(25) << std::left << (symbols_str + type_s) << req;
        for (unsigned i = 0; i < desc.size(); i++)
        {
          if (desc[i] != '\n')
            std::cout << desc[i];
          else std::cout << std::endl << std::setw(27) << " ";
        }
        std::cout << std::endl;
      };
    };


    // Display help on the --help switch.
    if (args_map.find("help") != args_map.end())
    {
      print_help();
#ifndef IOD_PCL_WITH_EXCEPTIONS
      exit(0);
#endif
      throw std::runtime_error("help");
      
    }

    auto parse_and_check_value = [&] (auto symbol, stringview elt)
      {
        try {
          parse_option_value(elt, options[symbol]);
        } catch (...)
        {
          std::stringstream ss;
          ss << "Invalid command line parameter " << symbol.name() << ": " << std::endl
          << "  Expected type " << pcl_type_string(options[symbol]) << " got " << elt.to_std_string()
          << std::endl;

#ifndef IOD_PCL_WITH_EXCEPTIONS
          std::cerr << ss.str() << std::endl;
          exit(1);
#else
          throw std::runtime_error(ss.str());
#endif
        }
      };

    // Parse options.
    std::map<std::string, bool> filled;
    foreach(std::make_tuple(opts...)) | [&] (auto o)
    {
      auto symbol = get_option_symbol(o);
      auto short_symbol = get_option_short_symbol(o);

      auto it = args_map.find(symbol.name());
      if (it == args_map.end() and
          strcmp(short_symbol.name(), symbol.name()))
        it = args_map.find(short_symbol.name());

      if (it != args_map.end() and it->second.size() > 0)
      {
        for (auto elt : it->second)
          parse_and_check_value(symbol, elt);
        filled[symbol.name()] = true;
      }
      else // Positional ?
      {
        unsigned position = 0;
        foreach(positionals) | [&] (auto p)
        {
          if (p.name() == symbol.name() and
              position < positional_values.size())
          {
            filled[symbol.name()] = true;
            parse_and_check_value(symbol, positional_values[position]);
          }
          position++;
        };
      }
      
    };

    // Check required args.
    std::vector<std::string> missing;
    foreach(required) | [&] (auto s)
    {
      if (filled.find(s.name()) == filled.end())
        missing.push_back(s.name());
    };

    // Error if at least one missing.
    if (missing.size() > 0)
    {
      std::stringstream err;
      if (missing.size() > 1)
      {
        err << "Error missing command line parameters: " << std::endl;
        for (auto m : missing)
          err << "  - " << m << std::endl;
      }
      else
        err << "Error missing required command line parameter " << missing[0] << std::endl;

#ifndef IOD_PCL_WITH_EXCEPTIONS
      std::cerr << err.str() << std::endl;
      exit(1);
#endif
      throw std::runtime_error(err.str());
    }


    return options;
  }


  template <typename... Q, typename... T, typename PS>
  auto parse_command_line(std::tuple<Q...> attrs,
                          int argc, const char* argv[],
                          assign_exp<_iod_pcl_positionals_t, PS> ps,
                          T&&... opts)
  {
    return parse_command_line(std::tuple_cat(attrs, std::make_tuple(ps)), argc, argv, opts...);
  }

  template <typename... Q, typename... T, typename PS>
  auto parse_command_line(std::tuple<Q...> attrs,
                          int argc, const char* argv[],
                          assign_exp<_iod_pcl_required_t, PS> ps,
                          T&&... opts)
  {
    return parse_command_line(std::tuple_cat(attrs, std::make_tuple(ps)), argc, argv, opts...);
  }

  template <typename... Q, typename... T, typename PS>
  auto parse_command_line(std::tuple<Q...> attrs,
                          int argc, const char* argv[],
                          assign_exp<_iod_pcl_description_t, PS> ps,
                          T&&... opts)
  {
    return parse_command_line(std::tuple_cat(attrs, std::make_tuple(ps)), argc, argv, opts...);
  }
  

  template <typename... T>
  auto parse_command_line(int argc, const char* argv[],
                          T&&... opts)
  {
    return parse_command_line(std::make_tuple(), argc, argv, opts...);
  }
}
