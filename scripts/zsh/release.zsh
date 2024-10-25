#!/usr/bin/zsh
# ================================================================================
# ================================================================================
# - File:    build.zsh
# - Purpose: This file contains a script that will build c and c++ software
#            using CMake
#
# Source Metadata
# - Author:  Jonathan A. Webb
# - Date:    February 26, 2022
# - Version: 1.0
# - Copyright: Copyright 2022, Jon Webb Inc.
# ================================================================================
# ================================================================================

cmake -S ../../VulkanTextureDepth/ -B ../../VulkanTextureDepth/build/release/ -DCMAKE_BUILD_TYPE=Release
cmake --build ../../VulkanTextureDepth/build/release/
# ================================================================================
# ================================================================================
# eof
