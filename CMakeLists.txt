cmake_minimum_required(VERSION 3.2)

project(noma_dimention)

if(WIN32)
  set(BOOST_ROOT_DIR "D:/lib/boost")
  include_directories(${BOOST_ROOT_DIR})
  link_directories("${BOOST_ROOT_DIR}/stage/lib")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qvec-report:1 /openmp")
else()
  message("Build for UNIX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -flto -funroll-loops -std=c++14 -march=native")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -flto")
endif(WIN32)

file(GLOB ${PROJECT_NAME}_SRC_FILES
    "src/*"
)

file(GLOB ${PROJECT_NAME}_INCLUDE_FILES
    "include/*"
)

include_directories("./include")

add_executable(common "./app/common.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

add_executable(enumerate_isomorphic "./app/enumerate_isomorphic.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

add_executable(generate_one_grouped "./app/generate_one_grouped.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

add_executable(enumerate_grouped "./app/enumerate_grouped.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

