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
    src/ast/redirection.c
    src/ast/builtin_exec.c
    src/lexer/syntax.c
    src/substitution/assignement_variables.c
    )

set(HEADERS
    src/include/global.h
    src/prompt/header/prompt.h
    src/lexer/header/lexer.h
    src/lexer/header/token.h
    src/auxiliary/header/auxiliary.h
    src/include/include.h
    src/ast/header/astconvert.h
    src/ast/header/builtin_exec.h
    src/substitution/header/assignement_variables.h
    )

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -pedantic -std=c99 -g -fsanitize=address")

add_custom_target(doc
                  COMMAND doxygen ../src/dox_config)

find_library(READLINE_LIB readline)

add_executable(42sh ${SRCS} ${HEADERS})

target_link_libraries(42sh "${READLINE_LIB}")
