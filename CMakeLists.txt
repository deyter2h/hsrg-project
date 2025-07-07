cmake_minimum_required(VERSION 3.15)
project(HSRG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# ----------------------
# 1) Заранее объявляем FetchContent
# ----------------------
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        master        # можно фиксировать версию, например v4.7.0
  UPDATE_DISCONNECTED TRUE     # не пытаться каждый раз проверять удалённый репозиторий
)

# ----------------------
# 2) И только здесь делаем FetchContent_MakeAvailable
#    Это работает как: если райл не установлен — склонируем и подключим
# ----------------------
find_package(raylib QUIET)
if (NOT raylib_FOUND)
  message(STATUS "raylib not found; fetching it with FetchContent")
  FetchContent_MakeAvailable(raylib)
else()
  message(STATUS "Found system raylib; will use it")
endif()

# В этом моменте у вас точно есть target raylib и include dirs
# Можно получить include через:
if (raylib_FOUND AND TARGET raylib)
  get_target_property(_rayInc raylib INTERFACE_INCLUDE_DIRECTORIES)
else()
  # если он не там, что мы загрузили, 
  # указать дефолтный путь внутри исходников
  set(_rayInc ${raylib_SOURCE_DIR}/src)
endif()

# ----------------------
# 3) Собираем ваш exe
# ----------------------
file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE HDR src/*.h)

add_executable(HSRG ${SRC} ${HDR})
target_include_directories(HSRG PRIVATE
  src
  ${_rayInc}
)
target_link_libraries(HSRG PRIVATE raylib)

# (опционально) Git version
find_package(Git QUIET)
if (GIT_FOUND)
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
