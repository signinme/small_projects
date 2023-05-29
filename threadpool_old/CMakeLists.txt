cmake_minimum_required(VERSION 2.8)

project(ThreadPoolTest)

link_libraries(pthread)

aux_source_directory(. DIR_SRCS)

include_directories(ThreadPool)

add_subdirectory(ThreadPool)

add_executable(main ${DIR_SRCS})

target_link_libraries(main ThreadPoolLib)