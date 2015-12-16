project(dyninst2llvm)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
include_directories("/home/chertio/research/DynInst/dyninst_install/include")
link_directories("/home/chertio/research/DynInst/dyninst_install/lib")
subdirs(src)

