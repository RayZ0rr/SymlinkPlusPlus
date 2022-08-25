#include "checker.hpp"
#include <string>
#include <iostream>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace Utilities::Checkers
{

stdfs::path GetCleanPath(const std::string &src)
{
  const stdfs::path tmp_src{ src };
  const stdfs::path lexical_src{ tmp_src.lexically_normal() };
  const stdfs::path abs_src{ stdfs::absolute(lexical_src) };
  return stdfs::weakly_canonical(abs_src);
}

bool CompareFiles(const std::string &src, const std::string &dest)
{
  const stdfs::path file_src{ GetCleanPath(src) };
  const stdfs::path file_dest{ GetCleanPath(dest) };
  return (stdfs::equivalent(file_src, file_dest));
}

bool CompareFileNames(const std::string &src, const std::string &dest)
{
  const stdfs::path file_src{ GetCleanPath(src) };
  const stdfs::path file_dest{ GetCleanPath(dest) };
  return (file_src.filename() == file_dest.filename());
}

bool CheckOverwrite(const std::string &src, const std::string &dest)
{
  const std::string &msg{ "\n\nOverwrite " + dest + " with " + src +
			  "?(Y/n) :-" };
  std::cout << msg;
  std::string answer;
  std::getline(std::cin, answer);
  if (answer == "yes" || answer == "Yes" || answer == "Y" || answer == "y" ||
      answer == "YES")
    return true;

  return false;
}

bool CheckFile(const std::string &target)
{
  const auto file_target{ GetCleanPath(target) };
  if (stdfs::is_directory(file_target) || not stdfs::exists(file_target)) {
    std::cerr << "\nError (CheckFile): requires source as exisiting file.\n";
    std::cerr << file_target << " is not a file.\n";
    return false;
  }
  return true;
}

bool CheckDirectory(const std::string &target)
{
  const auto file_target{ GetCleanPath(target) };
  if (not stdfs::is_directory(file_target)) {
    std::cerr
      << "\nError (CheckDirectory): requires source as exisiting directory.\n";
    std::cerr << file_target << " is not a directory.\n";
    return false;
  }
  return true;
}

bool ContentsCheckFile(const stdfs::path &src, const stdfs::path &dest,
		       bool debug)
{
  const stdfs::path file_src{ GetCleanPath(src) };
  const stdfs::path file_dest{ GetCleanPath(dest) };
  if (not CheckFile(file_src)) {
    std::cerr << "Error (ContentsCheckFile): Failed.\n\n";
    return false;
  }
  if (CompareFileNames(file_dest, file_src)) {
    if (debug) {
      std::cout << "\nIt exists. Same name\n";
      std::cout << file_src << " & " << file_dest << '\n';
    }
    return true;
  } else if (file_dest.has_parent_path()) {
    const stdfs::path file_parent{ file_dest.parent_path() };
    if (debug) {
      std::cout << "\nCan overwite or create new.\n";
      std::cout << file_dest << " or " << file_parent/file_src << '\n';
    }
    return false;
  }
  return false;
}

bool ContentsCheckDirectory(const stdfs::path &src, const stdfs::path &dest,
			    bool debug)
{
  const stdfs::path file_src{ src.lexically_normal() };
  const stdfs::path file_dest{ GetCleanPath(dest) };
  if (not CheckDirectory(file_dest)) {
    std::cerr << "Error (ContentsCheckDirectory): Failed.\n\n";
    return false;
  }
  auto src_begin{ file_src.begin() };
  if (file_src.has_root_directory()) {
    ++src_begin;
    if (file_src.has_root_name())
      ++src_begin;
  }
  bool result{ false };
  stdfs::path file_match{};
  for (stdfs::path::iterator it = src_begin; it != file_src.end(); ++it) {
    const stdfs::path file_current{ file_dest/file_match };
    bool found{ false };
    if (debug)
      std::cout << "\nChecking : in " << file_current << '\n';
    for (const stdfs::directory_entry &entry :
	 stdfs::directory_iterator(file_current)) {
      const stdfs::path target{ entry.path().lexically_relative(file_current) };
      if (debug) {
	std::cout << "\nChecking : " << *it;
	std::cout << "\tagainst : " << target << '\n';
      }
      if (*it == target) {
	if (debug) {
	  std::cout << "It exists. Same name\n";
	  std::cout << *it << " & " << target << '\n';
	  std::cout << "Continuing to next level.\n";
	}
	file_match /= *it;
	found = true;
	break;
      }
      found = false;
    }
    result = found;
  }
  if (debug)
    std::cout << "Search ended.\n";
  if (debug) {
    if (result)
      std::cout << "Found " << src << " inside " << dest << '\n';
    else
      std::cout << "Couldn't find " << src << " inside " << dest << '\n';
  }
  return result;
}

PathVerify_t CheckFileRecursive(const std::string &filename,
				const std::string &filepath, bool debug)
{
  const stdfs::path file_src{ filename };
  const stdfs::path file_dest{ GetCleanPath(filepath) };
  if (stdfs::is_directory(file_dest)) {
    if (ContentsCheckDirectory(file_src, file_dest, debug)) {
      if (debug) {
	std::cout << "\nFound inside directory.\n";
	std::cout << file_src << " is " << file_dest / file_src << "\n\n";
      }
      return PathVerify_t(true, file_dest / file_src);
    } else {
      if (debug) {
	std::cout << "\nCan make inside directory.\n";
	std::cout << file_src << " as " << file_dest / file_src << "\n\n";
      }
      return PathVerify_t(false, file_dest / file_src);
    }
  } else if (stdfs::exists(file_dest)) {
    if (debug)
      std::cout << "\nNot a directory.\n";
    if (ContentsCheckFile(file_src, file_dest, debug)) {
      return PathVerify_t(true, file_dest);
    }
  } else if (file_dest.has_parent_path() &&
	     stdfs::is_directory(file_dest.parent_path())) {
    const stdfs::path file_parent{ file_dest.parent_path() };
    if (debug) {
      std::cout << "\nCan make file.\n";
      std::cout << file_parent / file_src << " instead of " << file_dest
		<< '\n';
    }
    return PathVerify_t(false, file_parent / file_src);
  }
  if (debug) {
    std::cout << "\nIt doesn't exists. Checked :\n";
    std::cout << file_src << " & " << file_dest << '\n';
  }
  return PathVerify_t(false, "");
}

}
