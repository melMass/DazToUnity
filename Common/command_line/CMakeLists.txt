# set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MACOSX_RPATH TRUE)

# add the automatically determined parts of the RPATH which point to directories
# outside the build tree to the install RPATH
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# if(WIN32) set(OS_SOURCES SceneLoaderApp.rc ) elseif(APPLE) set(OS_SOURCES "")
# endif()
set(OS_SOURCES "")
set(APPLICATION_TGT_NAME "FixFBX")

add_executable(${APPLICATION_TGT_NAME} FixFBX.cpp ${OS_SOURCES})

target_link_libraries(
  ${APPLICATION_TGT_NAME} PRIVATE
  common
  # CONAN_PKG::nlohmann_json
  CONAN_PKG::cli11
  CONAN_PKG::fmt
  CONAN_PKG::simdjson
  # dzcore ${DZSDK_QT_CORE_TARGET} ${DZSDK_QT_GUI_TARGET}
)

if(APPLE)
  set_target_properties(
    ${APPLICATION_TGT_NAME} PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks")
elseif(WIN32)
  target_compile_definitions(${APPLICATION_TGT_NAME} PRIVATE -D_CONSOLE)
endif()

# set_target_properties (${APPLICATION_TGT_NAME} PROPERTIES FOLDER "SDK
# Samples/Applications" )

# source_group("Resource Files" REGULAR_EXPRESSION ".*\\.(rc|ico|icns)")

install(
  TARGETS ${APPLICATION_TGT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION plugins)
