#**********************************************************************
#	Copyright (C) 2002-2020 Daz 3D, Inc. All Rights Reserved.
#
#	This file is part of the DAZ Studio SDK.
#
#	This file may be used only in accordance with the DAZ Studio SDK
#	license provided with the DAZ Studio SDK.
#
#	The contents of this file may not be disclosed to third parties,
#	copied or duplicated in any form, in whole or in part, without the
#	prior written permission of Daz 3D, Inc, except as explicitly
#	allowed in the DAZ Studio SDK license.
#
#	See http://www.daz3d.com to contact DAZ 3D or for more
#	information about the DAZ Studio SDK.
#**********************************************************************

cmake_minimum_required(VERSION 3.4.0)

if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif(APPLE)

project("DazRunTimePlugins")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DAZ_STUDIO_EXE_DIR "" CACHE PATH "Path to DAZ Studio, needs to be installed to a writeable location" )
if(NOT DAZ_STUDIO_EXE_DIR )
	message("Location to DAZ Studio not provided. Projects will build locally.")
endif()

add_subdirectory("SDK")

find_package(Qt4 4.8.1 REQUIRED QtCore QtGui QtScript QtOpenGL QtNetwork QtSql QtXml)

add_subdirectory("Common")
add_subdirectory("Unity")
# #add_subdirectory("Unreal")
