project(iceplayer)
cmake_minimum_required(VERSION 2.6)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

if(NOT DEFINED BIN_INSTALL_DIR)
	set(BIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
endif()

set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(bindir ${BIN_INSTALL_DIR})
set(libdir ${LIB_INSTALL_DIR})
set(sharedir ${CMAKE_INSTALL_PREFIX}/share)
set(datadir ${CMAKE_INSTALL_PREFIX}/share/iceplayer)
set(localedir ${CMAKE_INSTALL_PREFIX}/share/locale)
set(includedir  ${CMAKE_INSTALL_PREFIX}/include/iceplayer)
set(CMAKE_C_FLAGS "-Wall -Os ${CMAKE_C_FLAGS}")
set(CMAKE_LINKER_FLAGS "-Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAG}")

add_subdirectory(Sources)
add_subdirectory(Data)
