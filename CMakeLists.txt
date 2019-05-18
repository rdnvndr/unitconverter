cmake_minimum_required(VERSION 3.10)
project(unitconverter VERSION 1.0.0)

file(GLOB HEADERS   "*.h")
file(GLOB SOURCES   "*.cpp")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
