cmake_minimum_required(VERSION 3.11)

project(hello)

add_executable(${PROJECT_NAME}
    # test.c    
    main.c
    gu.c
    ecs.c
    draw.c
    entities.c
    memory.c
    sparse.c
    ppsspp_emu_api.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspdebug
    pspdisplay
    pspctrl
    pspge
    pspgu
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)
