cmake_minimum_required(VERSION 3.5)
project(sample-app-skylink-cpp VERSION 1.0 LANGUAGES CXX)

####################################################################
### CMake config
####################################################################

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/boost_1_65_0/build/include)

include(GNUInstallDirs)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

set(CMAKE_BUILD_TYPE DEBUG)
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}") 

####################################################################
### Dependencies
####################################################################

#Sources
file(GLOB SOURCES "src/*.cpp")

# Find dependencies
find_package (Threads   REQUIRED)
find_package (X11       REQUIRED)
find_package (CURL      REQUIRED)
find_package (SDL2      REQUIRED)
find_package (Boost 1.65.0 COMPONENTS system date_time random REQUIRED)

# Unable to find_package(portaudio) at the moment
add_library(PORT_AUDIO SHARED IMPORTED)
set_target_properties(PORT_AUDIO PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/BUILD/portaudio/lib/libportaudio.so)

# SDL2 found via find_package(SDL2) has trailing whitespaces
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

####################################################################
### SAMPLE APP
####################################################################

set(SAMPLE_APP_NAME "skylinkcpp-demo")
add_executable(${SAMPLE_APP_NAME})

set (SKYLINK_CPP ${PROJECT_SOURCE_DIR}/skylinkcpp)

# Sample app source foles
target_sources(${SAMPLE_APP_NAME} 
              PUBLIC
              ${SOURCES}
              )

# Sample app includes
target_include_directories(${SAMPLE_APP_NAME} 
                           PRIVATE
                           "${SKYLINK_CPP}/include"
                           "${PROJECT_SOURCE_DIR}/third_party/BUILD/portaudio/include"
                           ${SDL2_INCLUDE_DIRS}
                           ${PORTAUDIO_INCLUDE_DIRS}
                           )

# Sample app link
target_link_libraries(${SAMPLE_APP_NAME}
                      PRIVATE
                      ${SKYLINK_CPP}/lib/libskylinkcpp.a 
                      ${CMAKE_THREAD_LIBS_INIT} 
                      ${CMAKE_DL_LIBS}
                      ${X11_LIBRARIES}
                      ${CURL_LIBRARIES}
                      ${SDL2_LIBRARIES}
                      Boost::system
                      Boost::date_time
                      Boost::random
                      PORT_AUDIO
                      )


