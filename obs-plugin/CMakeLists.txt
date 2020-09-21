project(react-obs)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

set(react-obs_HEADERS
	${react-obs_HEADERS}
	react-obs-main.h
	)
set(react-obs_SOURCES
	react-obs-main.cpp
	generated/protocol.pb.cc
	)

add_library(react-obs MODULE
	${react-obs_HEADERS}
	${react-obs_SOURCES}
	)

add_subdirectory(lib/sockpp)

target_link_libraries(react-obs
	${frontend-tools_PLATFORM_LIBS}
	obs-frontend-api
	libobs
	sockpp
	${PROTOBUF_LIBRARY})

set_target_properties(react-obs PROPERTIES FOLDER "frontend")

install_obs_plugin_with_data(react-obs data)
