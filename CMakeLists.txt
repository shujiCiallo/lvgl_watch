cmake_minimum_required(VERSION 3.12.4)
project(lvgl_sim C CXX)

# 1. 共享外层LVGL路径配置
set(SHARED_LVGL_DIR "${CMAKE_SOURCE_DIR}/../lvgl")
if(NOT EXISTS ${SHARED_LVGL_DIR}/CMakeLists.txt)
    message(FATAL_ERROR "共享LVGL库不存在,路径:${SHARED_LVGL_DIR}")
endif()
add_subdirectory(${SHARED_LVGL_DIR} lvgl_shared)

# 2. 基础编译标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 3. SDL2 依赖
find_package(SDL2 REQUIRED)
target_include_directories(lvgl PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_definitions(lvgl PUBLIC LV_CONF_PATH="${PROJECT_SOURCE_DIR}/lv_conf.h")

# 4. LVGL 可选功能开关（按需保留/删除）
option(LV_USE_DRAW_SDL "Use SDL draw unit" OFF)
option(LV_USE_LIBPNG "Use libpng to decode PNG" OFF)
option(LV_USE_LIBJPEG_TURBO "Use libjpeg turbo to decode JPEG" OFF)
option(LV_USE_FFMPEG "Use libffmpeg to display video" OFF)
option(LV_USE_FREETYPE "Use freetype font library" OFF)

# 功能开关自动生成编译宏
add_compile_definitions(
    $<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>
    $<$<BOOL:${LV_USE_LIBPNG}>:LV_USE_LIBPNG=1>
    $<$<BOOL:${LV_USE_LIBJPEG_TURBO}>:LV_USE_LIBJPEG_TURBO=1>
    $<$<BOOL:${LV_USE_FFMPEG}>:LV_USE_FFMPEG=1>
)

# 5. 统一收集所有源码：基础文件 + 全局src下所有.c（src/ src/ui/自动包含）
set(MAIN_SOURCES src/mouse_cursor_icon.c)
file(GLOB_RECURSE ALL_SRC_C "${PROJECT_SOURCE_DIR}/src/*.c")
list(APPEND MAIN_SOURCES ${ALL_SRC_C})

# 6. 基础链接库：lvgl内核 + SDL，不需要demo/example/svg可删减
# 最简版本：set(MAIN_LIBS lvgl ${SDL2_LIBRARIES})
set(MAIN_LIBS lvgl lvgl::demos lvgl::examples lvgl::thorvg ${SDL2_LIBRARIES})
# set(MAIN_LIBS lvgl ${SDL2_LIBRARIES})


# Linux/WSL 补充数学库、线程库
if(NOT MSVC)
    list(APPEND MAIN_LIBS m pthread)
endif()

# 7. 扩展第三方库条件链接（不用哪个直接删掉对应块）
if(LV_USE_DRAW_SDL)
    find_package(SDL2_image REQUIRED)
    target_include_directories(lvgl PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${SDL2_IMAGE_LIBRARIES})
endif()

if(LV_USE_LIBPNG)
    find_package(PNG REQUIRED)
    target_include_directories(lvgl PUBLIC ${PNG_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${PNG_LIBRARIES})
endif()

if(LV_USE_LIBJPEG_TURBO)
    find_package(JPEG REQUIRED)
    target_include_directories(lvgl PUBLIC ${JPEG_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${JPEG_LIBRARIES})
endif()

if(LV_USE_FFMPEG)
    list(APPEND MAIN_LIBS avformat avcodec avutil swscale z)
endif()

if(LV_USE_FREETYPE)
    find_package(Freetype REQUIRED)
    target_include_directories(lvgl PUBLIC ${FREETYPE_INCLUDE_DIRS})
    list(APPEND MAIN_LIBS ${FREETYPE_LIBRARIES})
endif()

# 8. LVGL Pro UI工程（不用LVGL Pro直接删除这段）
# set(LVGL_PRO_PROJECT_DIR "" CACHE PATH "Path to LVGL Pro Project folder")
# if(LVGL_PRO_PROJECT_DIR AND EXISTS "${LVGL_PRO_PROJECT_DIR}/CMakeLists.txt")
#     add_subdirectory(${LVGL_PRO_PROJECT_DIR} ui)
#     list(APPEND MAIN_LIBS lib-ui)
#     target_compile_definitions(lib-ui PUBLIC LV_LVGL_H_INCLUDE_SIMPLE)
#     target_include_directories(lib-ui PUBLIC ${LVGL_PRO_PROJECT_DIR} ${CMAKE_SOURCE_DIR}/../lvgl)
#     message(STATUS "LVGL Pro UI已加载: ${LVGL_PRO_PROJECT_DIR}")
# elseif(LVGL_PRO_PROJECT_DIR)
#     message(WARNING "LVGL Pro路径无效: ${LVGL_PRO_PROJECT_DIR}")
# endif()

# 9. 生成主程序main
add_executable(main ${MAIN_SOURCES})
target_compile_definitions(main PRIVATE LV_CONF_INCLUDE_SIMPLE)
target_include_directories(main PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/ui
)
target_link_libraries(main ${MAIN_LIBS})

# Windows强制弹出控制台窗口看日志
if(WIN32)
    if(MSVC)
        target_link_options(main PRIVATE "/SUBSYSTEM:CONSOLE")
    else()
        target_link_options(main PRIVATE "-mconsole")
    endif()
endif()

# 一键运行命令：cmake --build build && cmake --build build --target run
add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)

# 10. Debug模式严格警告（开发用，不需要可整块删除）
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     message(STATUS "Debug模式开启")
#     target_compile_options(lvgl PRIVATE
#         -Wall -Wextra -Wundef -Wuninitialized -Wreturn-type -Wshadow
#         -Wpointer-arith -Wformat-security -Wdouble-promotion
#         -Wno-pedantic
#     )
#     # ASAN内存检测，不用注释/删除
#     option(ASAN "Enable AddressSanitizer" OFF)
#     if(ASAN)
#         message(STATUS "ASAN内存泄漏检测开启")
#         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#         set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
#     endif()
# endif()