SetTarget(
    MODE STATIC
    TARGET_NAME Homework8
    DEFINE
        PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/"
        NOMINMAX
    INC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/vendor
    LIB
        Base
        Renderer
        Scene
)