cmake_minimum_required(VERSION 3.16)
project(SmartLib VERSION 2.0 LANGUAGES CXX)

# ── Estándar C++17 obligatorio ───────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Flags de compilación (Google C++ Style Guide + warnings máximas) ─────────
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# ── Ejecutable principal ─────────────────────────────────────────────────────
add_executable(smartlib
    main.cpp
    # Headers incluidos vía #include (header-only design)
    Persona.h
    Estudiante.h
    Profesor.h
    MaterialBibliotecario.h
    LibroFisico.h
    LibroDigital.h
    Prestamo.h
    Biblioteca.h
)

target_include_directories(smartlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ── Mensaje de configuración ─────────────────────────────────────────────────
message(STATUS "SmartLib v2.0 configurado con C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
