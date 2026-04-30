/// @file
/// @brief Linux handling of context menu.
/// @date 2026-04-30 (last modification)
//        ===============================
// Created by borkowsk on 17.04.26.
#ifndef SYMSHELLLIGHT_WB_CONTEXT_MENU_DEFAULT_H
#define SYMSHELLLIGHT_WB_CONTEXT_MENU_DEFAULT_H

#include "symshell.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Domyślna definicja menu kontekstowego. Dostarczana z biblioteki, ale można ją podmienić na poziomie linkowania.*/
//extern ssh_menu_item_definition  context_menu_default[];
/* Liczba itemów w domyślnym menu kontekstowym. Musi towarzyszyć `context_menu_default`. */
//extern unsigned context_menu_default_size;

#ifdef USE_ENGLISH_IF_POSSIBLE
/** \brief Structure for passing absolute click position and other data to trigger a context menu. */
typedef struct ssh_basic_win_place_context {
    unsigned long long ScrIdentifier; /**< Display system identification data. E.g., Display handle in X11. */
    unsigned long long WinIdentifier; /**< Calling window identification data. E.g., Window handle in X11. */
    unsigned X; /**< Absolute horizontal `x` cursor position in display layout or -1 if cannot be calculated. */
    unsigned Y; /**< Absolute vertical `y` cursor position in display layout or -1 if cannot be calculated. */
} ssh_basic_win_place_context;
#else
/** \brief Struktura do przekazywania absolutnego położenia kliknięcia i innych danych do uruchomienia menu kontekstowego. */
typedef struct ssh_basic_win_place_context {
    unsigned long long ScrIdentifier; /**< Dane identyfikacji systemu wyświetlania. Np. Display handle w X11 */
    unsigned long long WinIdentifier; /**< Dane identyfikacji wywołującego okna. Np. Window handle w X11 */
    unsigned X; /**< Bezwzględne położenie `x` kursora w układzie wyświetlacza albo -1, gdy nie można obliczyć. */
    unsigned Y; /**< Bezwzględne położenie `y` kursora w układzie wyświetlacza albo -1, gdy nie można obliczyć. */
} ssh_basic_win_place_context;
#endif

#ifdef USE_ENGLISH_IF_POSSIBLE
/** @brief Function triggering a context menu after right-clicking.
 * @details Called from the library, from the event loop. The library user can propose their own version,
 *          and the default version is located in the appropriate library source directory,
 *          e.g., "X11/wb_context_menu_expected_rofi.c"
 * @param x - horizontal coordinate of the mouse cursor.
 * @param y - vertical coordinate of the mouse cursor.
 * @param other_data - pointer to user data record containing at least Display handle and window handle.
 * @return 0 when a menu returned nothing or we expect a result later as a message.
 *  -1 when function declined handling and click should be passed to normal application handling (via `\b`).
 *  Any positive value is treated as a message to be returned by the `get_char` function.
 *  Any other negative value causes error info display, checking the `errno` variable value.
 * @details Function can be blocking or non-blocking (e.g., fire a separate thread). Basic implementation
 *  is provided by the SYMSHELL library, but defining one's own by the library user blocks linking the default version.
 */
#else
/** \brief Funkcja uruchamiająca kontekstowe menu po kliknięciu prawym klawiszem myszy.
 * \details Wywoływana z biblioteki, z pętli zdarzeń. Użytkownik biblioteki może zaproponować swoją wersję, a wersja
 *          domyślna znajduje się w odpowiednim katalogu źródłowym biblioteki, np. "X11/wb_context_menu_expected_rofi.c"
 * @param x - współrzędna pozioma kursora myszy w układzie obszaru roboczego okna X11.
 * @param y - współrzędna pionowa kursora myszy w układzie obszaru roboczego okna X11.
 * @param other_data - wskaźnik do rekordu danych użytkownika zawierającego co najmniej uchwyt Display i uchwyt okna.
 * @return 0 gdy menu nic nie zwróciło albo oczekujemy, że wynik wróci później jako message.
 *        -1 gdy funkcja zaniechała obsługi i kliknięcie ma być przekazane normalnej obsłudze w aplikacji (przez `\b`).
 *         Każda wartość dodatnia jest traktowana jako komunikat do zwrócenia przez funkcję `get_char`.
 *         Inna wartość ujemna powoduje wyświetlenie informacji o błędzie, ze sprawdzeniem wartości zmiennej `errno`.
 * @details Funkcja może być blokująca lub nieblokująca (np. odpalać osobny wątek). Podstawową implementację dostarcza
 *          biblioteka SYMSHELL, ale zdefiniowanie własnej przez użytkownika biblioteki blokuje linkowanie wersji domyślnej.
 */
#endif
extern long long ssh_context_menu_expected(unsigned x, unsigned y, struct ssh_basic_win_place_context* other_data);

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
