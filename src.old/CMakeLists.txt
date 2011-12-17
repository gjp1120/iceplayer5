include(FindGTK2)
include(FindPkgConfig)
include(FindLibXml2)
include(FindX11)

#fixme:这里不知出什么问题了，glib检测老是出错，只能这样解决
set(CMAKE_INCLUDE_PATH `pkg-config --cflags glib-2.0`)

configure_file(common.h ${CMAKE_CURRENT_BINARY_DIR}/common.h)
configure_file(about.cpp ${CMAKE_CURRENT_BINARY_DIR}/about.cpp)
configure_file(equalizer.c ${CMAKE_CURRENT_BINARY_DIR}/equalizer.c)
configure_file(core.h ${CMAKE_CURRENT_BINARY_DIR}/core.h)
configure_file(keybinding.c ${CMAKE_CURRENT_BINARY_DIR}/keybinding.c)
configure_file(single.c ${CMAKE_CURRENT_BINARY_DIR}/single.c)
configure_file(list.cpp ${CMAKE_CURRENT_BINARY_DIR}/list.cpp)
configure_file(core.cpp ${CMAKE_CURRENT_BINARY_DIR}/core.cpp)
configure_file(ui.cpp ${CMAKE_CURRENT_BINARY_DIR}/ui.cpp)
configure_file(common.cpp ${CMAKE_CURRENT_BINARY_DIR}/common.cpp)
configure_file(id3.c ${CMAKE_CURRENT_BINARY_DIR}/id3.c)
configure_file(lrc.cpp ${CMAKE_CURRENT_BINARY_DIR}/lrc.cpp)
configure_file(configure.c ${CMAKE_CURRENT_BINARY_DIR}/configure.c)
configure_file(desktop_lrc.c ${CMAKE_CURRENT_BINARY_DIR}/desktop_lrc.c)
configure_file(download.c ${CMAKE_CURRENT_BINARY_DIR}/download.c)
configure_file(instance.c ${CMAKE_CURRENT_BINARY_DIR}/instance.c)
configure_file(mini.c ${CMAKE_CURRENT_BINARY_DIR}/mini.c)
configure_file(pl.c ${CMAKE_CURRENT_BINARY_DIR}/pl.c)
configure_file(main.cpp ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)
configure_file(widget_lrc.c ${CMAKE_CURRENT_BINARY_DIR}/widget_lrc.c)
configure_file(mini.h ${CMAKE_CURRENT_BINARY_DIR}/mini.h)
configure_file(equalizer.h ${CMAKE_CURRENT_BINARY_DIR}/equalizer.h)
configure_file(configure.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h)
configure_file(keybinding.h ${CMAKE_CURRENT_BINARY_DIR}/keybinding.h)
configure_file(single.h ${CMAKE_CURRENT_BINARY_DIR}/single.h)
configure_file(list.h ${CMAKE_CURRENT_BINARY_DIR}/list.h)
configure_file(lrc.h ${CMAKE_CURRENT_BINARY_DIR}/lrc.h)
configure_file(about.h ${CMAKE_CURRENT_BINARY_DIR}/about.h)
configure_file(widget_lrc.h ${CMAKE_CURRENT_BINARY_DIR}/widget_lrc.h)
configure_file(id3.h ${CMAKE_CURRENT_BINARY_DIR}/id3.h)
configure_file(ui.h ${CMAKE_CURRENT_BINARY_DIR}/ui.h)
configure_file(instance.h ${CMAKE_CURRENT_BINARY_DIR}/instance.h)
configure_file(desktop_lrc.h ${CMAKE_CURRENT_BINARY_DIR}/desktop_lrc.h)
configure_file(download.h ${CMAKE_CURRENT_BINARY_DIR}/download.h)
configure_file(pl.h ${CMAKE_CURRENT_BINARY_DIR}/pl.h)

set(ICEPLAYER_SRC
	${CMAKE_CURRENT_BINARY_DIR}/about.cpp
	${CMAKE_CURRENT_BINARY_DIR}/equalizer.c
	${CMAKE_CURRENT_BINARY_DIR}/keybinding.c
	${CMAKE_CURRENT_BINARY_DIR}/single.c
	${CMAKE_CURRENT_BINARY_DIR}/list.cpp
	${CMAKE_CURRENT_BINARY_DIR}/core.cpp
	${CMAKE_CURRENT_BINARY_DIR}/ui.cpp
	${CMAKE_CURRENT_BINARY_DIR}/common.cpp
	${CMAKE_CURRENT_BINARY_DIR}/id3.c
	${CMAKE_CURRENT_BINARY_DIR}/lrc.cpp
	${CMAKE_CURRENT_BINARY_DIR}/configure.c
	${CMAKE_CURRENT_BINARY_DIR}/desktop_lrc.c
	${CMAKE_CURRENT_BINARY_DIR}/main.cpp
	${CMAKE_CURRENT_BINARY_DIR}/widget_lrc.c
	${CMAKE_CURRENT_BINARY_DIR}/download.c
	${CMAKE_CURRENT_BINARY_DIR}/instance.c
	${CMAKE_CURRENT_BINARY_DIR}/mini.c
	${CMAKE_CURRENT_BINARY_DIR}/pl.c
)

#gst的检测倒是很理想
pkg_search_module(GST REQUIRED gstreamer-0.10)
if(${GST_FOUND})
	include_directories(${GST_INCLUDEDIR})
else()
	message("gstreamer not found!")
endif()

#glib的几乎没作用，只是为了LIBS存在
pkg_search_module(GLIB REQUIRED glib-2.0)
if(NOT ${GLIB_FOUND})
	message("glib not found!")
endif()

#libnotify
pkg_search_module(LIBNOTIFY REQUIRED libnotify)
if(NOT ${LIBNOTIFY_FOUND})
	message("libnotify not found!")
endif()

#这是为了保证链接完全的
pkg_search_module(GTK REQUIRED gtk+-2.0)
if(NOT ${GTK_FOUND})
	message("Gtk+ not found!")
endif()

pkg_search_module(GTKMM gtkmm-2.4)
if(NOT ${GTKMM_FOUND})
	message("GTKMM not found!")
endif()

#libxml2
include_directories(${LIBXML2_INCLUDE_DIR})
#Gtk2
include_directories(${GTK2_INCLUDE_DIRS})

add_executable(iceplayer ${ICEPLAYER_SRC})
#Gtkmm
include_directories(${GTKMM_INCLUDE_DIRS})

target_link_libraries(iceplayer 
	pthread
	dl
	${X11_LIBRARIES}
	${GTK2_LIBRARIES}
	${LIBXML2_LIBRARIES}
	${GLIB_LIBRARIES}
	${GST_LIBRARIES}
	${LIBNOTIFY_LIBRARIES}
	${GTK_LIBRARIES}
	${GTKMM_LIBRARIES}
	)

install(TARGETS iceplayer
	RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${BIN_INSTALL_DIR}
)

add_subdirectory(pls)
