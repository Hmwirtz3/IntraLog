cmake_minimum_required(VERSION 3.11)

project(IntraLog)

# Collect all source files under src/ (C and C++).
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Build the app from sample/main.cpp + everything in src/
add_executable(${PROJECT_NAME}
    sample/main.cpp
    src/intraFont.c
    src/libccc.c
    src/intraLog.cpp
    sample/test.cpp
    
    ${SRC_FILES}
)

# Headers live in include/ (and possibly some headers in src/)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# PSP GU libs
target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspge
    pspdisplay
)

# Create EBOOT.PBP
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)
