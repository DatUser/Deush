cmake_minimum_required(VERSION 3.0)

project(42sh)

set(SRCS
    src/prompt/prompt.c
    src/lexer/lexer.c
    src/lexer/token.c
    src/auxiliary/cut.c
    src/ast/stringutils.c
    src/ast/parser.c
    src/ast/astconvert.c
    src/ast/ast-evaluation.c
    )

set(HEADERS
    src/prompt/header/prompt.h
    src/lexer/header/token.h
    src/auxiliary/header/auxiliary.h
    src/include/include.h
    src/ast/header/astconvert.h
    )

set(CMAKE_C_FLAGS "-Wall -Werror -pedantic -std=c99 -g")

find_library(READLINE_LIB readline)

add_executable(42sh ${SRCS} ${HEADERS})

target_link_libraries(42sh "${READLINE_LIB}")
