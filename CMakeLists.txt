cmake_minimum_required(VERSION 3.22)
project(XMEngine4)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions("HAVE_BOOLEAN=1")
add_compile_options(-fpermissive -Wno-write-strings)

list(APPEND CMAKE_PREFIX_PATH "./thirdparty/glew-2.1.0")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/freeglut")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/libjpeg")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/freetype6")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(JPEG REQUIRED)
find_package(Freetype REQUIRED)

add_executable(XMEngine4
        directx/ddraw.h
        gamecore/bullets/bullet.cpp
        gamecore/bullets/bullet.h
        gamecore/camera/camera.cpp
        gamecore/camera/camera.h
        gamecore/cartridges/cartridges.cpp
        gamecore/cartridges/cartridges.h
        gamecore/characters/characters.cpp
        gamecore/characters/characters.cpp
        gamecore/collisions/collision.cpp
        gamecore/collisions/collision.h
        gamecore/frustum/frustum.cpp
        gamecore/frustum/frustum.h
        gamecore/hud/hud.cpp
        gamecore/hud/hud.h
        gamecore/skybox/skybox.cpp
        gamecore/skybox/skybox.h
        gamecore/weapons/weapons.cpp
        gamecore/weapons/weapons.h
        gamecore/xmanimation/xmanimation.cpp
        gamecore/xmanimation/xmanimation.h
        gamecore/xmanimation/xmvertexanimation.cpp
        gamecore/xmanimation/xmvertexanimation.cpp
        gamecore/xmdmodel/xmdmodel.cpp
        gamecore/xmdmodel/xmdmodel.h
        gamecore/xmlevel/xmlevel.cpp
        gamecore/xmlevel/xmlevel.h
        gamecore/xmlevel/postprocess.cpp
        gamecore/xmlevel/postprocess.h
        gamecore/xmlevel/shaders.cpp
        gamecore/xmlevel/shaders.h
        gui/xmmenu/xmmenu.cpp
        gui/xmmenu/xmmenu.h
        gui/cryptfuncs.h
        gui/gui.cpp
        gui/gui.h
        gui/networking.cpp
        gui/networking.h
        math/mat3.h
        math/mat4.cpp
        math/mat4.h
        math/quat.h
        math/vector.cpp
        math/vector.h
        network/client.cpp
        network/client.h
        network/fileUpdater.cpp
        network/fileUpdater.h
        window/font.cpp
        window/font.h
        window/ProgressBar.rc
        window/resource.h
        window/shaders.cpp
        window/shaders.h
        window/tex3D.cpp
        window/tex3D.h
        window/textures.cpp
        window/textures.h
        window/timer.cpp
        window/timer.h
        window/updatergui.h
        window/window.cpp
        window/window.h
        main.cpp
        main.h)

target_link_libraries(XMEngine4 JPEG::JPEG ${OPENGL_LIBRARIES} GLUT::GLUT GLEW::GLEW -lgdi32 -lwinmm Freetype::Freetype -lws2_32 -lvfw32 -lcomctl32)
