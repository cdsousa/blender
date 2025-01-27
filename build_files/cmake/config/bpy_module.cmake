# SPDX-FileCopyrightText: 2011-2022 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

# defaults for building blender as a python module 'bpy'
#
# Example usage:
#   cmake -C../blender/build_files/cmake/config/bpy_module.cmake  ../blender
#

set(WITH_PYTHON_MODULE       ON  CACHE BOOL "" FORCE)


# -----------------------------------------------------------------------------
# Installation Configuration.
#
# NOTE: `WITH_INSTALL_PORTABLE` always defaults to ON when building as a Python module and
# isn't set here as it makes changing the setting impractical.
# Python-developers could prefer either ON/OFF depending on their usage:
#
# - When using the system's Python, disabling will install into their `site-packages`,
#   allowing them to run Python from any directory and `import bpy`.
# - When using Blender's bundled Python in `./../lib/` it will install there
#   which isn't especially useful as it requires running Python from this directory too.
#
# So default `WITH_INSTALL_PORTABLE` to ON, and developers who don't use Python from `./../lib/`
# can disable it if they wish to install into their systems Python.

# There is no point in copying python into Python.
set(WITH_PYTHON_INSTALL      OFF CACHE BOOL "" FORCE)

if(WIN32)
  set(WITH_WINDOWS_BUNDLE_CRT  OFF CACHE BOOL "" FORCE)
endif()


# -----------------------------------------------------------------------------
# Library Compatibility.

# JEMALLOC does not work with `dlopen()` of Python modules:
# https://github.com/jemalloc/jemalloc/issues/1237
set(WITH_MEM_JEMALLOC        OFF CACHE BOOL "" FORCE)

