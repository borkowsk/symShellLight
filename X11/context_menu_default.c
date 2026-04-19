/// @file
/// @brief Default context menu definition.
// Created by borkowsk on 17.04.26.
/// @date 2026-04-19 (last modification)

#include "symshell.h"
//#include "wb_context_menu_default.h"

/** Domyślna definicja menu kontekstowego. */
ssh_menu_item_definition  context_menu_default[]= {
        {"HELP", 'H'}, //To nigdzie nie jest zaimplementowane.
        {"DUMP", 'D'},
        {"QUIT", 'q'}
    };

unsigned context_menu_default_size= sizeof(context_menu_default) / sizeof(context_menu_default[0]);

/*v******************************************************************/
/*              SYMSHELLLIGHT version 2026-04...                    */
/*v******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/*v******************************************************************/
