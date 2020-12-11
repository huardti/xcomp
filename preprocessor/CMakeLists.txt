project(xpp)

include_directories(.)
include_directories(../tools)

add_executable(xpp
    main.cpp
    preprocessor.cpp
    file_manager.cpp
    ../tools/lexer.cpp
)