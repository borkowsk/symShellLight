/// @file
/// @brief Test linkowania podprogramów SYMSHELL'a.
/// @details Linking test for:
/// SYMSHELL.H - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++
///*******************************************************************
/// designed by W.Borkowski: https://github.com/borkowsk
/// - Last changed massively: 22.06.2007
/// - Comments edition:       19.11.2021
/// - Small changes:          06.02.2023
/// @date 2026-02-02 (last update)

#include "symshell.h"

#define Z_RYSOWANIEM (1)

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */
void test()
{
    unsigned char r=100,g=100,b=250;

    /* inicjacja grafiki */
    shell_setup("Just a linking test",0,nullptr); /* Przekazanie parametrów wywołania */
    buffering_setup(1);			         /* Przełączanie buforowania okna. Może nie działać po inicjacji*/
    fix_size(0);					     /* Czy symulować niezmienność rozmiarów okna, nawet gdy zmiana możliwa? */
    set_background(256+128);		     /* Ustala index koloru do czyszczenia okna. Może nie działać po inicjacji*/
    mouse_activity(1);		             /* Ustala czy mysz ma byc obsługiwana. Zwraca poprzedni stan flagi */
    init_plot(400, 400, 1, 1);           /* inicjacja grafiki (kiedyś też semigrafiki) */
    set_title("LINK TEST FOR SYMSHELL"); /* Ustawienie tytułu okna. Może nie być możliwe na niektórych platformach. */

    /* Operacje przestawiania własności pracy okna graficznego */
    print_transparently(1);	             /* Włącza drukowanie tekstu bez zamazywania tła. Zwraca stan poprzedni */
    line_width(2);		                 /* Ustala szerokość linii. Może być trochę kosztowne. Zwraca stan poprzedni */
    line_style(SSH_LINE_SOLID);            /* Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
    put_style(SSH_SOLID_PUT);              /* Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */
    
    set_rgb(1, 10, 100, 200);       /* Zmienia definicje koloru w palecie kolorów. Indeksy 0..255 */
    set_gray(511, 250);            /* Zmienia definicje odcienia szarości w palecie szarości. Indeksy 256..511 */
    
    set_pen(100, 1, SSH_SOLID_PUT); /* Ustala aktualny kolor linii za pomocą typu ssh_color */
    set_brush(100); /* Ustala aktualny kolor wypełnień za pomocą typu ssh_color */
    set_pen_rgb(r, g, b, 2, SSH_SOLID_PUT); /* Ustala aktualny kolor linii za pomocą składowych RGB */
    set_brush_rgb(r, g, b); /* Ustala aktualny kolor wypełnień za pomocą składowych RGB */

    set_pen_rgba(200, g, b, 64, 2, SSH_SOLID_PUT); /* Ustala aktualny kolor linii za pomocą składowych RGBA(-lpha) */
    set_brush_rgba(150, g, b, 64 ); /* Ustala aktualny kolor wypełnień za pomocą składowych RGBA(-lpha) */
               

    /* ODCZYTYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO*/
    int ret=buffered();				     /* Zwraca 1, jeśli buforowane */
    ret=fixed();					     /* Czy okno ma zafiksowana wielkość */
    ssh_color bcg=background();		     /* Aktualny kolor tla... */
    ret=get_line_width();				 /* Aktualna grubość linii */
    ssh_color pn=get_pen();			     /* Aktualny kolor linii jako ssh_color */
    ssh_color br=get_brush();			 /* Aktualny kolor wypełnień jako ssh_color */
    ret=screen_height();  				 /* Aktualne rozmiary okna po przeliczeniach z init_plot*/
    ret=screen_width();   				 /*  ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora */
    ret=char_height('X');			     /* Aktualne rozmiary znaku  */
    ret=char_width('X');		         /* ...potrzebne do pozycjonowania tekstu */
    ret=string_height("Blablably");	     /* Aktualne rozmiary łańcucha (string'u) */
    ret=string_width("Blablably");	     /* ...potrzebne do jego pozycjonowania */

#ifdef Z_RYSOWANIEM
    /* TE FUNKCJE RACZEJ ZAWSZE SĄ ZAIMPLEMENTOWANE BO TO PODSTAWA */
    /* PUNKTOWANIE  */
    plot(100,150, 254 );		        /* Wyświetlenie punktu na ekranie w kolorze indeksowanym c=254 */
    plot_rgb(100,152,200,100,250);

    /* MALOWANIE */
    fill_circle_d(100,300,100);	        /* Wyświetlenie kola w kolorach domyślnych*/
    fill_rect_d(300, 300, 350, 320);       /* Wyświetla prostokąt w kolorach domyślnych*/
    static const ssh_point points[] = { { 10,0 },{0,-10},{-10,0},{0,10} };
    fill_poly_d(250, 250, points, 4);      /* Wypełnia wielokąt przesunięty o vx,vy w kolorach domyślnych */

    fill_circle(100, 300, 10,254);		/* Wyświetlenie kola w kolorze indeksowanym c=254 */
    fill_rect(350,320,380,350,253);		/* Wyświetla prostokąt w kolorze c=253 */
    fill_poly(300, 250, points, 4, 128);	/* Wypełnia wielokąt przesunięty o vx,vy w kolorze c */

    /* RYSOWANIE  */
    line_d(0, 20, 500, 500);            /* Wyświetlenie linii w kolorze domyślnym */
    circle_d(200, 200, 50);		             /* Wyświetlenie okręgu o promieniu 50 w kolorze domyślnym */

    line(0, 25, 250, 500,128);		 /* Wyświetlenie linii w kolorze indeksowanym c=128 */
    circle(200, 200, 75, 254);			 /* Wyświetlenie okręgu w kolorze indeksowanym c=254 */


/*    fill_flood(int x, int y,
        ssh_color fill, ssh_color border); // Wypełnia powodziowo lub algorytmem siania
    fill_flood_rgb(int x, int y,
        int rf, int gf, int bf, int rb, int gb, int bb); */
#endif
    /* WYPROWADZANIE TEKSTU */
    printbw(10, 10, "%s", "...");           /* Drukuje w kolorach domyślnych */
    printc(10, 24, 255, 200, "%s", "...");    /* Drukuje w kolorach użytkownika */

    /* OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO */

    flush_plot();					/* Ostateczne uzgodnienie zawartości ekranu z zawartością pamięci */
    dump_screen("Test");	        /* Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */
    delay_ms(2000);				    /* Wymuszenie oczekiwania przez pewną liczbę milisekund */

    /* POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU) */
    while (true)
    {
        if( input_ready() ) 	            /* Funkcja sprawdzająca, czy jest cos do wzięcia z wejścia */
        {
            int c = get_char();    	        /* Funkcja odczytywania znaków sterowania */
            if( c=='a' )
                set_char('b');          /* Odesłanie znaku na wejście. Zwraca 0, jeśli nie ma miejsca */
                                            /* Pewne jest tylko odesłanie jednego znaku. */
            if( c==0)
            { /*Nic do zrobienia*/ }
            else if (c == '\b')
            {
                int xpos, ypos, click;
                get_mouse_event(xpos, ypos, click); /* Funkcja odczytująca ostatnie zdarzenie myszy */
                clear_screen();			/* Czyści ekran/okno lub ekran wirtualny */
            }
            else if (c == '\r')
            {
                int x, y;
                unsigned width, height;
                repaint_area(&x, &y, &width, &height); /* Podaje obszar, który ma być odnowiony i zwraca 0 */
                /* Jeśli zwraca -1 to brak danych lub brak implementacji. Odrysować całość! */
                /* Jeśli zwraca -2 to znaczy, że te dane już zostały odczytane. Należy zignorować! */
            }
            else if (c == ' ')
            {
                dump_screen("LINK TEST FOR SYMSHELL");
            }
            else if ( c == -1 || c == 27 )
                return; // KONIEC TESTU

        }
        delay_ms(100); //W module SVG to może być jedynie 1 ms.
    }
}

/* DLA close_plot() */
//WB_error_enter_before_clean = 0; /* Czy zamykać od razu, czy dać szanse na przejrzenie zawartości */

int main()
{
    test();
    close_plot();  /* zamkniecie grafiki (kiedyś też semigrafiki) */
    return 0;
}

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2026                         */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Spolecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
