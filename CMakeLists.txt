cmake_minimum_required(VERSION 3.27)
project(Final_Project)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(Resource)

file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(Final_Project

    Engine/Allegro5Exception.cpp
    Engine/Allegro5Exception.hpp
    Engine/AudioHelper.cpp
    Engine/AudioHelper.hpp
    Engine/Collider.cpp
    Engine/Collider.hpp
    Engine/GameEngine.cpp
    Engine/GameEngine.hpp
    Engine/Group.cpp
    Engine/Group.hpp
    Engine/IControl.cpp
    Engine/IControl.hpp
    Engine/IObject.cpp
    Engine/IObject.hpp
    Engine/IScene.cpp
    Engine/IScene.hpp
    Engine/LOG.cpp
    Engine/LOG.hpp
    Engine/Point.cpp
    Engine/Point.hpp
    Engine/Resources.cpp
    Engine/Resources.hpp
    Engine/Sprite.cpp
    Engine/Sprite.hpp

    Object/Tank.cpp
    Object/Tank.hpp
    Object/AITank.cpp
    Object/AITank.hpp
    Object/Bullet.cpp
    Object/Bullet.hpp
    Object/AIStrategy.cpp
    Object/AIStrategy.hpp
    Object/Explosion.cpp
    Object/Explosion.hpp
    Object/LandMine.cpp
    Object/LandMine.hpp
    Object/Coin.cpp
    Object/Coin.hpp
    
    Scene/PlayScene.cpp
    Scene/PlayScene.hpp
    Scene/HomeScene.cpp
    Scene/HomeScene.hpp
    Scene/StageSelectScene.cpp
    Scene/StageSelectScene.hpp
    Scene/HistoryScene.cpp
    Scene/HistoryScene.hpp

    UI/Component/Image.cpp
    UI/Component/Image.hpp
    UI/Component/ImageButton.cpp
    UI/Component/ImageButton.hpp
    UI/Component/Label.cpp
    UI/Component/Label.hpp
    UI/Component/Slider.cpp
    UI/Component/Slider.hpp
    
    main.cpp
)

IF (WIN32)
    target_include_directories(Final_Project PUBLIC C:/allegro/include)
    target_link_directories(Final_Project PUBLIC C:/allegro/lib)
    target_compile_options(Final_Project PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(Final_Project liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(Final_Project PUBLIC lib)
    target_include_directories(Final_Project PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(Final_Project PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(Final_Project ${ALLEGRO5_LIBRARIES})
    target_link_directories(Final_Project PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()