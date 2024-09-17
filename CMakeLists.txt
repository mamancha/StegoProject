cmake_minimum_required(VERSION 3.5.0)
project(StegoProject VERSION 0.1.0 LANGUAGES C)

add_executable(StegoProject Extract.c Hide.c StegoProgram.c Tools.c)

include(CTest)
enable_testing()

