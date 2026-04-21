/// @file
/// @brief SymshellLight default implementation of `ssh_context_menu_expected` using rofi (blocking).
//  Created by borkowsk on 16.04.26.
/// @date 2026-04-21 (last modification)

#ifdef __cplusplus
#error This file is only for pure "C" compilation.
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "symshell.h"
//#include "wb_context_menu_default.h"

/**< Z symshx11.c — maska poziomów śledzenia 1-msgs 2-grafika 3-grafika detaliczna 4-alokacje/zwalnianie */
extern int    ssh_trace_level;

/** Szablon komendy do wywołania programu rofi wyświetlające menu kontekstowe.
 *  Kolejność parametrów %s %d oraz %lld musi być zachowana. */
const char* command_tmpl=
"echo '%s' | rofi -monitor \"-1\" -dmenu -p \">\" -theme-str '"
                        "listview {\n"
                                "lines:          %d;\n"
                                "fixed-height:   false;\n"
                                "scrollbar:      false;\n"
                                "}\n"
                        "entry {\n"
                                "placeholder:    \"abcd...\";\n"
                                "}\n"
                        "window {\n"
                                "width:          300px;\n"
                                "border:         2px;\n"
                                "border-radius:  10px;\n"
                                "location: north west;\n"
                                "anchor:   north west;\n"
                                "x-offset:     %lldpx;\n"
                                "y-offset:     %lldpx;\n"
                                "}'\0";

/* BUFORY NA STRINGI KONIECZNE DO URUCHOMIENIA rofi */
/* ================================================ */
char menu_str[1024]; /**< Menu przetworzone na listę dla programu rofi. */
char command[2048]; /**< Ostateczna treść komendy dla popen. */
char answer[1024]; /**< Bufor na odpowiedź z komendy. */

/** Opakowanie dla popen z obsługą błędów wypisywaną na `stderr`.
 * @param command to całość komendy do wywołania.
 * @param out_str to bufor na odpowiedź.
 * @param exp_size to rozmiar bufora na odpowiedź.
 * @return EXIT_SUCCESS jak się udało.
 */
static int run_popen(const char command[],char out_str[],size_t exp_size);

/* Funkcja uruchamiająca kontekstowe menu po kliknięciu prawym klawiszem myszy.
 *
 * DEFINCJE MENU BIERZE Z ZMIENNEJ `context_menu_default`.
 *
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
    if(other_data==NULL)
    {
        fprintf(stderr,"The default context menu launcher requires the other_data structure to be populated.\n");
        errno=EINVAL;
        return -2; //Błąd obsługi!
    }
    else
    {
        if(ssh_trace_level>2)
            fprintf(stderr,"ssh_context_menu_expected(%u,%u):`ssh_basic_win_place_context`:\nD:\tx%llx\nW:\tx%llx\nX:\t%u\nY:\t%u\n\n",
                x,y,
                other_data->ScrIdentifier,
                other_data->WinIdentifier,
                other_data->X,
                other_data->Y
                );
        fflush(stderr);

        *menu_str='\0'; // Początek "stakowania" definicji menu na string-u.
        for(int i=0; i < context_menu_default_size; i++) {
            strcat(menu_str, context_menu_default[i].item_text);
            strcat(menu_str,"\n");
        }                                                                  assert(strlen(menu_str)<sizeof(menu_str));

        sprintf(command, command_tmpl,
                menu_str,
                context_menu_default_size,
                other_data->X,
                other_data->Y);                                              assert(strlen(command)<sizeof(command));

        if(ssh_trace_level>1) fprintf(stderr,"%s\n\n",command);
        int ret=run_popen(command,answer,sizeof(answer));
        if(ssh_trace_level>0) fprintf(stderr,"%s\n\n",answer);

        if(ret==EXIT_SUCCESS) // Gdy wywołanie się powiodło, to trzeba dopasować odpowiedź i wysłać odpowiednią wartość.
        {
            for(int i=0; i < context_menu_default_size; i++) {
                if(strcmp(answer, context_menu_default[i].item_text) == 0)
                    return context_menu_default[i].item_value;
            }

            return 0; //Jak nie znalazł nic w odpowiedzi.
        }
        else return -2;
    }
    // To poniżej już bezużyteczne.
    // return -1; /* OBSŁUGA ZANIECHANA! Poślij dane domyślnej obsłudze `\b` */
}

/* Implementacja zabezpieczonej funkcji popen*/
int run_popen(const char comm_str[],char result_buffer[],size_t RESULT_SIZE)
{
    FILE *fp;

    // 1. Wywołanie popen
    // Czyścimy errno przed wywołaniem, by mieć pewność co do pochodzenia błędu
    errno = 0;
    fp = popen(comm_str, "r");

    if (fp == NULL) {
        fprintf(stderr, "Błąd krytyczny popen: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // 2. Odczyt danych z kontrolą rozmiaru
    // `fgets` jest bezpieczny, bo nie wyjdzie poza RESULT_SIZE
    if (fgets(result_buffer, RESULT_SIZE, fp) != NULL) {
        // Opcjonalnie: usuwanie znaku nowej linii, który fgets zachowuje
        result_buffer[strcspn(result_buffer, "\n")] = '\0';

        //printf("Odebrano dane: [%s]\n", result_buffer);
    } else {
        // Jeśli `fgets` zwrócił NULL, więc sprawdzamy, czy to błąd, czy pusty wynik
        if (ferror(fp)) {
            fprintf(stderr, "Błąd podczas odczytu ze strumienia.\n");
        } else {
            if(ssh_trace_level>1)
                fprintf(stderr,"Wykonanie popen nie zwróciło żadnych danych.\n"); //Co jest możliwe i OK.
        }
    }

    // 3. Zamknięcie strumienia i odczyt statusu zakończenia
    // `pclose` zwraca status procesu, który warto sprawdzić
    int status = pclose(fp);
    if (status == -1) {
        fprintf(stderr, "Błąd podczas zamykania strumienia: %s\n", strerror(errno));
        return EXIT_FAILURE;
    } else {
        // Sprawdzenie, czy proces zakończył się sukcesem (kod 0)
        // Np. "rofi" zwraca 1, gdy nie udziela odpowiedzi (pusty output),
        // bo kliknie się poza jego okna, a ma opcje znikania.
        // To nas nie koniecznie tutaj interesuje, ale kiedyś może.
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            if(ssh_trace_level>0)
                fprintf(stderr, "Wykonanie popen zakończone kodem błędu: %d\n", WEXITSTATUS(status));
            //return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
