# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/john/code/rockup/contract

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/john/code/rockup/build

# Include any dependencies generated for this target.
include CMakeFiles/rockup.wasm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/rockup.wasm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/rockup.wasm.dir/flags.make

CMakeFiles/rockup.wasm.dir/rockup.cpp.o: CMakeFiles/rockup.wasm.dir/flags.make
CMakeFiles/rockup.wasm.dir/rockup.cpp.o: /home/john/code/rockup/contract/rockup.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/john/code/rockup/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/rockup.wasm.dir/rockup.cpp.o"
	/usr/bin/eosio-cpp   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rockup.wasm.dir/rockup.cpp.o -c /home/john/code/rockup/contract/rockup.cpp

CMakeFiles/rockup.wasm.dir/rockup.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rockup.wasm.dir/rockup.cpp.i"
	/usr/bin/eosio-cpp  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/john/code/rockup/contract/rockup.cpp > CMakeFiles/rockup.wasm.dir/rockup.cpp.i

CMakeFiles/rockup.wasm.dir/rockup.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rockup.wasm.dir/rockup.cpp.s"
	/usr/bin/eosio-cpp  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/john/code/rockup/contract/rockup.cpp -o CMakeFiles/rockup.wasm.dir/rockup.cpp.s

CMakeFiles/rockup.wasm.dir/rockup.cpp.o.requires:

.PHONY : CMakeFiles/rockup.wasm.dir/rockup.cpp.o.requires

CMakeFiles/rockup.wasm.dir/rockup.cpp.o.provides: CMakeFiles/rockup.wasm.dir/rockup.cpp.o.requires
	$(MAKE) -f CMakeFiles/rockup.wasm.dir/build.make CMakeFiles/rockup.wasm.dir/rockup.cpp.o.provides.build
.PHONY : CMakeFiles/rockup.wasm.dir/rockup.cpp.o.provides

CMakeFiles/rockup.wasm.dir/rockup.cpp.o.provides.build: CMakeFiles/rockup.wasm.dir/rockup.cpp.o


# Object files for target rockup.wasm
rockup_wasm_OBJECTS = \
"CMakeFiles/rockup.wasm.dir/rockup.cpp.o"

# External object files for target rockup.wasm
rockup_wasm_EXTERNAL_OBJECTS =

rockup.wasm: CMakeFiles/rockup.wasm.dir/rockup.cpp.o
rockup.wasm: CMakeFiles/rockup.wasm.dir/build.make
rockup.wasm: CMakeFiles/rockup.wasm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/john/code/rockup/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable rockup.wasm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rockup.wasm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/rockup.wasm.dir/build: rockup.wasm

.PHONY : CMakeFiles/rockup.wasm.dir/build

CMakeFiles/rockup.wasm.dir/requires: CMakeFiles/rockup.wasm.dir/rockup.cpp.o.requires

.PHONY : CMakeFiles/rockup.wasm.dir/requires

CMakeFiles/rockup.wasm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rockup.wasm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rockup.wasm.dir/clean

CMakeFiles/rockup.wasm.dir/depend:
	cd /home/john/code/rockup/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/john/code/rockup/contract /home/john/code/rockup/contract /home/john/code/rockup/build /home/john/code/rockup/build /home/john/code/rockup/build/CMakeFiles/rockup.wasm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rockup.wasm.dir/depend

