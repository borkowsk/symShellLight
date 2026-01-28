/** \file   sshmenuf.h
*  \brief   INTERFACE FOR SYMSHELL MENU AND RELATED FEATURES
*  \warning MS WINDOWS ONLY. Otherwise dummy version is provided.
*           Unix version used Motif is outdated.
*  \author  borkowsk
*  \n :atom_symbol:
* ************************************************** */
#ifndef SYMSHELL_MENU_FUNCTIONS_H_INCLUDED
#define SYMSHELL_MENU_FUNCTIONS_H_INCLUDED

#ifdef __cplusplus
#define DEF_VAL_IF_CPP( _P_ )  =(_P_)
#else
#define DEF_VAL_IF_CPP( _P_ )
#endif

/**
 * @defgroup SymShellMenu Interface do obsługi menu
 * \brief    Podstawowe funkcje pozwalające manipulować menu w Windows
 * \details  Wersja dla X11 używająca Motifs kiedyś istniała, ale jest już raczej bezużyteczna.
 *           Jest moduł dummy służący za zaślepkę na tej platformie oraz w SVG.
 *           TODO wersja tekstowa lub html/JavaScript. A może jest coś przenośnego co by można włączyć?
 */
///@{

#ifdef __cplusplus
extern "C" {
#endif

    /** \brief Zmienia tytuł okna */
    int ssh_set_window_name(const char* WindowName);

    /** \brief Typ uchwytu do menu */
    typedef void* ssh_menu_handle;

    /** \brief Uchwyt do głównego menu. \return Cokolwiek zwraca, to raczej 0 jest wartością niepoprawną.  */
    ssh_menu_handle ssh_main_menu();

    /** \brief Uchwyt do PODMENU podanego menu */
    ssh_menu_handle ssh_sub_menu(
					ssh_menu_handle hMenu,				              /**< uchwyt do menu lub sub-menu */
					unsigned    Position				              /**< pozycja składowej menu */
                    );

    /** \brief pozycja itemu o danej nazwie */
    /** \return MAX_UINT if cant find such item */
    unsigned ssh_get_item_position(
                 ssh_menu_handle hMenu,				                 /**< uchwyt do menu lub sub-menu */
                 const char* ItemName				                 /**< nazwa składowej menu */
                 );

    /** \brief dodanie itemu o danej nazwie */
    /** \note  by default item is ENABLE immediately. It could be changed using \p Flags */
    int	ssh_menu_add_item(
					ssh_menu_handle hMenu,				             /**< uchwyt do menu lub sub-menu */
					const char* ItemName,				             /**< nazwa składowej menu */
					unsigned    Message,                             /**< ??? */
					unsigned    Flags                                /**< flagi ustawień */
                                DEF_VAL_IF_CPP(0)
                                );

    /** \brief znakowanie itemu menu */
    /** \param asPosition Need be 1 if submenu checking ??? */
    int ssh_menu_mark_item(
					ssh_menu_handle hMenu,				              /**< uchwyt do menu lub sub-menu */
					unsigned    Check,                                /**< czy z check-markiem */
					unsigned    ItemCommandOrPosition,				  /**< jakiś identyfikator itemu */
					unsigned    asPosition DEF_VAL_IF_CPP(0)
                                );

    /** \brief Usuwa item z menu */
    int ssh_menu_remove_item(
                    ssh_menu_handle hMenu,				             /**< uchwyt do menu lub sub-menu */
                    unsigned    ItemCommandOrPosition,				 /**< jakiś identyfikator itemu */
                    unsigned    asPosition DEF_VAL_IF_CPP(0)
                    );

    /** \brief zapewnia że menu staje się gotowe do użycia */
    int ssh_realize_menu(ssh_menu_handle hMenu);				      /**< \param hMenu - uchwyt do menu lub sub-menu */

#ifdef unix
/** For debuging purposes - external accesible */
extern int 		ssh_menu_trace;
/** MOTIF STYLE OPTIONS */
extern char*    ssh_menu_options;
#endif

#ifdef __cplusplus
};
#endif
///@}

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2022-10-27                   */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif //SYMSHELL_MENU_FUNCTIONS_H_INCLUDED





