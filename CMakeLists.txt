cmake_minimum_required(VERSION 3.0)

set(mnet_include /home/guest/multinet/multinet/include)
set(mnet_lib_spectra_include /home/guest/multinet/multinet/lib/)
set(mnet_lib_dlib_include /home/guest/multinet/multinet/lib/dlib)
set(mnet_lib_eigen_include /home/guest/multinet/multinet/lib/eigen3)

include_directories (${mnet_include} ${mnet_lib_include} ${mnet_lib_dlib_include} ${mnet_lib_eigen_include})
link_directories(/home/guest/multinet/multinet/)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wextra -DNDEBUG -DDLIB_NO_GUI_SUPPORT -O4 -fopenmp -fmessage-length=0 -funroll-loops -fno-omit-frame-pointer -march=native -fsanitize=address")

file (GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/experiments.cpp)
add_executable (experiments ${CMAKE_CURRENT_SOURCE_DIR}/experiments.cpp)

file (GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/evaluation.cpp)
add_executable (evaluation ${CMAKE_CURRENT_SOURCE_DIR}/evaluation.cpp)

target_link_libraries (experiments -lmultinet -lblas)
target_link_libraries (evaluation -lmultinet -lblas)
