cmake_minimum_required(VERSION 3.2)

project(noma_dimention)

if(WIN32)
  set(BOOST_ROOT_DIR "D:/lib/boost")
  include_directories(${BOOST_ROOT_DIR})
  include_directories("C:/lib/json/src")
  link_directories("${BOOST_ROOT_DIR}/stage/lib")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qvec-report:1 /openmp")
else()
  message("Build for UNIX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -flto -funroll-loops -std=c++14 -march=native")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -flto")
  find_package("Boost" REQUIRED COMPONENTS system filesystem thread)
  find_package("Threads")
  include_directories(${Boost_INCLUDE_DIRS})
  include_directories("../date")
  include_directories("../rapidjson/include")
  link_directories(${Boost_LIBRARY_DIRS})
endif(WIN32)

file(GLOB ${PROJECT_NAME}_SRC_FILES
    "src/*"
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC_FILES})

if(WIN32)
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME} jemalloc)
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif(WIN32)

