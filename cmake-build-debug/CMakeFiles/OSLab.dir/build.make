# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Users/GoldGlaid/CLionProjects/OSLab

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/OSLab.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/OSLab.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/OSLab.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OSLab.dir/flags.make

CMakeFiles/OSLab.dir/lab1/parent.c.o: CMakeFiles/OSLab.dir/flags.make
CMakeFiles/OSLab.dir/lab1/parent.c.o: ../lab1/parent.c
CMakeFiles/OSLab.dir/lab1/parent.c.o: CMakeFiles/OSLab.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/OSLab.dir/lab1/parent.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/OSLab.dir/lab1/parent.c.o -MF CMakeFiles/OSLab.dir/lab1/parent.c.o.d -o CMakeFiles/OSLab.dir/lab1/parent.c.o -c /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/lab1/parent.c

CMakeFiles/OSLab.dir/lab1/parent.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/OSLab.dir/lab1/parent.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/lab1/parent.c > CMakeFiles/OSLab.dir/lab1/parent.c.i

CMakeFiles/OSLab.dir/lab1/parent.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/OSLab.dir/lab1/parent.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/lab1/parent.c -o CMakeFiles/OSLab.dir/lab1/parent.c.s

# Object files for target OSLab
OSLab_OBJECTS = \
"CMakeFiles/OSLab.dir/lab1/parent.c.o"

# External object files for target OSLab
OSLab_EXTERNAL_OBJECTS =

OSLab: CMakeFiles/OSLab.dir/lab1/parent.c.o
OSLab: CMakeFiles/OSLab.dir/build.make
OSLab: CMakeFiles/OSLab.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable OSLab"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OSLab.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OSLab.dir/build: OSLab
.PHONY : CMakeFiles/OSLab.dir/build

CMakeFiles/OSLab.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OSLab.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OSLab.dir/clean

CMakeFiles/OSLab.dir/depend:
	cd /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/GoldGlaid/CLionProjects/OSLab /mnt/c/Users/GoldGlaid/CLionProjects/OSLab /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug /mnt/c/Users/GoldGlaid/CLionProjects/OSLab/cmake-build-debug/CMakeFiles/OSLab.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/OSLab.dir/depend

