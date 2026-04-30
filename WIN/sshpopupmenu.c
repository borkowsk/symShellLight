/// @file
/// @brief Implementacja Popup Menu w Windows.
/// @date 2026-04-30 (created)
/// 
/// Zadanie wymaga użycia kilku kluczowych funkcji WinAPI:
/// `CreatePopupMenu`, `AppendMenu` oraz `TrackPopupMenu`.
///
/// Ponieważ chcesz użyć "czystego" API, musimy pamiętać o obsłudze uchwytu okna(`HWND`),
/// do którego menu będzie wysyłać komunikaty.
///
/// ### Struktura i Funkcja
///
/// ...
/// 
/// ### Kluczowe detale techniczne:
/// 
/// * **AppendMenuA**: Użyłem wersji z literą **A** (ANSI), ponieważ w Twojej strukturze używasz `const char* `.
///     Jeśli projekt jest skompilowany w UNICODE, jawne użycie wersji ANSI zapobiegnie błędom rzutowania.
/// * **SetForegroundWindow(hwnd)**: To absolutnie krytyczny "hack" wymagany przez WinAPI. Bez tego menu często nie znika,
///     gdy użytkownik kliknie gdzieś indziej na pulpicie.
/// * **Identyfikatory(Msg)**: Wartości `Msg` przekazane w strukturze trafią do Twojej procedury okienkowej(`WndProc`) jako
///     `LOWORD(wParam)` w komunikacie `WM_COMMAND`.
/// * **Koordynaty(x, y)**: Pamiętaj, że `TrackPopupMenu` oczekuje współrzędnych **ekranowych** (Screen Coordinates).
///     Jeśli masz współrzędne relatywne do okna (Client Coordinates), musisz je najpierw przeliczyć funkcją
///     `ClientToScreen`.
/// 
/// ### Przykład użycia:
/// 
/// ```Cpp
/// ssh_menu_item_definition mojeMenu[] = {
///    {"Otwórz", 101},
///    {"Edytuj", 102},
///    {"Usuń", 103}
/// };
/// 
/// // Wywołanie np. w reakcji na WM_RBUTTONUP
/// UseContextMenu(hwnd, point.x, point.y, mojeMenu, 3);
/// 
/// ```
/// 
/// Ten kod jest "lekki", nie wymaga zewnętrznych bibliotek i idealnie pasuje do starszych projektów utrzymywanych w czystym WinAPI.
/// 

#include <windows.h>
#include "symshell.h"

/* \brief Structure for defining a simple menu. */
// typedef struct ssh_menu_item_definition {
//    const char*  item_text;  /**< Menu line text. Can also be a label differing in that value is 0. */
//    long long   item_value;  /**< Value passed through `get_char()` function. For labels, 0. */
// } ssh_menu_item_definition;

long UseContextMenu(HWND hwnd, int x, int y, ssh_menu_item_definition* elementy, int liczbaElementow)
{
    // 1. Tworzymy pusty kontener menu
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return -1;

    // 2. Wypełniamy menu elementami z tablicy
    for (int i = 0; i < liczbaElementow; ++i) {
        // Używamy MF_STRING dla zwykłego tekstu. 
        // ID elementu to Msg, który zostanie wysłany w komunikacie WM_COMMAND.
        AppendMenuA(hMenu, MF_STRING, elementy[i].item_value, elementy[i].item_text);
    }

    // Ustawienie okna na foreground jest wymagane przez Windows, aby menu znikało po kliknięciu obok.
    SetForegroundWindow(hwnd);

    // 3. Wyświetlamy menu. 
    // TPM_RETURNCMD sprawia, że funkcja zwraca ID wybranego elementu zamiast wysyłać go do okna.
    // Dzięki temu możesz obsłużyć kliknięcie bezpośrednio tutaj (alternatywnie pozwolić systemowi wysłać WM_COMMAND).
    // Ale z TPM_RETURNCMD jest prościej.
    POINT point = { x,y };
    ClientToScreen(hwnd, &point);
    unsigned ret=TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, 0, hwnd, NULL);

    // 4. Sprzątamy zasoby
    DestroyMenu(hMenu);

    return ret;
}

/* ******************************************************************/
/*                 SYMSHELLLIGHT  version 2026                      */
/* ******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/

