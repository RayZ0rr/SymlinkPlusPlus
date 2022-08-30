<h1 align="center">
  Symlink++
</h1>
<p align="center"><i>Manage symlinks like a pro</i>.</p>

`symlink++`is a tool to help you create symlinks for files and/or folders from a `source` to a `target`. If there are conflicting files with the same name it will ask for user confirmation before proceeding. It also comes with multiple flags to help manipulate the linking features such as :
- *absolute* or *relative* linking ( *relative* linking is done by default )
- *create missing folders*
- *Overwrite/Skip* exisitng files.
and some more. More details regarding the flag and usage is detailed below.

# Installation

1. Clone the repository
	```
	git clone https://github.com/RayZ0rr/SymlinkPlusPlus.git
	```
2. Install it.
	```
	make install
	```
By default, the binary is created in `bin` directory in the repository and copied into `~/.local/bin`. So, make sure `~/.local/bin` exists.

# Usage

The basic command line usage is as follows:
```bash
symlink++ [FLAGS] [--] [source] [target]
```
here `source` can be a file or directory. The `target` can be existing or non-exisiting but incase the `source` is a directory, the `target` will be considered as directory only. If `--` is provided then flags parsing will stop after it and only source and destination will be taken.

## Flags

The following flags are available :
- `-n,--dry-run` :- Show the possible linkings that will happen without actually linking the files.
- `-p,--parents` :- Create any missing parent directories of `target`.
- `-A,--absolute` :- Use *absolute* linking instead of *relative* linking.
- `-S,--skip-existing` :- Don't ask for **Overwrite** confirmation on exisiting files and skip them directly.
- `-f,--force` :- force overwrite without asking for confirmation.
- `-D,--debug` :-Print more information in the output ( usually for debugging purposes ).
