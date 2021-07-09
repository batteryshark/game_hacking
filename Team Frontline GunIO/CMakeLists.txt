cmake_minimum_required(VERSION 3.14)
project(TF_Gunio)
set(CMAKE_CXX_STANDARD 17)


add_library(TF_Gunio SHARED GunIOBoard.cpp GunIOBoard.h GunIOInput.cpp GunIOInput.h GunIOOutput.cpp GunIOOutput.h GunIOUtils.cpp GunIOUtils.h)