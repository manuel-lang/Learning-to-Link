cmake_minimum_required(VERSION 2.8)
project(AlphaLinkage)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_BUILD_TYPE Release)

INCLUDE_DIRECTORIES(data_reader, lib, types, utils)

FILE(GLOB UTIL_SOURCES "utils/*.cpp")
ADD_EXECUTABLE(AlphaLinkage main.cpp ${UTIL_SOURCES} lib/Hungarian.cpp)
TARGET_INCLUDE_DIRECTORIES(AlphaLinkage PUBLIC types PUBLIC utils PUBLIC data_reader)

ADD_EXECUTABLE(DistanceLearning distance_main.cpp ${UTIL_SOURCES} lib/Hungarian.cpp)
TARGET_INCLUDE_DIRECTORIES(DistanceLearning PUBLIC types PUBLIC utils PUBLIC data_reader)
