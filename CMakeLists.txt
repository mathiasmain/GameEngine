cmake_minimum_required(VERSION 3.21)

project(NXLL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic -O3)
endif()

# ====== Vulkan SDK (esperado instalado no sistema) ======
# Isso depende da variável de ambiente VULKAN_SDK estar definida
if (WIN32)
    if(NOT DEFINED ENV{VULKAN_SDK})
    #FATAL_ERROR    
        message( "Variável de ambiente VULKAN_SDK não está definida.")
        set(VULKAN_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")
        set(VULKAN_LIB_DIR     "$ENV{VULKAN_SDK}/Lib")
        set(VULKAN_LIB         vulkan-1)    
    else()
        set(VULKAN_INCLUDE_DIR "D:/Programas/Vulkan/Include")
        set(VULKAN_LIB_DIR     "D:/Programas/Vulkan/Lib")
        set(VULKAN_LIB         vulkan-1)
    endif()
endif()

# ====== SDL3 (clonado do GitHub) ======
# Assumimos que você clonou com submodules
set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sdl3")

# Adiciona os includes e o diretório do SDL
add_subdirectory(${SDL3_DIR} EXCLUDE_FROM_ALL)

# ====== Executável do projeto ======
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/S.h
    src/S.cpp
    src/V.h
    src/V.cpp
)

# Adiciona a GLM ao include path
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
)

# Inclui os headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${VULKAN_INCLUDE_DIR}
)

# Linka com Vulkan e SDL3
target_link_directories(${PROJECT_NAME} PRIVATE
    ${VULKAN_LIB_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${VULKAN_LIB}
    SDL3-static  # ou SDL3 se quiser a versão dinâmica
)

# Se necessário, defina isso para o SDL compilar corretamente
target_compile_definitions(${PROJECT_NAME} PRIVATE
    SDL_STATIC=1
)