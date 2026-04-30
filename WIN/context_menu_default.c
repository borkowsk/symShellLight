/// @file
/// @brief Default context menu definition.
/// @date 2026-04-30 (last modification)
//        ================================+
// Created by borkowsk on 17.04.26.

#ifdef __cplusplus
#error This file is only for pure "C" compilation.
#endif

#include "symshell.h"
//#include "wb_context_menu_default.h"

/* Domyślna definicja menu kontekstowego/Default context menu definition.
 * (file: "context_menu_default.c") */
ssh_menu_item_definition  context_menu_default[]= {
        //{"HELP", 'H'}, //To nigdzie nie jest zaimplementowane.
        {"DUMP", 'D'},
        {"... ", -1 }, //Niech to obsłuży użytkownik biblioteki.
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
