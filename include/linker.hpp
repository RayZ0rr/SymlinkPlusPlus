#pragma once

#include "checker.hpp"
#include <string>
#include <utility>

namespace Utilities::Linkers
{

enum class LinkType
{
  echo,
  relative,
  absolute,
};

struct LinkOpts
{
  bool skip{ false };
  bool force{ false };
  bool parents{ false };
  LinkType type{ LinkType::relative };
};

using LinkStatus_t = std::pair<int, int>;

bool DryLink(const std::string &src, const std::string &dest,
	     const LinkOpts &lOptions, bool debug = false);
bool DryRelativeLink(const std::string &src, const std::string &dest,
		     const LinkOpts &lOptions, bool debug = false);
LinkStatus_t LinkFile(const std::string &src,
		      const std::string &dest, const LinkOpts &lOptions,
		      bool debug = false);
LinkStatus_t LinkDirectory(const std::string &src, const std::string &dest,
			   const LinkOpts &lOptions, bool debug = false);

}
