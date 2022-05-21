
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
)

add_library(common STATIC ${LIB_SRCS})

target_link_libraries(common
	PRIVATE
	${DAZ_SDK_DZCORE_LIB}
	${DZSDK_QT_CORE_TARGET}
	${DZSDK_QT_GUI_TARGET}
	${DZSDK_QT_SCRIPT_TARGET}
	${DZSDK_QT_NETWORK_TARGET}
)
