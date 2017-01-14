<img src="project-logo.jpg" align="right" />
# Honours Project [![Awesome](https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg)](https://github.com/sindresorhus/awesome)

A **GPGPU cracking hashed passwords library** written in C/C++ developed to illustrate a brute-force attack against a MD5 hashed password. Includes additional support for other types of attacks (dictionary) and hashing algorithms (e.g. SHA1). The library is based on **OpenCL** which allow the user to change between CPU and GPU execution or run in benchmark mode the cracking process.

## Status

[![build status](https://gitlab.com/B00233705/Honours-Project/badges/master/build.svg)](https://gitlab.com/B00233705/Honours-Project/commits/master)

## Introduction

Due to the now available massive computational power provided by grid computing systems or specialised hardware like FPGA there is a higher probability of successful password cracking. The library implements a indexed-based word generator on the GPU followed by a meet-in-the-middle attack of the hashed algorithm to perform an fast cracking process. The application will provide an interface of communication with the library to execute the cracking attack.

## Requirements

- CMake v3.2 – generate project files for the IDE
- Visual Studio 14 – development environment
- OpenCL Driver 1.1 – execute OpenCL kernels
- Docopt – library to create a user command line support for the application
- VC++ v19 – compiler with C++ 11 features for the IDE

## How To Install

Minimum setup for an application to load the HashCrack library after the binary files have been installed on local machine.

``` cmake
cmake_minimum_required (VERSION 3.2)

project (demo VERSION 1.0.0)

find_package (HonoursProject 1.0.0 REQUIRED COMPONENTS HashCrack CONFIG)

add_executable (${PROJECT_NAME} main.cpp)

target_link_libraries (${PROJECT_NAME} HonoursProject::HashCrack)

add_custom_command (TARGET ${PROJECT_NAME} 
	POST_BUILD COMMAND ${CMAKE_COMMAND} 
		-E copy_if_different "$<TARGET_FILE_DIR:HonoursProject::HashCrack>/$<TARGET_FILE_NAME:HonoursProject::HashCrack>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
```
