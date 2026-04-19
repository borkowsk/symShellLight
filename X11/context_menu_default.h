/// @file
/// @brief Default context menu declarations.
// Created by borkowsk on 17.04.26.
/// @date 2026-04-19 (last modification)

#ifndef SYMSHELLLIGHT_WB_CONTEXT_MENU_DEFAULT_H
#define SYMSHELLLIGHT_WB_CONTEXT_MENU_DEFAULT_H

#include "symshell.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Domyślna definicja menu kontekstowego. W X11 dostarczana z biblioteki, ale można ją podmienić na poziomie linkowania.*/
extern ssh_menu_item_definition  context_menu_default[];
/** Liczba itemów w domyślnym menu kontekstowym. Musi towarzyszyć `context_menu_default`. */
extern unsigned context_menu_default_size;

#ifdef __cplusplus
} //extern C
#endif

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
#endif //SYMSHELLLIGHT_WB_CONTEXT_MENU_DEFAULT_H
