cmake_minimum_required (VERSION 3.8)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

project ("WebNewtypeDownloader")

# find the Qt5::Core Qt5::Network fmt
find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable (WebNewtypeDownloader "WebNewTypeDownloader.cpp")

target_link_libraries (WebNewtypeDownloader Qt5::Core Qt5::Network fmt::fmt)

