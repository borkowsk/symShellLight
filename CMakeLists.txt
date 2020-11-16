cmake_minimum_required(VERSION 2.8)

project(symshelllight)

include_directories("./")

if(true) # SVG SYMSHELL LIB
add_library( wbsyshSVG STATIC
    "SVG/errorent.c"
    "SVG/symshsvg.cpp"
    "UTILS/sshdefs.c"
    "UTILS/sshutils.cpp"
    "UTILS/sshmenufdummy.c" #zamienia funkcje obslugi menu na puste wywołania
)
endif()

if(true) # only for X11 LIB
add_definitions( -DHAVE_X11 -DUSE_X11 )
add_library( wbsyshX11 STATIC 
    "X11/errorent.c"
    "X11/symshx11.c"
    "X11/wbx11ico.c"
    "UTILS/sshdefs.c"
    "UTILS/sshutils.cpp"
    "UTILS/sshmenufdummy.c" #zamienia funkcje obslugi menu na puste wywołania
)
endif()

if(false) # only for WINDOWS - NOT TESTED IN THIS VERSION (after 2016)
add_library( wbsyshWIN STATIC
    "WIN/errorent.c"
    "WIN/sshmenufwin.c"
    "WIN/symshwin.c"
    "WIN/autor.cpp"
    "UTILS/sshutils.cpp"
    "UTILS/sshmenufdummy.c" #zamienia funkcje obslugi menu na puste wywołania
    )
endif()


## PROGRAMY TESTOWE ##
######################



#KONIEC
