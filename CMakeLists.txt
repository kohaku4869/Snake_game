cmake_minimum_required(VERSION 3.16)   # 3.16 trở lên là ổn, 3.29 cũng được
project(snake_game LANGUAGES CXX)

# Yêu cầu C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tìm SFML (cần cài đúng gói libsfml-dev hoặc mingw-w64-x86_64-sfml)
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

# Thêm executable
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/core/Config.hpp
        src/core/Types.hpp
        src/core/Utils.hpp
        src/core/Input.hpp
        src/Game.cpp
        src/Game.hpp
        src/grid/Grid.cpp
        src/grid/Grid.hpp
        "src/entities/Snake.cpp"
        "src/entities/Snake.hpp"
        "src/entities/Bullet.cpp"
        "src/entities/Bullet.hpp"
        "src/entities/Item.cpp"
        "src/entities/Item.hpp"
        "src/systems/Collision.cpp"
        "src/systems/Collision.hpp"
        src/systems/Spawner.cpp
        src/systems/Spawner.hpp
        src/systems/SoundManager.cpp
        src/systems/SoundManager.hpp
        src/ui/Hud.cpp
        src/ui/Hud.hpp

)

# Thêm include directory cho header files
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link SFML
target_link_libraries(${PROJECT_NAME} PRIVATE
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
)


# (tuỳ chọn) copy folder assets vào build để chạy trực tiếp
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)