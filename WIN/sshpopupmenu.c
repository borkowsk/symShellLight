/// Zadanie wymaga u¿ycia kilku kluczowych funkcji WinAPI : `CreatePopupMenu`, `AppendMenu` oraz `TrackPopupMenu`. Poniewa¿ chcesz u¿yæ "czystego" API, musimy pamiêtaæ o obs³udze uchwytu okna(`HWND`), do którego menu bêdzie wysy³aæ komunikaty.
///
/// ### Struktura i Funkcja
///
/// ...
/// 
/// ### Kluczowe detale techniczne :
/// 
/// ***`AppendMenuA`* * : U¿y³em wersji z liter¹ * *A * *(ANSI), poniewa¿ w Twojej strukturze u¿ywasz `const char* `. Jeœli projekt jest skompilowany w UNICODE, jawne u¿ycie wersji ANSI zapobiegnie b³êdom rzutowania.
/// *** `SetForegroundWindow(hwnd)`** : To absolutnie krytyczny "hack" wymagany przez WinAPI.Bez tego menu czêsto nie znika, gdy u¿ytkownik kliknie gdzieœ indziej na pulpicie.
/// *** Identyfikatory(Msg)** : Wartoœci `Msg` przekazane w strukturze trafi¹ do Twojej procedury okienkowej(`WndProc`) jako `LOWORD(wParam)` w komunikacie `WM_COMMAND`.
/// *** Koordynaty(x, y)** : Pamiêtaj, ¿e `TrackPopupMenu` oczekuje wspó³rzêdnych** ekranowych** (Screen Coordinates).Jeœli masz wspó³rzêdne relatywne do okna(Client Coordinates), musisz je najpierw przeliczyæ funkcj¹ `ClientToScreen`.
/// 
/// ### Przyk³ad u¿ycia :
/// 
/// ```Cpp
///     // Wywo³anie np. w reakcji na WM_RBUTTONUP
///     PokazMenuKontekstowe(hwnd, point.x, point.y, mojeMenu, 3);
/// ```
/// 
/// Ten kod jest "lekki", nie wymaga zewnêtrznych bibliotek i idealnie pasuje do starszych projektów utrzymywanych w czystym WinAPI.
/// 
/// #include <windows.h>
/// 
/// <summary>
/// ```
/// MenuItem mojeMenu[] = {
///    {"Otwórz", 101},
///    { "Edytuj", 102 },
///    { "Usuñ", 103 }
/// };
/// ```
/// </summary>
struct MenuItem {
    const char* NazwaPozycji;
    unsigned Msg;
};

void PokazMenuKontekstowe(HWND hwnd, int x, int y, MenuItem* elementy, int liczbaElementow) {
    // 1. Tworzymy pusty kontener menu
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    // 2. Wype³niamy menu elementami z tablicy
    for (int i = 0; i < liczbaElementow; ++i) {
        // U¿ywamy MF_STRING dla zwyk³ego tekstu. 
        // ID elementu to Msg, który zostanie wys³any w komunikacie WM_COMMAND.
        AppendMenuA(hMenu, MF_STRING, elementy[i].Msg, elementy[i].NazwaPozycji);
    }

    // 3. Wyœwietlamy menu. 
    // TPM_RETURNCMD sprawia, ¿e funkcja zwraca ID wybranego elementu zamiast wysy³aæ go do okna.
    // Dziêki temu mo¿esz obs³u¿yæ klikniêcie bezpoœrednio tutaj lub pozwoliæ systemowi wys³aæ WM_COMMAND.

    // Ustawienie okna na foreground jest wymagane przez Windows, aby menu znika³o po klikniêciu obok.
    SetForegroundWindow(hwnd);

    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hwnd, NULL);

    // 4. Sprz¹tamy zasoby
    DestroyMenu(hMenu);
}