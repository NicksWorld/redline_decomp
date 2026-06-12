cmake_minimum_required(VERSION 3.20)

project(redline CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/reccmp.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_compile_options("/MT")

add_executable(redline WIN32
    src/main.cpp
    src/enginestate.cpp
    src/mutex.cpp
    src/log.cpp
    src/pack.cpp
    src/scripts.cpp
    src/file.cpp
    src/keybinds.cpp
    src/config.cpp
    src/replay.cpp
    src/registry.cpp
    src/render.cpp
    thirdparty/3dnow/amddcpu.cpp

    # Game state methods
    src/state/none.cpp
    src/state/debug.cpp
    src/state/av.cpp
    src/state/shutdown.cpp

    res/redline.rc
)
target_link_libraries(redline PRIVATE winmm.lib)
target_include_directories(redline PRIVATE res)

target_link_libraries(redline PRIVATE ddraw.lib dxguid.lib)

target_link_directories(redline PRIVATE thirdparty/smacker)
target_link_libraries(redline PRIVATE Smackw32.lib)
target_include_directories(redline PRIVATE "thirdparty/smacker/include")

reccmp_add_target(redline ID REDLINE)

set_property(TARGET redline PROPERTY OUTPUT_NAME "Redline")
set_property(TARGET redline PROPERTY PREFIX "")
set_property(TARGET redline PROPERTY SUFFIX ".exe")


set(CMAKE_CXX_FLAGS "/W3 /EHsc /GX /D \"WIN32\" /D \"_WINDOWS\"")
set(CMAKE_CXX_FLAGS_DEBUG "/Gm /Zi /Od /D \"_DEBUG\"")
set(CMAKE_CXX_FLAGS_RELEASE "/GX /Od /Ob2 \"NDEBUG\"")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /Ob2 /D \"NDEBUG\"")
set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os /D \"NDEBUG\"")

set(CMAKE_EXE_LINKER_FLAGS "/machine:I386")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/incremental:no")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/incremental:no")

set(CMAKE_STATIC_LINKER_FLAGS "/machine:I386")

set(CMAKE_SHARED_LINKER_FLAGS "/machine:I386")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/incremental:no")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "/incremental:no")

set(CMAKE_JOB_POOL_COMPILE 1)
set(CMAKE_JOB_POOL_LINK 1)

reccmp_configure()
