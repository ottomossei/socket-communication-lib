cmake_minimum_required(VERSION 3.1)
project(socket-communication-lib VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# デフォルトのビルドタイプをDebugに設定（ただしユーザーが手動で指定した場合はそれを優先）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug Release" FORCE)
endif()

# ライブラリのソースコード指定
add_library(socketcomm SHARED
    src/server.cpp
)

# ライブラリの出力名
set_target_properties(socketcomm PROPERTIES OUTPUT_NAME "socketcomm")

# インクルードディレクトリを指定
target_include_directories(socketcomm PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# スレッドライブラリのリンク
find_package(Threads REQUIRED)
target_link_libraries(socketcomm PUBLIC Threads::Threads)

# ライブラリのインストール設定
include(GNUInstallDirs)
install(TARGETS socketcomm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# example
add_executable(server_example example/main.cpp)
target_link_libraries(server_example PRIVATE socketcomm)

# デフォルトではexampleをビルドせず、allターゲットでビルド
add_custom_target(example ALL DEPENDS server_example)
