<img src="icon.jpg" align="right" />
# Honours Project - GPGPU Cracking Hashed Passwords with OpenCL [![Awesome](https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg)](https://github.com/sindresorhus/awesome)

Description TBA

## Status

[![build status](https://gitlab.com/B00233705/Honours-Project/badges/master/build.svg)](https://gitlab.com/B00233705/Honours-Project/commits/master)

## Introduction

TBA

## Requirements

- Intel OpenCL SDK >= 6.0.1 or AMD OpenCL SDK >= 3.0.0 or NVIDIA OpenCL SDK > 8.5.0

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
