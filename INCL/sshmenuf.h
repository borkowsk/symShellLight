/** \file   sshmenuf.h
*  \brief INTERFEJS DLA MENU SYMSHELL I POWIĄZANYCH FUNKCJI.
*  @date 2026-04-29 (ostatnia modyfikacja)
*  \warning TYLKO DLA MS WINDOWS. W innym przypadku dostarczana jest wersja pusta (zaślepka).
*  \note Wersja Unix używająca Motif jest przestarzała.
*  \note Pod X11 dostępne jest menu podręczne (popup) używające _rofi_, a pod Windows natywne.
*  \author  borkowsk
* ********************************************************************************************* */
#ifndef SYMSHELL_MENU_FUNCTIONS_H_INCLUDED_
#define SYMSHELL_MENU_FUNCTIONS_H_INCLUDED_

#ifdef __cplusplus
#define DEF_VAL_IF_CPP( _P_ )  =(_P_)
#else
#define DEF_VAL_IF_CPP( _P_ )
#endif

/**
 * @defgroup SymShellMenu Interfejs do dynamicznej obsługi menu Windows.
 * \brief    Podstawowe funkcje pozwalające manipulować menu w MS Windows.
 * \details  Wersja dla X11 używająca Motif kiedyś istniała, ale jest już raczej bezużyteczna.
 *           Istnieje moduł "dummy" służący za zaślepkę na innych platformach oraz w SVG.
 * TODO: wersja tekstowa lub html/JavaScript. A może istnieje coś przenośnego, co można by włączyć?
 */
///@{

#ifdef __cplusplus
extern "C" {
#endif

    /** \brief Zmienia tytuł okna. */
    int ssh_set_window_name(const char* WindowName);

    /** \brief Typ uchwytu do menu. */
    typedef void* ssh_menu_handle;

    /** \brief Uchwyt do głównego menu. \return Cokolwiek zwraca, to raczej 0 jest wartością niepoprawną.  */
    ssh_menu_handle ssh_main_menu();

    /** \brief Uchwyt do PODMENU podanego menu */
    ssh_menu_handle ssh_sub_menu(
                    ssh_menu_handle hMenu,					 /**< Uchwyt do menu lub sub-menu. */
                    unsigned    Position					 /**< Pozycja składowej menu. */
                    );

    /** \brief pozycja itemu o danej nazwie. */
    /** \return MAX_UINT if cant find such item. */
    unsigned ssh_get_item_position(
                 ssh_menu_handle hMenu,						 /**< Uchwyt do menu lub sub-menu. */
                 const char* ItemName						 /**< Nazwa składowej menu. */
                 );

    /** \brief dodanie itemu o danej nazwie. */
    /** \note  by default item is ENABLE immediately. It could be changed using `Flags`. */
    int	ssh_menu_add_item(
                    ssh_menu_handle hMenu,					 /**< Uchwyt do menu lub sub-menu. */
                    const char* ItemName,					 /**< Nazwa składowej menu. */
                    unsigned    Message,			 		 /**< ??? */
                    unsigned    Flags			 			 /**< Flagi ustawień. */
                                DEF_VAL_IF_CPP(0)
                                );

    /** \brief znakowanie itemu menu. */
    /** \param asPosition Need be 1 if submenu checking ??? */
    int ssh_menu_mark_item(
                    ssh_menu_handle hMenu,					 /**< Uchwyt do menu/sub-menu, na którym przeprowadzamy operację. */
                    unsigned    Check,			 			 /**< Informuje, czy z check-markiem. */
                    unsigned    ItemCommandOrPosition,		 /**< Jakiś identyfikator itemu. */
                    unsigned    asPosition DEF_VAL_IF_CPP(0)
                                );

    /** \brief Usuwa item z menu. */
    int ssh_menu_remove_item(
                    ssh_menu_handle hMenu,					 /**< Uchwyt do menu/sub-menu, na którym przeprowadzamy operację. */
                    unsigned    ItemCommandOrPosition,		 /**< Jakiś identyfikator itemu. */
                    unsigned    asPosition DEF_VAL_IF_CPP(0)
                    );

    /** \brief zapewnia, że menu staje się gotowe do użycia. */
    int ssh_realize_menu(ssh_menu_handle hMenu);			 /**< \param hMenu - uchwyt do menu lub sub-menu. */

#ifdef unix
/** Do celów debugowania — dostępne zewnętrznie. */
extern int      ssh_menu_trace;
/** OPCJE W STYLU MOTIF */
extern char* ssh_menu_options;
#endif

#ifdef __cplusplus
};
#endif

///@}

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2026                         */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif //SYMSHELL_MENU_FUNCTIONS_H_INCLUDED_





