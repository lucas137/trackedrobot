//===========================================================================//
//  Nathan Lucas
//  2018
//===========================================================================//

#ifndef __cplusplus
#error must be compiled as C++
#endif

#include "core/overwatch.hpp"   // moc::Overwatch, moc::version
#include "config/config.hpp"    // moc::Configuration, moc::parse_config

#include <eyelib.hpp>       // eye::screen_list

#include <utl/app.hpp>      // utl::app::arguments
#include <utl/json.hpp>     // nlohmann::json

#include <exception>    // std::exception
#include <iostream>     // std::cout
#include <string>       // std::string, std::to_string

namespace {   //-------------------------------------------------------------

bool
help(std::string const& arg)
{
  return (arg == "-h") || (arg == "-?") || (arg == "---help");
}

std::string
usage(std::string const& name)
{
  return  name + " " + std::to_string(moc::version) +
          "\n"
          "\n"  "Multirobot overwatch control."
          "\n"
          "\n"  "Usage:"
          "\n"
          "\n"  "  " + name + "  [-h] [<filename>]"
          "\n"
          "\n"  "    -h          show help"
          "\n"  "    filename    configuration file name"
          "\n"
          "\n"  "Press 'F1' for help."
          "\n";
}

} // anonymous --------------------------------------------------------------

int
main(int argc, char* argv[])
{
  auto args = utl::app::parse_args(argc, argv, true);

  moc::Configuration config;

  // Check for arguments
  if ((args.size() == 2) && !args[1].empty())
  {
    if (help(args[1]))
    {
      std::cout << usage(args[0]);
      return EXIT_SUCCESS;
    }
    config.config_file = args[1];
  }

  // Configure and run
  try
  {
    moc::parse_config(config);
    std::cout //<< config <<'\n'<<'\n'
              << eye::screen_list(config.window.screen) <<'\n';
    moc::Overwatch ovr(config);
    ovr.run();
  }
  catch (std::exception& e)
  {
    std::cout <<'\n'<< __FILE__<<':'<<__LINE__    // source and line number
              <<'\n'<< "ERROR: exception:"
              <<'\n'<< e.what()
              <<'\n';
    return EXIT_FAILURE;
  }
  catch (...)   // catch all other exceptions
  {
    std::cout <<'\n'<< __FILE__<<':'<<__LINE__    // source and line number
              <<'\n'<< "ERROR: unknown exception"
              <<'\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//===========================================================================//
