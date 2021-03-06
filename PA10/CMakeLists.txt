PROJECT(Tutorial)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#export compile commands for use in visualizations
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules")
add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
find_package(ImageMagick COMPONENTS Magick++ REQUIRED )
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_Mixer REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(GLM REQUIRED)
FIND_PACKAGE(ASSIMP REQUIRED)
FIND_PACKAGE(Bullet REQUIRED)

SET(CXX11_FLAGS -std=gnu++11)
SET(CDEBUG_FLAGS -g)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS} ${CDEBUG_FLAGS}")
SET(TARGET_LIBRARIES "${OPENGL_LIBRARY} ${SDL2_LIBRARY} ${ASSIMP_LIBRARIES} ${BULLET_LIBRARIES} ")

IF(UNIX)
  ADD_DEFINITIONS(-DUNIX)
ENDIF(UNIX)

IF(NOT APPLE)
  IF(GLEW_FOUND)
      INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIRS})
      LINK_LIBRARIES(${GLEW_LIBRARIES})
  ENDIF(GLEW_FOUND)
  IF(ASSIMP_FOUND)
      INCLUDE_DIRECTORIES(${ASSIMP_INCLUDE_DIRS})
      LINK_LIBRARIES(${ASSIMP_LIBRARIES})
  ENDIF(ASSIMP_FOUND)
ENDIF(NOT APPLE)

# Set Includdes
INCLUDE_DIRECTORIES(
  "${PROJECT_SOURCE_DIR}/include"
  ${SDL2_INCLUDE_DIR}
  ${SDL2_MIXER_INCLUDE_DIR}
  ${GLM_INCLUDE_DIRS}
  ${ASSIMP_INCLUDE_DIRS}
  ${ImageMagick_INCLUDE_DIRS}
  ${BULLET_INCLUDE_DIRS}
)

# Copy shaders, models, and default config
FILE(COPY src/shaders DESTINATION .)
FILE(COPY models DESTINATION .)
FILE(COPY textures DESTINATION .)
FILE(COPY sounds DESTINATION .)
FILE(COPY config.json DESTINATION .)

# Set sources
FILE(GLOB_RECURSE SOURCES "src/*.cpp")
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

MESSAGE(STATUS "ImageMagick: " ${ImageMagick_INCLUDE_DIRS})
MESSAGE(STATUS "ImageMagick: " ${ImageMagick_LIBRARIES})

add_custom_target("${PROJECT_NAME}_SUCCESSFUL" ALL
                  DEPENDS ${PROJECT_NAME}
                  COMMAND ${CMAKE_COMMAND} -E echo ""
                  COMMAND ${CMAKE_COMMAND} -E echo "====================="
                  COMMAND ${CMAKE_COMMAND} -E echo "  Compile complete!"
                  COMMAND ${CMAKE_COMMAND} -E echo "====================="
                  COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_CURRENT_BINARY_DIR}"
                 )

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OPENGL_LIBRARY} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY} ${ASSIMP_LIBRARY} ${ImageMagick_LIBRARIES} ${BULLET_LIBRARIES})
