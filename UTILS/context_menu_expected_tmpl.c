/// @file
/// @brief Simple demo of `ssh_context_menu_expected` user defined function.
//  Created by borkowsk on 16.04.26.
/// @date 2026-04-19 (last modification)
#include <stdio.h>
#include "symshell.h"

/* Funkcja uruchamiająca kontekstowe menu po kliknięciu prawym klawiszem myszy.
 * @param x — współrzędna pozioma kursora myszy.
 * @param y — współrzędna pionowa kursora myszy.
 * @param other_data - wskaźnik do rekordu danych użytkownika zawierającego co najmniej uchwyt Display i uchwyt okna.
 * @return 0, gdy menu nic nie zwróciło albo oczekujemy, że wynik wróci później jako message.
 *        -1, gdy funkcja zaniechała obsługi i kliknięcie ma być przekazane normalnej obsłudze w aplikacji (przez `\b`).
 *         Każda wartość dodatnia jest traktowana jako komunikat do zwrócenia przez funkcję `get_char()`.
 *         Wartość ujemna powoduje wyświetlenie informacji o błędzie, ze sprawdzeniem wartości zmiennej `errno`.
 * @details Funkcja może być blokująca lub nieblokująca (np. odpalać osobny wątek). Podstawową implementację dostarcza
 *          biblioteka SYMSHELL, ale zdefiniowanie własnej przez użytkownika biblioteki blokuje linkowanie wersji domyślnej.
 */
long long ssh_context_menu_expected(unsigned x, unsigned y, struct ssh_basic_win_place_context* other_data)
{
    fprintf(stderr,"Dummy `ssh_context_menu_expected(%d,%d)` is called...\n",x,y);
    if(other_data!=NULL)
    {
        fprintf(stderr,"Dummy `ssh_basic_win_place_context`:\nD:\t%lld\nW:\t%lld\nX:\t%u\nY:\t%u\n",
                other_data->ScrIdentifier,
                other_data->WinIdentifier,
                other_data->X,
                other_data->Y
                );
        fflush(stderr);
    }

    return -1; /* OBSŁUGA ZANIECHANA! Poślij dane domyślnej obsłudze `\b` */
}
