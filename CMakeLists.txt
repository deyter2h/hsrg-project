cmake_minimum_required(VERSION 3.15)
project(HSRG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Куда класть собранные бинарники
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# Попытаемся найти установленный пакет raylib
find_package(raylib QUIET)

if (NOT raylib_FOUND)
    message(STATUS "Raylib not found — скачиваем и собираем встроенно")

    # Указываем, откуда брать исходники raylib
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        master        # или укажите версию, например v4.0.0
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        FetchContent_Populate(raylib)
        # raylib требует CMake ≥3.8
        add_subdirectory(
            ${raylib_SOURCE_DIR}
            ${raylib_BINARY_DIR}
            EXCLUDE_FROM_ALL
        )
    endif()

    # После add_subdirectory переменная raylib_LIBRARY будет найдена
    set(raylib_FOUND    TRUE)
    set(raylib_LIBRARIES raylib)      # target name
    set(raylib_INCLUDE_DIRS ${raylib_SOURCE_DIR}/src)
endif()

# Все .cpp/.h из папки src автоматически попадут в сборку
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

add_executable(HSRG ${SOURCES} ${HEADERS})

# Инклюд-директории
target_include_directories(HSRG PRIVATE
    src
    ${raylib_INCLUDE_DIRS}
)

# Линкуем Raylib
target_link_libraries(HSRG PRIVATE ${raylib_LIBRARIES})

# Если нужно, можно добавить дефиницию версии из Git-тегов:
find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_VERSION "unknown")
endif()
target_compile_definitions(HSRG PRIVATE GIT_VERSION="${GIT_VERSION}")
