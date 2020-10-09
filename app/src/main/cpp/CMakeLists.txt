set(pathToProject D:/Projects/GitHub/Computer-Components-AR-Android-App)
set(pathToOpenCv D:/Installers/INSTALADORES DE APPS/opencv-3.4.3-android-sdk/OpenCV-android-sdk)

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -fexceptions -Wall")

if (${ANDROID_PLATFORM_LEVEL} LESS 12)
    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 \
                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
    add_definitions("-DDYNAMIC_ES3")
    set(GL3STUB_SRC gl3stub.c)
    set(OPENGL_LIB GLESv2)
else ()
    set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 12)

include_directories(${pathToProject}/app/src/main/jniIncludes)

add_subdirectory(glm)

add_subdirectory(cppLib)

add_library(main_simple SHARED
        ${GL3STUB_SRC}
        main_simple.cpp
        Shader.cpp
        Texture.cpp
        VideoRenderer.cpp
        )

add_library( lib_opencv SHARED IMPORTED )
add_library( libc++ SHARED IMPORTED)

set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${pathToProject}/app/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)
set_target_properties(libc++ PROPERTIES IMPORTED_LOCATION ${pathToProject}/app/src/main/jniLibs/${ANDROID_ABI}/libc++_shared.so)

find_library( log-lib log )

target_link_libraries(main_simple
        ${OPENGL_LIB}
        android
        EGL
        log
        m
        ${log-lib}
        lib_opencv
        libc++
        )