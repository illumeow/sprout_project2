# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /src/mainbot

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /src/mainbot/build

# Include any dependencies generated for this target.
include CMakeFiles/mainbot.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mainbot.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mainbot.dir/flags.make

CMakeFiles/mainbot.dir/src/main.cpp.o: CMakeFiles/mainbot.dir/flags.make
CMakeFiles/mainbot.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/src/mainbot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mainbot.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mainbot.dir/src/main.cpp.o -c /src/mainbot/src/main.cpp

CMakeFiles/mainbot.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mainbot.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /src/mainbot/src/main.cpp > CMakeFiles/mainbot.dir/src/main.cpp.i

CMakeFiles/mainbot.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mainbot.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /src/mainbot/src/main.cpp -o CMakeFiles/mainbot.dir/src/main.cpp.s

# Object files for target mainbot
mainbot_OBJECTS = \
"CMakeFiles/mainbot.dir/src/main.cpp.o"

# External object files for target mainbot
mainbot_EXTERNAL_OBJECTS =

mainbot: CMakeFiles/mainbot.dir/src/main.cpp.o
mainbot: CMakeFiles/mainbot.dir/build.make
mainbot: /usr/lib/x86_64-linux-gnu/libcrypto.so
mainbot: /usr/lib/x86_64-linux-gnu/libssl.so
mainbot: /usr/local/lib/libdpp.so
mainbot: CMakeFiles/mainbot.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/src/mainbot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable mainbot"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mainbot.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mainbot.dir/build: mainbot

.PHONY : CMakeFiles/mainbot.dir/build

CMakeFiles/mainbot.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mainbot.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mainbot.dir/clean

CMakeFiles/mainbot.dir/depend:
	cd /src/mainbot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /src/mainbot /src/mainbot /src/mainbot/build /src/mainbot/build /src/mainbot/build/CMakeFiles/mainbot.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mainbot.dir/depend
