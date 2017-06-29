cmake_minimum_required(VERSION 3.0)

set(MNET_PATH ../multinet/multinet)

set(MNET_INCLUDE ${MNET_PATH}/include)
set(MNET_SPECTRA ${MNET_PATH}/lib/spectra)
set(MNET_DLIB ${MNET_PATH}/lib/dlib)
set(MNET_EIGEN ${MNET_PATH}/lib/eigen3)

include_directories (${MNET_INCLUDE} ${MNET_SPECTRA} ${MNET_DLIB} ${MNET_EIGEN})
link_directories(${MNET_PATH})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wextra -DNDEBUG -DDLIB_NO_GUI_SUPPORT -O4 -fopenmp -fmessage-length=0 -funroll-loops -fno-omit-frame-pointer -march=native")

add_library (mlnet_evaluation SHARED utils.cpp)

add_executable (synthetic ${CMAKE_CURRENT_SOURCE_DIR}/synthetic.cpp)
add_executable (real ${CMAKE_CURRENT_SOURCE_DIR}/real.cpp)

target_link_libraries (synthetic mlnet_evaluation -lmultinet -lblas)
target_link_libraries (real mlnet_evaluation -lmultinet -lblas)
