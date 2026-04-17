/// @file
/// @brief SymshellLight default of `WB_context_menu_expected` using rofi (blocking).
//  Created by borkowsk on 16.04.26.
/// @date 2026-04-17 (last modification)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "symshell.h"

/** Domyślna definicja menu kontekstowego. */
struct menu_item_definition {
    const char* item_text; long long   item_value;
} context_menu[]= {
        {"HELP", 'H'},
        {"DUMP", 'D'},
        {"QUIT", 'q'}
};
char menu_str[1024];  /**< Menu przetworzone na listę dla programu wyświetlającego. */

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
                                "placeholder:    \"Szukaj...\";\n"
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
long long WB_context_menu_expected(unsigned x,unsigned y,struct WBProposedContextMenyOtherData* other_data)
{
    if(other_data==NULL)
    {
        fprintf(stderr,"The default context menu launcher requires the other_data structure to be populated.\n");
        errno=EINVAL;
        return -2; //Błąd obsługi!
    }
    else
    {
//        fprintf(stderr,"WB_context_menu_expected:`WBProposedContextMenyOtherData`:\nD:\tx%llx\nW:\tx%llx\nX:\t%u\nY:\t%u\n\n",
//                other_data->ScrIdentifier,
//                other_data->WinIdentifier,
//                other_data->X,
//                other_data->Y
//                );
//        fflush(stderr);

        size_t menu_size=sizeof(context_menu)/sizeof(context_menu[0]);
        *menu_str='\0';
        for(int i=0;i<menu_size;i++) {
            strcat(menu_str, context_menu[i].item_text);
            strcat(menu_str,"\n");
        }                                                                  assert(strlen(menu_str)<sizeof(menu_str));

        //size_t command_len=sizeof(command_tmpl)+1+sizeof(context_menu)+80;
        //char command[command_len]; // TO CHYBA ROZSZERZENIE gcc/clang

        sprintf(command,command_tmpl,
                                menu_str,
                                menu_size,
                                other_data->X,
                other_data->Y);                                              assert(strlen(command)<sizeof(command));

        //fprintf(stderr,"%s\n\n",command);
        int ret=run_popen(command,answer,sizeof(answer));
        fprintf(stderr,"%s\n\n",answer);

        if(ret==EXIT_SUCCESS)
        {
            for(int i=0;i<menu_size;i++) {
                if(strcmp(answer,context_menu[i].item_text)==0)
                    return context_menu[i].item_value;
            }

            return 0; //Jak nie znalazł nic w odpowiedzi.
        }
        else return -2;
    }
    // To poniżej juz bezużyteczne.
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
    // fgets jest bezpieczny, bo nie wyjdzie poza RESULT_SIZE
    if (fgets(result_buffer, 1024, fp) != NULL) {
        // Opcjonalnie: usuwanie znaku nowej linii, który fgets zachowuje
        result_buffer[strcspn(result_buffer, "\n")] = '\0';

        //printf("Odebrano dane: [%s]\n", result_buffer);
    } else {
        // Jeśli fgets zwrócił NULL, więc sprawdzamy, czy to błąd, czy pusty wynik
        if (ferror(fp)) {
            fprintf(stderr, "Błąd podczas odczytu ze strumienia.\n");
        } else {
            //printf("Komenda nie zwróciła żadnych danych.\n"); //Co jest możliwe i OK.
        }
    }

    // 3. Zamknięcie strumienia i odczyt statusu zakończenia
    // pclose zwraca status procesu, który warto sprawdzić
    int status = pclose(fp);
    if (status == -1) {
        fprintf(stderr, "Błąd podczas zamykania strumienia: %s\n", strerror(errno));
        return EXIT_FAILURE;
    } else {
        // Sprawdzenie, czy proces zakończył się sukcesem (kod 0)
        // Np. "rofi" zwraca 1, gdy nie udziela odpowiedzi (pusty output).
        // Np. kliknie się poza jego okna, a ma opcje znikania. To nas nie koniecznie tutaj interesuje, ale kiedyś może.
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            //fprintf(stderr, "Komenda zakończona kodem błędu: %d\n", WEXITSTATUS(status));
            //return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
    return -1;
}
