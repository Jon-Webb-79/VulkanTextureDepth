@echo off
REM ================================================================================
REM ================================================================================
REM File:    build.bat
REM Purpose: This file contains a script that will build C and C++ software
REM          using CMake
REM
REM Source Metadata
REM Author:  Jonathan A. Webb
REM Date:    February 26, 2022
REM Version: 1.0
REM Copyright: Copyright 2022, Jon Webb Inc.
REM ================================================================================
REM ================================================================================

REM Navigate to the directory where CMake is needed
cd /d ../../VulkanApplication/

REM Run CMake configuration for the project
cmake -S . -B build\debug -DCMAKE_BUILD_TYPE=Debug

REM Build the project
cmake --build build\debug

REM ================================================================================
REM ================================================================================
REM eof

