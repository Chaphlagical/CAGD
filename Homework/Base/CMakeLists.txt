SetTarget(
    MODE STATIC
    TARGET_NAME Base
    DEFINE
        PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/"
        NOMINMAX
    INC
        ${PROJECT_SOURCE_DIR}/Base
        ${PROJECT_SOURCE_DIR}/Base/Eigen
    LIB
        Engine
        Editor
        implot
)