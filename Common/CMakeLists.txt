set(CMAKE_AUTOMOC ON)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(COMMON_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(COMMON_LIB_INCLUDE_DIR ${COMMON_LIB_INCLUDE_DIR} PARENT_SCOPE)

include_directories(${COMMON_LIB_INCLUDE_DIR})
set(LIB_SRCS
	"DzRuntimePluginAction.cpp"
	"DzThread.cpp"
	"debug.cpp"
	"DzFbxUtils.cpp"
	"DzUtils.cpp"
	"DTUConfig.h"
)

add_library(common STATIC ${LIB_SRCS})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common
	PUBLIC

	dzcore
	${DZSDK_QT_CORE_TARGET}
	${DZSDK_QT_GUI_TARGET}
	${DZSDK_QT_SCRIPT_TARGET}
	${DZSDK_QT_NETWORK_TARGET}
	"/Users/imac/Daz 3D/Applications/DAZ 3D/DAZStudio4 64-bit/Frameworks/libfbxsdk.dylib"
	# CONAN_PKG::fbxsdk
	# CONAN_PKG::fmt

)
print_target_properties(common)
