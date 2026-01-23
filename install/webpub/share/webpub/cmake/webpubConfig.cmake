# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_webpub_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED webpub_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(webpub_FOUND FALSE)
  elseif(NOT webpub_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(webpub_FOUND FALSE)
  endif()
  return()
endif()
set(_webpub_CONFIG_INCLUDED TRUE)

# output package information
if(NOT webpub_FIND_QUIETLY)
  message(STATUS "Found webpub: 0.0.0 (${webpub_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'webpub' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT webpub_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(webpub_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${webpub_DIR}/${_extra}")
endforeach()
