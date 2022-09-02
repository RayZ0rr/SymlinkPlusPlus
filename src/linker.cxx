#include "linker.hpp"
#include "checker.hpp"
#include <string>
#include <iostream>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace UC = Utilities::Checkers;

namespace Utilities::Linkers
{

stdfs::path ExistingPath(const std::string &dest)
{
  const stdfs::path file_dest{ UC::GetCleanPath(dest) };
  stdfs::path result{};
  stdfs::path file_match{};
  for (stdfs::path::iterator it = file_dest.begin(); it != file_dest.end();
       ++it) {
    file_match /= *it;
    // std::cerr << "\nExistingPath : file_match is : " << file_match << '\n' ;
    if (not stdfs::exists(file_match))
      break;
    else
      result = file_match;
  }
  return result;
}

bool PrepareForDirectory(const stdfs::path &file_src,
			 const stdfs::path &file_dest, bool parents)
{
  const stdfs::path tmp_dest{ UC::GetCleanPath(file_dest) };
  const stdfs::path file_parent{ tmp_dest.parent_path() };
  const stdfs::path file_existing{ ExistingPath(file_dest) };
  if (stdfs::exists(file_dest) && stdfs::is_directory(file_dest)) {
    return true;
  } else if (stdfs::exists(file_dest) && not stdfs::is_directory(file_dest)) {
    std::cerr << "\nError (PrepareTarget): invalid target specified.\n";
    std::cerr << file_src << " is an existing directory.\nBut ";
    std::cerr << file_dest << " is not an existing directory.\n";
    return false;
  } else if (not stdfs::is_directory(file_dest) &&
	     stdfs::is_directory(file_parent)) {
    std::cout << "\nCreating " << file_dest << '\n';
    stdfs::create_directory(file_dest);
    return true;
  } else if (stdfs::is_directory(file_existing)) {
    if (parents || (file_existing == file_parent)) {
      std::cout << "\nCreating " << file_dest << '\n';
      stdfs::create_directories(file_dest);
      return true;
    } else {
      std::cerr << "\nError (PrepareTarget): invalid target specified.\n";
      std::cerr << file_src << " is an existing directory.\nBut ";
      std::cerr << file_dest << " cannot be created as a directory.\n";
      std::cerr
	<< "\nTo create missing directories either use '-p/--parents' flag or\n";
      std::cerr << "create it manually.\n";
      return false;
    }
  } else {
    std::cerr << "\nError (PrepareTarget): invalid target specified.\n";
    std::cerr << file_src << " is an existing directory.\nBut ";
    std::cerr << file_dest << " cannot be created as a directory.\n";
    return false;
  }
  return false;
}

bool PrepareForFile(const stdfs::path &file_src, const stdfs::path &file_dest,
		    bool parents)
{
  const stdfs::path tmp_dest{ UC::GetCleanPath(file_dest) };
  const stdfs::path file_parent{ tmp_dest.parent_path() };
  const stdfs::path file_existing{ ExistingPath(file_dest) };
  if (stdfs::is_directory(file_dest) || stdfs::exists(file_dest) ||
      stdfs::is_directory(file_parent)) {
    return true;
  } else if (stdfs::is_directory(file_existing) && parents &&
	     (file_existing != file_parent)) {
    std::cout << "\nCreating " << file_parent << '\n';
    stdfs::create_directories(file_parent);
    return true;
  } else if (stdfs::is_directory(file_existing)) {
    if (parents || (file_existing == file_parent)) {
      std::cout << "\nCreating " << file_dest << '\n';
      stdfs::create_directories(file_dest);
      return true;
    } else {
      std::cerr << "\nError (PrepareTarget): invalid target specified.\n";
      std::cerr << file_src << " is an existing file.\nBut ";
      std::cerr << file_dest << " cannot be created as a file.\n";
      std::cerr
	<< "\nTo create missing directories either use '-p/--parents' flag or\n";
      std::cerr << "create it manually.\n";
      return false;
    }
  } else {
    std::cerr << "\nError (PrepareTarget): invalid target specified.\n";
    std::cerr << file_src << " is an existing file.\nBut ";
    std::cerr << file_dest << " cannot be created as a file.\n";
    return false;
  }
  return false;
}

bool PrepareTarget(const std::string &src, const std::string &dest,
		   bool parents)
{
  const stdfs::path file_src{ UC::GetCleanPath(src) };
  const stdfs::path file_dest{ UC::GetCleanPath(dest) };
  if (stdfs::is_directory(file_src)) {
    return PrepareForDirectory(file_src, file_dest, parents);
  } else if (stdfs::exists(file_src)) {
    return PrepareForFile(file_src, file_dest, parents);
  }
  return false;
}

bool CreateLink(const stdfs::path &src, const stdfs::path &dest,
		const LinkOpts &lOptions)
{
  const stdfs::path file_src{ UC::GetCleanPath(src) };
  const stdfs::path file_dest{ UC::GetCleanPath(dest) };
  switch (lOptions.type) {
  case LinkType::relative: {
    // const stdfs::path abs_src{ stdfs::absolute(src) };
    // const stdfs::path abs_dest{ stdfs::absolute(dest) };
    const stdfs::path target{ file_src.lexically_relative(
      file_dest.parent_path()) };
    // const stdfs::path target{ stdfs::relative(src, dest.parent_path()) };
    stdfs::create_symlink(target, file_dest);
    break;
  }
  case LinkType::absolute:
    stdfs::create_symlink(file_src, file_dest);
    break;
  case LinkType::echo:
  default:
    return false;
  }
  return true;
}

bool CreateLinkForce(const stdfs::path &src, const stdfs::path &dest,
		     const LinkOpts &lOptions)
{
  if (lOptions.skip) {
    const stdfs::path file_src{ UC::GetCleanPath(src) };
    const stdfs::path file_dest{ UC::GetCleanPath(dest) };
    std::cout << "\nSkipping Linking : " << file_dest << " to " << file_src
	      << '\n';
    return false;
  }
  if (lOptions.force) {
    std::cout << "\nOverwriting and Linking : " << dest << " to " << src
	      << '\n';
    stdfs::remove(dest);
    return CreateLink(src, dest, lOptions);
  } else if (UC::CheckOverwrite(src, dest)) {
    std::cout << "Overwriting and Linking : " << dest << " to " << src << '\n';
    stdfs::remove(dest);
    return CreateLink(src, dest, lOptions);
  } else {
    std::cout << "Skipping Linking : " << dest << " to " << src << '\n';
    return false;
  }
  return false;
}

LinkStatus_t LinkFile(const std::string &src,
		      const std::string &dest, const LinkOpts &lOptions,
		      bool debug)
{
  LinkStatus_t link_status{ 0, 0 };
  // const auto file_src = stdfs::path(src).lexically_normal();
  const stdfs::path file_src{ UC::GetCleanPath(src) };
  if (not UC::CheckFile(file_src)) {
    std::cerr << "Error (LinkFile). Exiting.\n\n";
    return LinkStatus_t(-1, -1);
  }
  const stdfs::path file_dest{ UC::GetCleanPath(dest) };
  if (not PrepareTarget(file_src, file_dest, lOptions.parents))
    return LinkStatus_t(-1, -1);
  const stdfs::path file_src_elmnt{ file_src.filename() };
  if (debug) {
    std::cout << "\nCheckFileRecursive : " << file_src_elmnt;
    std::cout << " from " << file_src << " against " << file_dest << '\n';
  }
  const auto [result, target] =
    UC::CheckFileRecursive(file_src_elmnt, file_dest, debug);
  if (result) {
    CreateLinkForce(file_src, target, lOptions) ? ++link_status.first :
						  ++link_status.second;
  } else if ((not result) && stdfs::is_directory(file_dest)) {
    std::cout << "\nLinking : " << target << " to " << file_src;
    CreateLink(file_src, target, lOptions) ? ++link_status.first :
					     ++link_status.second;
  } else if (stdfs::exists(file_dest)) {
    CreateLinkForce(file_src, file_dest, lOptions) ? ++link_status.first :
						     ++link_status.second;
  } else if (file_dest.has_parent_path()) {
    std::cout << "\nLinking : " << file_dest << " to " << file_src;
    CreateLink(file_src, file_dest, lOptions) ? ++link_status.first :
						++link_status.second;
  } else {
    ++link_status.second;
  }
  return link_status;
}

LinkStatus_t LinkDirectory(const std::string &src, const std::string &dest,
			   const LinkOpts &lOptions, bool debug)
{
  LinkStatus_t link_status{ 0, 0 };
  const stdfs::path file_src{ UC::GetCleanPath(src) };
  if (not UC::CheckDirectory(file_src)) {
    std::cerr << "Error (LinkDirectory). Exiting.\n\n";
    return LinkStatus_t(-1, -1);
  }
  const stdfs::path file_dest{ UC::GetCleanPath(dest) };
  if (not PrepareTarget(file_src, file_dest, lOptions.parents))
    return LinkStatus_t(-1, -1);
  for (const stdfs::directory_entry &entry :
       stdfs::recursive_directory_iterator(file_src)) {
    const stdfs::path file_entry{ entry };
    const stdfs::path file_entry_elmnt{ file_entry.lexically_relative(
      file_src) };
    const auto [result, target] =
      UC::CheckFileRecursive(file_entry_elmnt, file_dest, debug);
    if (entry.is_directory()) {
      if (not(result || target.empty())) {
	if (debug)
	  std::cout << "\nCreating " << (target) << '\n';
	stdfs::create_directories(target);
      }
    } else {
      if (debug) {
	std::cout << "\nfile_src : " << file_src;
	std::cout << "\nfile_entry : " << file_entry;
	std::cout << "\nfile_entry_elmnt : " << file_entry_elmnt;
	std::cout << "\nCheckFileRecursive : " << file_entry_elmnt;
	std::cout << " from \"" << file_entry << "\" inside " << file_dest
		  << '\n';
      }
      if (result) {
	CreateLinkForce(file_entry, target, lOptions) ? ++link_status.first :
							++link_status.second;
      } else if (not(result || target.empty())) {
	if (stdfs::path(target).has_parent_path() &&
	    not stdfs::is_directory(stdfs::path(target).parent_path())) {
	  stdfs::path target_parent{ stdfs::path(target).parent_path() };
	  if (debug)
	    std::cout << "\nCreating " << target_parent << '\n';
	  stdfs::create_directories(target_parent);
	}
	std::cout << "\nLinking : " << target << " to " << file_entry;
	CreateLink(file_entry, target, lOptions) ? ++link_status.first :
						   ++link_status.second;
      } else {
	++link_status.second;
      }
    }
  }
  return link_status;
}

}
