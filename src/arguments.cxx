#include "arguments.hpp"
#include <iostream>

inline void debugD(const std::string &msg)
{
  std::cerr << msg << std::endl;
}

namespace Utilities::Arguments
{

template <typename T>
void GetArgs(const int argCount, char *argVector[], std::vector<T> &fileVector,
	     bool show)
{
  fileVector.clear();
  // Loop through each argument and store its value to the provided string
  // vector
  for (int count{ 0 }; count < argCount; ++count) {
    if (show)
      std::cout << "Getting arg : " << argVector[count] << std::endl;
    fileVector.push_back(argVector[count]);
  }
}
template void GetArgs<char *>(const int argCount, char *argVector[],
			      std::vector<char *> &fileVector, bool show);
template void GetArgs<std::string>(const int argCount, char *argVector[],
				   std::vector<std::string> &fileVector,
				   bool show);

void UseArgs(CLI::App &apps, arguments &args)
{
  // apps.name(args.m_name);
  // add version output
  apps.set_version_flag(
    "-V,--version", "Symlink++ Version 0.9.3 (Manage symlinks like a pro).\n");
  apps.positionals_at_end();
  // apps.allow_extras(); // Do not throw an error if extra arguments are left over.

  CLI::Option *opt_src =
    apps.add_option("src", args.m_src, "Source to link with.");
  opt_src->required(1);
  opt_src->take_first();
  opt_src->allow_extra_args(false);
  opt_src->option_text(" (required) ");
  // opt_src->each(CheckCommand);
  // opt_src->each([&](std::string arg) { std::cout << "file callback invoked: " << arg << "\n"; }) ;
  // opt_src->check( CLI::Validator("Check if command exists").operation(CheckCommand) ) ;
  args.m_options.push_back(opt_src);

  CLI::Option *opt_dest =
    apps.add_option("dest", args.m_dest, "Destination to create link(s).");
  opt_dest->required(1);
  opt_dest->take_first();
  opt_dest->option_text(" (required) ");
  opt_dest->allow_extra_args(false);
  args.m_options.push_back(opt_dest);

  CLI::Option *dry_flag = apps.add_flag(
    "-n,--dry-run", "Make a test run (directories will be created)");
  dry_flag->default_val(false);
  dry_flag->option_text(" (default : false) ");
  dry_flag->multi_option_policy(CLI::MultiOptionPolicy::TakeFirst);
  args.m_flags.push_back(dry_flag);

  CLI::Option *skip_flag = apps.add_flag(
    "-S,--skip-existing", "skip existing without asking for overwrite");
  skip_flag->needs(opt_src);
  skip_flag->default_val(false);
  skip_flag->option_text(" ");
  args.m_flags.push_back(skip_flag);

  CLI::Option *force_flag = apps.add_flag(
    "-f,--force", "force overwrite without confirmation");
  force_flag->needs(opt_src);
  force_flag->default_val(false);
  force_flag->option_text(" ");
  args.m_flags.push_back(force_flag);

  CLI::Option *parent_flag =
    apps.add_flag("-p,--parents", "Make missing parents directory");
  parent_flag->needs(opt_src);
  parent_flag->take_first();
  parent_flag->default_val(false);
  parent_flag->option_text(" ");
  args.m_flags.push_back(parent_flag);

  CLI::Option *debug_flag =
    apps.add_flag("-D,--debug", "Provide more info (debug) in the output");
  debug_flag->needs(opt_src);
  debug_flag->take_first();
  parent_flag->default_val(false);
  // debug_flag->default_str("1");
  debug_flag->option_text(" ");
  args.m_flags.push_back(debug_flag);

  CLI::Option *absolute_flag =
    apps.add_flag("-A,--absolute", "Use absolute linking (relative linking is used by default)");
  absolute_flag->needs(opt_src);
  absolute_flag->take_first();
  absolute_flag->default_val(false);
  absolute_flag->option_text(" ");
  args.m_flags.push_back(absolute_flag);
}

}
