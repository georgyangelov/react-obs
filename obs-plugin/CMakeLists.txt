project(react-obs)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

set(react-obs_HEADERS
	${react-obs_HEADERS}
	react-obs-main.h
	api-server.hpp
	experiments.hpp
	layout.hpp
	obs-adapter.hpp
	props.hpp
	shadow-sources.hpp
	yoga-logger.hpp
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
add_subdirectory(lib/yoga)

target_link_libraries(react-obs
	${frontend-tools_PLATFORM_LIBS}
	obs-frontend-api
	libobs
	sockpp
	yogacore
	${PROTOBUF_LIBRARY})

set_target_properties(react-obs PROPERTIES FOLDER "frontend")

install_obs_plugin_with_data(react-obs data)
