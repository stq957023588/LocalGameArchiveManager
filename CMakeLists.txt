cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(GameArchiveManager LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/Users/95702/AppData/Local/Qt/6.8.0/mingw_64") # Qt Kit Dir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 COMPONENTS Widgets REQUIRED Sql Network) # Qt COMPONENTS

# 添加 Qt6 资源
qt_add_resources(${PROJECT_NAME} resources.qrc)

# 指定 yaml-cpp 的头文件路径和库路径
include_directories(${CMAKE_SOURCE_DIR}/include) # 包含头文件
link_directories(${CMAKE_SOURCE_DIR}/libs)       # 链接库文件


aux_source_directory(./src srcs)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(${PROJECT_NAME}
    # WIN32 # If you need a terminal for debug, please comment this statement 
    ${srcs} 
) 
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets 
    Qt6::Sql 
    Qt6::Network
    ${CMAKE_SOURCE_DIR}/libs/libzip.dll.a
    # ${CMAKE_SOURCE_DIR}/libs/libyaml-cpp.a
    # ${CMAKE_SOURCE_DIR}/libs/libminizip.a
    # ${CMAKE_SOURCE_DIR}/libs/libzlibstatic.a
) # Qt6 Shared Library


# 查找所有 DLL 文件
file(GLOB DLL_FILES "${CMAKE_SOURCE_DIR}/dll/*.dll")

# 使用 foreach 循环复制所有 DLL 文件到 EXE 所在的目录
foreach(DLL_FILE ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILE}  # 当前 DLL 文件
        $<TARGET_FILE_DIR:${PROJECT_NAME}>  # EXE 文件所在的目录
    )
endforeach()