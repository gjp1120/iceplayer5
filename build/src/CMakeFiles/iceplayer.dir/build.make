# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/GaoJinpei/iceplayer5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/GaoJinpei/iceplayer5/build

# Include any dependencies generated for this target.
include src/CMakeFiles/iceplayer.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/iceplayer.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/iceplayer.dir/flags.make

src/CMakeFiles/iceplayer.dir/gui.cc.o: src/CMakeFiles/iceplayer.dir/flags.make
src/CMakeFiles/iceplayer.dir/gui.cc.o: src/gui.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/GaoJinpei/iceplayer5/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/iceplayer.dir/gui.cc.o"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iceplayer.dir/gui.cc.o -c /home/GaoJinpei/iceplayer5/build/src/gui.cc

src/CMakeFiles/iceplayer.dir/gui.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iceplayer.dir/gui.cc.i"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/GaoJinpei/iceplayer5/build/src/gui.cc > CMakeFiles/iceplayer.dir/gui.cc.i

src/CMakeFiles/iceplayer.dir/gui.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iceplayer.dir/gui.cc.s"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/GaoJinpei/iceplayer5/build/src/gui.cc -o CMakeFiles/iceplayer.dir/gui.cc.s

src/CMakeFiles/iceplayer.dir/gui.cc.o.requires:
.PHONY : src/CMakeFiles/iceplayer.dir/gui.cc.o.requires

src/CMakeFiles/iceplayer.dir/gui.cc.o.provides: src/CMakeFiles/iceplayer.dir/gui.cc.o.requires
	$(MAKE) -f src/CMakeFiles/iceplayer.dir/build.make src/CMakeFiles/iceplayer.dir/gui.cc.o.provides.build
.PHONY : src/CMakeFiles/iceplayer.dir/gui.cc.o.provides

src/CMakeFiles/iceplayer.dir/gui.cc.o.provides.build: src/CMakeFiles/iceplayer.dir/gui.cc.o

src/CMakeFiles/iceplayer.dir/common.cc.o: src/CMakeFiles/iceplayer.dir/flags.make
src/CMakeFiles/iceplayer.dir/common.cc.o: src/common.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/GaoJinpei/iceplayer5/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/iceplayer.dir/common.cc.o"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iceplayer.dir/common.cc.o -c /home/GaoJinpei/iceplayer5/build/src/common.cc

src/CMakeFiles/iceplayer.dir/common.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iceplayer.dir/common.cc.i"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/GaoJinpei/iceplayer5/build/src/common.cc > CMakeFiles/iceplayer.dir/common.cc.i

src/CMakeFiles/iceplayer.dir/common.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iceplayer.dir/common.cc.s"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/GaoJinpei/iceplayer5/build/src/common.cc -o CMakeFiles/iceplayer.dir/common.cc.s

src/CMakeFiles/iceplayer.dir/common.cc.o.requires:
.PHONY : src/CMakeFiles/iceplayer.dir/common.cc.o.requires

src/CMakeFiles/iceplayer.dir/common.cc.o.provides: src/CMakeFiles/iceplayer.dir/common.cc.o.requires
	$(MAKE) -f src/CMakeFiles/iceplayer.dir/build.make src/CMakeFiles/iceplayer.dir/common.cc.o.provides.build
.PHONY : src/CMakeFiles/iceplayer.dir/common.cc.o.provides

src/CMakeFiles/iceplayer.dir/common.cc.o.provides.build: src/CMakeFiles/iceplayer.dir/common.cc.o

src/CMakeFiles/iceplayer.dir/main.cc.o: src/CMakeFiles/iceplayer.dir/flags.make
src/CMakeFiles/iceplayer.dir/main.cc.o: src/main.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/GaoJinpei/iceplayer5/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/iceplayer.dir/main.cc.o"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iceplayer.dir/main.cc.o -c /home/GaoJinpei/iceplayer5/build/src/main.cc

src/CMakeFiles/iceplayer.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iceplayer.dir/main.cc.i"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/GaoJinpei/iceplayer5/build/src/main.cc > CMakeFiles/iceplayer.dir/main.cc.i

src/CMakeFiles/iceplayer.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iceplayer.dir/main.cc.s"
	cd /home/GaoJinpei/iceplayer5/build/src && /usr/lib/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/GaoJinpei/iceplayer5/build/src/main.cc -o CMakeFiles/iceplayer.dir/main.cc.s

src/CMakeFiles/iceplayer.dir/main.cc.o.requires:
.PHONY : src/CMakeFiles/iceplayer.dir/main.cc.o.requires

src/CMakeFiles/iceplayer.dir/main.cc.o.provides: src/CMakeFiles/iceplayer.dir/main.cc.o.requires
	$(MAKE) -f src/CMakeFiles/iceplayer.dir/build.make src/CMakeFiles/iceplayer.dir/main.cc.o.provides.build
.PHONY : src/CMakeFiles/iceplayer.dir/main.cc.o.provides

src/CMakeFiles/iceplayer.dir/main.cc.o.provides.build: src/CMakeFiles/iceplayer.dir/main.cc.o

# Object files for target iceplayer
iceplayer_OBJECTS = \
"CMakeFiles/iceplayer.dir/gui.cc.o" \
"CMakeFiles/iceplayer.dir/common.cc.o" \
"CMakeFiles/iceplayer.dir/main.cc.o"

# External object files for target iceplayer
iceplayer_EXTERNAL_OBJECTS =

src/iceplayer: src/CMakeFiles/iceplayer.dir/gui.cc.o
src/iceplayer: src/CMakeFiles/iceplayer.dir/common.cc.o
src/iceplayer: src/CMakeFiles/iceplayer.dir/main.cc.o
src/iceplayer: /usr/lib/libSM.so
src/iceplayer: /usr/lib/libICE.so
src/iceplayer: /usr/lib/libX11.so
src/iceplayer: /usr/lib/libXext.so
src/iceplayer: /usr/lib/libfreetype.so
src/iceplayer: /usr/lib/libglib-2.0.so
src/iceplayer: /usr/lib/libgobject-2.0.so
src/iceplayer: /usr/lib/libgdk_pixbuf-2.0.so
src/iceplayer: /usr/lib/libgdk-x11-2.0.so
src/iceplayer: /usr/lib/libgtk-x11-2.0.so
src/iceplayer: /usr/lib/libcairo.so
src/iceplayer: /usr/lib/libpango-1.0.so
src/iceplayer: /usr/lib/libatk-1.0.so
src/iceplayer: /usr/lib/libxml2.so
src/iceplayer: /usr/lib/libSM.so
src/iceplayer: /usr/lib/libICE.so
src/iceplayer: /usr/lib/libX11.so
src/iceplayer: /usr/lib/libXext.so
src/iceplayer: /usr/lib/libfreetype.so
src/iceplayer: /usr/lib/libglib-2.0.so
src/iceplayer: /usr/lib/libgobject-2.0.so
src/iceplayer: /usr/lib/libgdk_pixbuf-2.0.so
src/iceplayer: /usr/lib/libgdk-x11-2.0.so
src/iceplayer: /usr/lib/libgtk-x11-2.0.so
src/iceplayer: /usr/lib/libcairo.so
src/iceplayer: /usr/lib/libpango-1.0.so
src/iceplayer: /usr/lib/libatk-1.0.so
src/iceplayer: /usr/lib/libxml2.so
src/iceplayer: src/CMakeFiles/iceplayer.dir/build.make
src/iceplayer: src/CMakeFiles/iceplayer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable iceplayer"
	cd /home/GaoJinpei/iceplayer5/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iceplayer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/iceplayer.dir/build: src/iceplayer
.PHONY : src/CMakeFiles/iceplayer.dir/build

src/CMakeFiles/iceplayer.dir/requires: src/CMakeFiles/iceplayer.dir/gui.cc.o.requires
src/CMakeFiles/iceplayer.dir/requires: src/CMakeFiles/iceplayer.dir/common.cc.o.requires
src/CMakeFiles/iceplayer.dir/requires: src/CMakeFiles/iceplayer.dir/main.cc.o.requires
.PHONY : src/CMakeFiles/iceplayer.dir/requires

src/CMakeFiles/iceplayer.dir/clean:
	cd /home/GaoJinpei/iceplayer5/build/src && $(CMAKE_COMMAND) -P CMakeFiles/iceplayer.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/iceplayer.dir/clean

src/CMakeFiles/iceplayer.dir/depend:
	cd /home/GaoJinpei/iceplayer5/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/GaoJinpei/iceplayer5 /home/GaoJinpei/iceplayer5/src /home/GaoJinpei/iceplayer5/build /home/GaoJinpei/iceplayer5/build/src /home/GaoJinpei/iceplayer5/build/src/CMakeFiles/iceplayer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/iceplayer.dir/depend
