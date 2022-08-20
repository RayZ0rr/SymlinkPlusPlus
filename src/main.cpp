#include "checker.hpp"
#include "linker.hpp"
#include "arguments.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace UL = Utilities::Linkers;
namespace UA = Utilities::Arguments;

inline void debugD(const std::string &msg)
{
  std::cout << msg << std::endl;
}

template <class T> inline void debugV(const T &var, const std::string &msg = "")
{
  std::cout << msg;
  std::cout << var << std::endl;
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cout << "Nothing to do. Provide source (to be linked) and destination (to make link)\n";
    std::cout << argv[0] << "[flags] src dest" << std::endl;
    std::cout << "Use 'symlink++ --help' for help." << std::endl;
    return 1;
  }

  // stringV args_vect;

  CLI::App main_app{ "Run commands" };
  UA::arguments args_struct{};

  UA::UseArgs(main_app, args_struct);

  CLI11_PARSE(main_app, argc, argv);

  bool force{ false };
  force = main_app.get_option("--force")->as<bool>();
  bool parents{ main_app.get_option("-p")->as<bool>() };
  bool include_debug{ false };
  CLI::Option *debug_flag = main_app.get_option("-D");
  if (debug_flag->count())
    debug_flag->results(include_debug);
  bool dry_run{ false };
  if (args_struct.m_flags[0]->count())
    args_struct.m_flags[0]->results(dry_run);
  bool absolute{ main_app.get_option("-A")->as<bool>() };

  UL::LinkType link_type{ UL::LinkType::relative };
  if (absolute)
    link_type = UL::LinkType::absolute;
  if (dry_run)
    link_type = UL::LinkType::echo;

  UL::LinkOpts linker_options{ force, parents, link_type };

  UL::LinkStatus_t link_status;
  if (stdfs::is_directory(args_struct.m_src)) {
    link_status = UL::LinkDirectory(args_struct.m_src, args_struct.m_dest,
				    linker_options, include_debug);
  } else {
    const stdfs::path file_src{ args_struct.m_src };
    const stdfs::path file_proper{ file_src.lexically_normal() };
    link_status = UL::LinkFile(file_proper.filename(), args_struct.m_src,
			       args_struct.m_dest, linker_options,
			       include_debug);
  }

  std::cout << "\n\nSymlink++ finished.\n";
  std::cout << "\nTotal linked : " << link_status.first;
  std::cout << "\nNot linked : " << link_status.second;
  std::cout << "\n\n";

  return 0;
}
