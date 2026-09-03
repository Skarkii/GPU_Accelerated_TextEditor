cmake_minimum_required(VERSION 3.20)
project(gpu_text_editor VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    # Windows: use vendored dependencies

    # GLFW configuration
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)

    # GLM (header-only)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/external/glm)

    # Vulkan headers path (needed by volk)
    set(VULKAN_HEADERS_PATH ${CMAKE_SOURCE_DIR}/external/Vulkan-Headers/include)
    include_directories(${VULKAN_HEADERS_PATH})

    # Volk (Vulkan meta-loader - loads Vulkan at runtime, no need to link vulkan-1.lib)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
    set(VOLK_HEADERS_ONLY OFF)
    set(VOLK_INSTALL OFF)
    add_subdirectory(external/volk)
else()
    # Linux: use system packages via Nix
    find_package(Vulkan REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)
endif()

# Main executable
add_executable(gpu_text_editor
    src/main.cpp
)

target_include_directories(gpu_text_editor PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

if(WIN32)
    target_compile_definitions(gpu_text_editor PRIVATE
        USE_VOLK
        VK_USE_PLATFORM_WIN32_KHR
    )
    target_link_libraries(gpu_text_editor PRIVATE
        volk
        glfw
        glm
    )
else()
    target_link_libraries(gpu_text_editor PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
    )
endif()

# Enable warnings
target_compile_options(gpu_text_editor PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
