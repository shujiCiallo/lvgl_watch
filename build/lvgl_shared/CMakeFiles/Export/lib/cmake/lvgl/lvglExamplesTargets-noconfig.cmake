#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lvgl::lvgl_examples" for configuration ""
set_property(TARGET lvgl::lvgl_examples APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(lvgl::lvgl_examples PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/liblvgl_examples.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS lvgl::lvgl_examples )
list(APPEND _IMPORT_CHECK_FILES_FOR_lvgl::lvgl_examples "${_IMPORT_PREFIX}/lib/liblvgl_examples.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
