cmake_minimum_required (VERSION 3.8)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

project ("WebNewtypeDownloader")

find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(fmt CONFIG REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable WebNewtypeDownloader "WebNewTypeDownloader.cpp")

target_link_librariesWebNewtypeDownloader Qt5::Core Qt5::Network fmt::fmt)

