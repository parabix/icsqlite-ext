macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

# Looking for Parabix in PARABIX_PATH
set (PARABIX_INCLUDE_DIR ${PARABIX_PATH}/include)

find_library(PARABIX_LIBRARIES NAMES parabix libparabix
  NO_DEFAULT_PATH
  PATHS ${PARABIX_PATH}
  PATH_SUFFIXES lib bin
)

if (NOT PARABIX_LIBRARIES)
  SUBDIRLIST(SUBDIRS ${PARABIX_PATH})
  foreach(DIR ${SUBDIRS})
    find_library(PARABIX_LIBRARIES NAMES parabix libparabix
      NO_DEFAULT_PATH
      PATHS ${PARABIX_PATH}/${DIR}
      PATH_SUFFIXES lib bin
    )
    if (PARABIX_LIBRARIES)
        break()
    endif()
  endforeach()
endif()

# handle the QUIETLY and REQUIRED arguments and set PARABIX_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Parabix
                                  REQUIRED_VARS PARABIX_LIBRARIES PARABIX_INCLUDE_DIR)

mark_as_advanced(PARABIX_INCLUDE_DIR PARABIX_LIBRARIES)

