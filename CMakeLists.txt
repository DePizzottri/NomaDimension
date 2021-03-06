cmake_minimum_required(VERSION 3.2)

project(noma_dimention)

if(WIN32)
  set(BOOST_ROOT_DIR "C:/lib/boost_1_66_0")
  include_directories(${BOOST_ROOT_DIR})
  link_directories("${BOOST_ROOT_DIR}/stage_x64/lib")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qvec-report:1 /openmp")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -flto -funroll-loops -std=c++14 -march=native -fopenmp")
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
  set(BOOST_ROOT_DIR "/home/max/fiber_test/boost")
  include_directories(${BOOST_ROOT_DIR})
  link_directories("${BOOST_ROOT_DIR}/stage/lib")
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

if(UNIX)
  target_link_libraries(enumerate_isomorphic "boost_fiber" "boost_context")
endif(UNIX)

add_executable(generate_one_grouped "./app/generate_one_grouped.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

add_executable(enumerate_grouped "./app/enumerate_grouped.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

add_executable(check_poset "./app/check_poset.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})

add_executable(generate_flat_grid "./app/generate_flat_grid.cpp" ${${PROJECT_NAME}_SRC_FILES} ${${PROJECT_NAME}_INCLUDE_FILES})
