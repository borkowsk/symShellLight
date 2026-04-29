/** \file   sshmenuf.h
*  \brief INTERFACE FOR SYMSHELL MENU AND RELATED FEATURES.
*  @date 2026-04-29 (last modification)
*  \warning MS WINDOWS ONLY. Otherwise, a dummy version is provided.
*  \note Unix version using Motif is outdated.
*  \note Under X11, a popup menu using _rofi_ is available, but under MS Windows native popup is used.
*  \author  borkowsk
* **************************************************************************************************** */
#ifndef SYMSHELL_MENU_FUNCTIONS_H_INCLUDED_
#define SYMSHELL_MENU_FUNCTIONS_H_INCLUDED_

#ifdef __cplusplus
#define DEF_VAL_IF_CPP( _P_ )  =(_P_)
#else
#define DEF_VAL_IF_CPP( _P_ )
#endif

/**
 * @defgroup SymShellMenu Interface for dynamic Windows menu handling.
 * \brief    Basic functions for manipulating menus in MS Windows.
 * \details  A version for X11 using Motif once existed but is now mostly useless.
 *           A dummy module serves as a placeholder on other platforms and in SVG.
 * TODO: text or html/JavaScript version. Is there any portable alternative to include?
 */
///@{

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Changes the window title. Redundant. */
int ssh_set_window_name(const char* WindowName);

/** \brief Definition of a menu handle. */
typedef void* ssh_menu_handle;

/** \brief Creates a new, empty menu. */
ssh_menu_handle ssh_create_menu();

/** \brief Creates a new, empty popup menu. */
ssh_menu_handle ssh_create_popup_menu();

/** \brief Attaches a menu to the window. */
int ssh_set_menu(ssh_menu_handle hMenu);

/** \brief Appends a new item to the menu. */
int ssh_append_menu(ssh_menu_handle hMenu,                   /**< Handle to the target menu. */
                    unsigned    Flags,                       /**< System flags (e.g., MF_STRING). */
                    unsigned    NewItemIdentifier,           /**< ID returned upon click. */
                    const char* ItemName                     /**< Text displayed in the menu. */
);

/** \brief Inserts a new item at a specific position. */
int ssh_insert_menu(ssh_menu_handle hMenu,                   /**< Menu handle. */
                    unsigned    Position,                    /**< Insertion position. */
                    unsigned    Flags,                       /**< System flags. */
                    unsigned    NewItemIdentifier,           /**< Item ID. */
                    const char* ItemName                     /**< Item text. */
);

/** \brief Deletes an item from the menu. */
int ssh_delete_menu(ssh_menu_handle hMenu,                   /**< Menu handle. */
                    unsigned    ItemCommandOrPosition,       /**< ID or position. */
                    unsigned    asPosition DEF_VAL_IF_CPP(0) /**< Whether to treat ID as position. */
);

/** \brief Enables or disables a menu item. */
int ssh_enable_menu_item(ssh_menu_handle hMenu,              /**< Menu handle. */
                         unsigned    ItemCommandOrPosition,  /**< ID or position. */
                         unsigned    Flags,                  /**< Flags (e.g., MF_GRAYED). */
                         unsigned    asPosition DEF_VAL_IF_CPP(0)
);

/** \brief Checks or unchecks a menu item. */
int ssh_check_menu_item(ssh_menu_handle hMenu,               /**< Menu handle. */
                        unsigned    ItemCommandOrPosition,   /**< ID or position. */
                        unsigned    Flags,                   /**< Flags (e.g., MF_CHECKED). */
                        unsigned    asPosition DEF_VAL_IF_CPP(0)
);

/** \brief Replaces an existing menu item with another. */
int ssh_modify_menu(ssh_menu_handle hMenu,                   /**< Menu handle. */
                    unsigned    ItemCommandOrPosition,       /**< ID or position. */
                    unsigned    Flags,                       /**< System flags. */
                    unsigned    NewItemIdentifier,           /**< New ID or submenu handle. */
                    const char* ItemName,                    /**< New text or data. */
                    unsigned    asPosition DEF_VAL_IF_CPP(0)
);

/** \brief Refreshes the window's menu bar. */
int ssh_draw_menu_bar();

/** \brief Displays a popup menu at a specific location. */
int ssh_track_popup_menu(ssh_menu_handle hMenu,              /**< Handle to the popup menu. */
                         unsigned    Flags,                  /**< Positioning flags. */
                         int x, int y                        /**< Screen coordinates. */
);

/** \brief Destroys the menu and releases resources. */
int ssh_destroy_menu(ssh_menu_handle hMenu);

/** \brief Retrieves a submenu handle from a given position. */
ssh_menu_handle ssh_get_submenu(
        ssh_menu_handle hMenu,                              /**< Parent menu handle. */
        unsigned    ItemCommandOrPosition,                  /**< Item identifier. */
        unsigned    asPosition DEF_VAL_IF_CPP(0)
);

/** \brief Ensures that the menu is ready for use. */
int ssh_realize_menu(ssh_menu_handle hMenu);                /**< \param hMenu - handle to menu or sub-menu. */

#ifdef unix
/** For debugging purposes - externally accessible. */
extern int      ssh_menu_trace;
/** MOTIF STYLE OPTIONS */
extern char*  ssh_menu_options;
#endif

#ifdef __cplusplus
};
#endif

///@}

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2026                         */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif //SYMSHELL_MENU_FUNCTIONS_H_INCLUDED_





