#pragma once

#include "CLI11.hpp"
#include <string>
#include <vector>

template <typename T>
void GetArgs(int argCount, char *argVector[], std::vector<T> &fileVector,
	     bool show = false);

using CLIOptionV = std::vector<CLI::Option *>;
using doubleV = std::vector<double>;
using stringV = std::vector<std::string>;

namespace Utilities::Arguments
{

struct arguments
{
  bool m_out{ false };
  int *m_count{ nullptr };
  std::string m_src;
  std::string m_dest;
  CLIOptionV m_options;
  CLIOptionV m_flags;
};

void UseArgs(CLI::App &apps, arguments &args);

void CheckCommand(const std::string &command);
std::string CheckCommandOutput(const std::string &command);

}
