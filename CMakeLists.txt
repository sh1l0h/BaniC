cmake_minimum_required(VERSION 3.12)
project(banic LANGUAGES C)

add_executable(${PROJECT_NAME} 
    src/main.c
    src/utils.c
    src/preprocessor.c
    src/io/log.c    
    src/data_structures/hash_map.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)

target_link_libraries(${PROJECT_NAME} PRIVATE m)
