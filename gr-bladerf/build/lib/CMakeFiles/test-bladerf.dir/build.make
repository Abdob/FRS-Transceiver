# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build

# Include any dependencies generated for this target.
include lib/CMakeFiles/test-bladerf.dir/depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/test-bladerf.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/test-bladerf.dir/flags.make

lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o: lib/CMakeFiles/test-bladerf.dir/flags.make
lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o: ../lib/test_bladerf.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-bladerf.dir/test_bladerf.cc.o -c /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/test_bladerf.cc

lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-bladerf.dir/test_bladerf.cc.i"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/test_bladerf.cc > CMakeFiles/test-bladerf.dir/test_bladerf.cc.i

lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-bladerf.dir/test_bladerf.cc.s"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/test_bladerf.cc -o CMakeFiles/test-bladerf.dir/test_bladerf.cc.s

lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.requires:

.PHONY : lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.requires

lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.provides: lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.requires
	$(MAKE) -f lib/CMakeFiles/test-bladerf.dir/build.make lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.provides.build
.PHONY : lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.provides

lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.provides.build: lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o


lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o: lib/CMakeFiles/test-bladerf.dir/flags.make
lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o: ../lib/qa_bladerf.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o -c /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/qa_bladerf.cc

lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-bladerf.dir/qa_bladerf.cc.i"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/qa_bladerf.cc > CMakeFiles/test-bladerf.dir/qa_bladerf.cc.i

lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-bladerf.dir/qa_bladerf.cc.s"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/qa_bladerf.cc -o CMakeFiles/test-bladerf.dir/qa_bladerf.cc.s

lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.requires:

.PHONY : lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.requires

lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.provides: lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.requires
	$(MAKE) -f lib/CMakeFiles/test-bladerf.dir/build.make lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.provides.build
.PHONY : lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.provides

lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.provides.build: lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o


lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o: lib/CMakeFiles/test-bladerf.dir/flags.make
lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o: ../lib/qa_single_rx.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o -c /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/qa_single_rx.cc

lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-bladerf.dir/qa_single_rx.cc.i"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/qa_single_rx.cc > CMakeFiles/test-bladerf.dir/qa_single_rx.cc.i

lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-bladerf.dir/qa_single_rx.cc.s"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib/qa_single_rx.cc -o CMakeFiles/test-bladerf.dir/qa_single_rx.cc.s

lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.requires:

.PHONY : lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.requires

lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.provides: lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.requires
	$(MAKE) -f lib/CMakeFiles/test-bladerf.dir/build.make lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.provides.build
.PHONY : lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.provides

lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.provides.build: lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o


# Object files for target test-bladerf
test__bladerf_OBJECTS = \
"CMakeFiles/test-bladerf.dir/test_bladerf.cc.o" \
"CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o" \
"CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o"

# External object files for target test-bladerf
test__bladerf_EXTERNAL_OBJECTS =

lib/test-bladerf: lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o
lib/test-bladerf: lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o
lib/test-bladerf: lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o
lib/test-bladerf: lib/CMakeFiles/test-bladerf.dir/build.make
lib/test-bladerf: /usr/local/lib/libgnuradio-runtime.so
lib/test-bladerf: /usr/local/lib/libgnuradio-pmt.so
lib/test-bladerf: /usr/local/lib/liblog4cpp.so
lib/test-bladerf: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
lib/test-bladerf: /usr/lib/x86_64-linux-gnu/libboost_system.so
lib/test-bladerf: /usr/lib/x86_64-linux-gnu/libcppunit.so
lib/test-bladerf: lib/libgnuradio-bladerf-1.0.0git.so.0.0.0
lib/test-bladerf: /usr/local/lib/libgnuradio-runtime.so
lib/test-bladerf: /usr/local/lib/libgnuradio-pmt.so
lib/test-bladerf: /usr/local/lib/liblog4cpp.so
lib/test-bladerf: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
lib/test-bladerf: /usr/lib/x86_64-linux-gnu/libboost_system.so
lib/test-bladerf: lib/CMakeFiles/test-bladerf.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable test-bladerf"
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-bladerf.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/CMakeFiles/test-bladerf.dir/build: lib/test-bladerf

.PHONY : lib/CMakeFiles/test-bladerf.dir/build

lib/CMakeFiles/test-bladerf.dir/requires: lib/CMakeFiles/test-bladerf.dir/test_bladerf.cc.o.requires
lib/CMakeFiles/test-bladerf.dir/requires: lib/CMakeFiles/test-bladerf.dir/qa_bladerf.cc.o.requires
lib/CMakeFiles/test-bladerf.dir/requires: lib/CMakeFiles/test-bladerf.dir/qa_single_rx.cc.o.requires

.PHONY : lib/CMakeFiles/test-bladerf.dir/requires

lib/CMakeFiles/test-bladerf.dir/clean:
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib && $(CMAKE_COMMAND) -P CMakeFiles/test-bladerf.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/test-bladerf.dir/clean

lib/CMakeFiles/test-bladerf.dir/depend:
	cd /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/lib /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib /home/foci2/Documents/sdr/FRS-Transceiver/gr-bladerf/build/lib/CMakeFiles/test-bladerf.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/CMakeFiles/test-bladerf.dir/depend

