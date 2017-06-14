cmake_minimum_required(VERSION 3.0)

set(MNET_PATH ../multinet/multinet)

set(MNET_INCLUDE ${MNET_PATH}/include)
set(MNET_SPECTRA ${MNET_PATH}/lib/spectra)
set(MNET_DLIB ${MNET_PATH}/lib/dlib)
set(MNET_EIGEN ${MNET_PATH}/lib/eigen3)

include_directories (${MNET_INCLUDE} ${MNET_SPECTRA} ${MNET_DLIB} ${MNET_EIGEN})
link_directories(${MNET_PATH})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wextra -DNDEBUG -DDLIB_NO_GUI_SUPPORT -O4 -fopenmp -fmessage-length=0 -funroll-loops -fno-omit-frame-pointer -march=native -fsanitize=address")

add_library (mlnet_evaluation SHARED utils.cpp)

add_executable (performance ${CMAKE_CURRENT_SOURCE_DIR}/performance.cpp)
add_executable (accuracy ${CMAKE_CURRENT_SOURCE_DIR}/accuracy.cpp)
add_executable (dk ${CMAKE_CURRENT_SOURCE_DIR}/dk.cpp)

target_link_libraries (performance mlnet_evaluation -lmultinet -lblas)
target_link_libraries (dk mlnet_evaluation -lmultinet -lblas)
target_link_libraries (accuracy mlnet_evaluation -lmultinet -lblas)
