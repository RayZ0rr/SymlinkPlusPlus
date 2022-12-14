#pragma once

#include <string>
#include <utility>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace Utilities::Checkers
{

using PathVerify_t = std::pair<bool, std::string>;

stdfs::path GetCleanPath(const std::string &src) ;

bool CompareFileNames(const std::string &src, const std::string &dest);

bool CompareFiles(const std::string &src, const std::string &dest);

bool CheckOverwrite(const std::string &src, const std::string &dest);

bool CheckFile(const std::string &target) ;

bool CheckDirectory(const std::string &target) ;

PathVerify_t CheckFileRecursive(const std::string &filename,
				const std::string &filepath,
				bool debug = false);

}
