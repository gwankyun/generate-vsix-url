# cmake版本
cmake_minimum_required(VERSION 3.28) # C++20模塊支持

# 項目名
project(generate_vsix_url LANGUAGES CXX) # 指定C++可以跳過C編譯器檢測

function(check_package name)
  message(CHECK_START "Looking for ${name}")
  if(${name}_FOUND)
    set(ver ${${name}_VERSION})
    message(CHECK_PASS "found version \"${ver}\"")
  else()
    message(CHECK_FAIL "not found")
  endif()
endfunction()

find_package(fmt REQUIRED)
check_package(fmt)
find_package(spdlog REQUIRED)
check_package(spdlog)
find_package(hello-imgui CONFIG REQUIRED)
check_package(hello-imgui)
find_package(nlohmann_json REQUIRED)
check_package(nlohmann_json)
find_package(clip CONFIG REQUIRED) # 剪貼板
check_package(clip)
find_package(boost_algorithm REQUIRED)
check_package(boost_algorithm)
find_package(boost_config REQUIRED)
check_package(boost_config)

add_executable(main)
target_sources(main
  PRIVATE FILE_SET CXX_MODULES FILES
    src/control.ixx
    src/util.ixx
  PRIVATE
    src/util.cpp
    src/control.cpp
    src/main.cpp
)
target_link_libraries(main
  PRIVATE
    fmt::fmt
    spdlog::spdlog
    hello-imgui::hello_imgui
    nlohmann_json::nlohmann_json
    clip::clip
    Boost::algorithm
    Boost::config
)
target_compile_features(main PRIVATE cxx_std_23)

# 僅在Release構建中設置為Windows子系統（隱藏控制臺）
set_target_properties(main PROPERTIES
    WIN32_EXECUTABLE $<CONFIG:Release>  # Release模式：WIN32子系統
)

# 添加鏈接器標志：指定入口點點mainCRTStartup（兼容標準main函數）
target_link_options(main PRIVATE
    $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>  # Release模式生效
)
