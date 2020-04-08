cmake_minimum_required(VERSION 3.15)
project(mywork C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(mywork
        bits.c
        bits.h
        bsig.c
        bsig.h
        defs.h
        hash.c
        hash.h
        page.c
        page.h
        psig.c
        psig.h
        query.c
        query.h
        reln.c
        reln.h
        tsig.c
        tsig.h
        tuple.c
        tuple.h
        util.c
        util.h
        x1.c
        )
