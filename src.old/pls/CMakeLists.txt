include(FindGTK2)
include(FindPkgConfig)
include(FindLibXml2)
include(FindX11)

#插件安装路径
set(PLS_INSTALL_DIR ${datadir}/iceplayer/pls)

#fixme:这里不知出什么问题了，glib检测老是出错，只能这样解决
set(CMAKE_INCLUDE_PATH `pkg-config --cflags glib-2.0`)

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

#libxml2
include_directories(${LIBXML2_INCLUDE_DIR})
#Gtk2
include_directories(${GTK2_INCLUDE_DIRS})

set(UPDATER_SRC
	update.c
)

add_library(iceplayer_updater SHARED ${UPDATER_SRC})

target_link_libraries(iceplayer_updater 
	pthread
	${X11_LIBRARIES}
	${GTK2_LIBRARIES}
	${LIBXML2_LIBRARIES}
	${GLIB_LIBRARIES}
	${GST_LIBRARIES}
	${LIBNOTIFY_LIBRARIES}
	${GTK_LIBRARIES}
	)

install(TARGETS iceplayer_updater
    LIBRARY DESTINATION ${PLS_INSTALL_DIR}
    ARCHIVE DESTINATION ${PLS_INSTALL_DIR}
)
