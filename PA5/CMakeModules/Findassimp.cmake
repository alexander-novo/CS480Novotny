if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ASSIMP_ARCHITECTURE "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(ASSIMP_ARCHITECTURE "32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

find_path(
  assimp_INCLUDE_DIRS
  NAMES postprocess.h scene.h version.h config.h cimport.h
  PATHS /usr/
        /usr/include/
        /usr/local/include/
        ../libs/assimp/
        ../libs/
        ${ASSIMP_ROOT_DIR}/include/
)

find_library(
  assimp_LIBRARIES
  NAMES assimp
  PATHS /usr/
        /usr/include/
        /usr/local/lib/
        /usr/lib/
        ../libs/
        ../libs/assimp/
        ${ASSIMP_ROOT_DIR}lib/
)

if (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
  SET(assimp_FOUND TRUE)
ENDIF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)

if (assimp_FOUND)
  if (NOT assimp_FIND_QUIETLY)
  message(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
  endif (NOT assimp_FIND_QUIETLY)
else (assimp_FOUND)
  if (assimp_FIND_REQUIRED)
  message(FATAL_ERROR "Could not find asset importer library")
  endif (assimp_FIND_REQUIRED)
endif (assimp_FOUND)

