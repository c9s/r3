# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find judy
# Find the native Judy headers and libraries.
#
# Judy_INCLUDE_DIRS	- where to find judy.h, etc.
# Judy_LIBRARIES	- List of libraries when using judy.
# Judy_FOUND	- True if judy found.

# Look for the header file.
FIND_PATH(Judy_INCLUDE_DIR NAMES Judy.h)

# Look for the library.
FIND_LIBRARY(Judy_LIBRARY NAMES judy)

# Handle the QUIETLY and REQUIRED arguments and set Judy_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Judy DEFAULT_MSG Judy_LIBRARY Judy_INCLUDE_DIR)

# Copy the results to the output variables.
IF(Judy_FOUND)
	SET(Judy_LIBRARIES ${Judy_LIBRARY})
	SET(Judy_INCLUDE_DIRS ${Judy_INCLUDE_DIR})
ELSE(Judy_FOUND)
	SET(Judy_LIBRARIES)
	SET(Judy_INCLUDE_DIRS)
ENDIF(Judy_FOUND)

MARK_AS_ADVANCED(Judy_INCLUDE_DIRS Judy_LIBRARIES)
