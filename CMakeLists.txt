cmake_minimum_required(VERSION 2.9)
project(AlphaLinkage)


SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
SET(CXX TRUE)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(HEADER_FILES "$ENV{GUROBI_HOME}/include/gurobi_c++.h")

FIND_PACKAGE(GUROBI REQUIRED)

INCLUDE_DIRECTORIES(data_reader, lib, types, utils, ${GUROBI_INCLUDE_DIRS})

FILE(GLOB UTIL_SOURCES "utils/*.cpp")
ADD_EXECUTABLE(AlphaLinkage main.cpp ${UTIL_SOURCES} ${HEADER_FILES} lib/Hungarian.cpp lib/Clarkson.cpp lib/AffineFunction.cpp)
TARGET_INCLUDE_DIRECTORIES(AlphaLinkage PUBLIC types PUBLIC utils PUBLIC data_reader PUBLIC lib PUBLIC)
TARGET_INCLUDE_DIRECTORIES(AlphaLinkage PRIVATE $ENV{GUROBI_HOME}/include/)
# SET(GUROBI_CXX_LIBRARY $ENV{GUROBI_HOME}/lib/libgurobi_c++.a)
target_link_libraries(AlphaLinkage ${GUROBI_CXX_LIBRARY} ${GUROBI_LIBRARY})


ADD_EXECUTABLE(DistanceLearning distance_main.cpp ${UTIL_SOURCES} ${HEADER_FILES} lib/Hungarian.cpp)
TARGET_INCLUDE_DIRECTORIES(DistanceLearning PUBLIC types PUBLIC utils PUBLIC data_reader PUBLIC lib PUBLIC)
TARGET_INCLUDE_DIRECTORIES(DistanceLearning PRIVATE $ENV{GUROBI_HOME}/include/)
TARGET_LINK_DIRECTORIES(DistanceLearning INTERFACE $ENV{GUROBI_HOME}/lib/)
