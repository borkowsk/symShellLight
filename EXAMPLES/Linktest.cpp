/// (Przykładowy program SYMSHELL'A)/*
/// Linking test for:
/// SYMSHELL.H - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++
///*******************************************************************
/// designed by W.Borkowski: https://github.com/borkowsk
/// Last changed massively: 22.06.2007
/// Comments edition: 19.11.2021

#include "symshell.h"

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */
void test()
{
    unsigned char r=100,g=200,b=150;
    shell_setup("Just a linking test",0,nullptr);/* Przekazanie parametrów wywołania */
    buffering_setup(1);			/* Przełączanie buforowania okna. Może nie działać po inicjacji*/
    fix_size(0);					/* Czy symulować niezmienność rozmiarów okna, nawet gdy zmiana możliwa? */
    set_background(128);		    /* Ustala index koloru do czyszczenia okna. Może nie działać po inicjacji*/

    /* Operacje przestawiania własności pracy okna graficznego */
    mouse_activity(1);		/* Ustala czy mysz ma byc obsługiwana. Zwraca poprzedni stan flagi */
    print_transparently(1);	/* Włącza drukowanie tekstu bez zamazywania t�a. Zwraca stan poprzedni */
    line_width(2);		/* Ustala szerokość linii. Może być trochę kosztowne. Zwraca stan poprzedni */
    line_style(SSH_LINE_SOLID);          /* Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
    put_style(SSH_SOLID_PUT);            /* Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */
    set_pen(254,1,SSH_SOLID_PUT/*?*/);	  /* Ustala aktualny kolor linii za pomocą typu ssh_color */
    set_brush(128);                        /* Ustala aktualny kolor wypełnień za pomocą typu ssh_color */
    set_pen_rgb(r,g,b,2,SSH_SOLID_PUT);  /* Ustala aktualny kolor linii za pomocą składowych RGB */
    set_brush_rgb(r,g,b);/* Ustala aktualny kolor wypełnień za pomocą składowych RGB */
    set_rgb(128,10,100,200);       /* Zmienia definicje koloru w palecie kolorów. Indeksy 0..255 */
    set_gray(512,250);             /* Zmienia definicje odcienia szarości w palecie szarości. Indeksy 256..511 */

    init_plot(300,300,1,1); /* inicjacja grafiki (kiedyś też semigrafiki) */

    /* ODCZYTYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO*/
    int ret=buffered();				    /* Zwraca 1, jeśli buforowane */
    ret=fixed();					    /* Czy okno ma zafiksowana wielkość */
    ssh_color bcg=background();		    /* Aktualny kolor tla... */
    ret=get_line_width();				/* Aktualna grubość linii */
    ssh_color pn=get_pen();			    /* Aktualny kolor linii jako ssh_color */
    ssh_color br=get_brush();			/* Aktualny kolor wypełnień jako ssh_color */
    ret=screen_height();  				/* Aktualne rozmiary okna po przeliczeniach z init_plot*/
    ret=screen_width();   				/*  ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora */
    ret=char_height('X');			/* Aktualne rozmiary znaku  */
    ret=char_width('X');		    /* ...potrzebne do pozycjonowania tekstu */
    ret=string_height("Blablably");	/* Aktualne rozmiary łańcucha (string'u) */
    ret=string_width("Blablably");	/* ...potrzebne do jego pozycjonowania */

    /* WYPROWADZANIE TEKSTU */

    printbw(10,10,"%s","...");/* Drukuje w kolorach domyślnych*/
    printc(10,24,255,200,"%s","...");/* Drukuje w kolorach użytkownika*/

#ifdef Z_RYSOWANIEM
    /* TE FUNKCJE RACZEJ ZAWSZE SĄ ZAIMPLEMENTOWANE BO TO PODSTAWA */
    /* PUNKTOWANIE  */
    plot(int x,int y,
              ssh_color c);		/* Wyświetlenie punktu na ekranie */
    plot_rgb(int x,int y,
                  int r,int g,int b);
    fill_flood(int x,int y,
                    ssh_color fill,ssh_color border);/* Wypelnia powodziowo lub algorytmem siania */
    fill_flood_rgb(int x,int y,
                        int rf,int gf,int bf,int rb,int gb,int bb);
    /* RYSOWANIE  */
    line_d(int x1,int y1,int x2,int y2);/* Wyswietlenie lini w kolorze domyslnym */
    line(int x1,int y1,int x2,int y2,
              ssh_color c);			/* Wyswietlenie lini w kolorze c */

    circle_d(int x,int y,int r);		/* Wyswietlenie okregu w kolorze domyslnym */
    circle(int x,int y,int r,
                ssh_color c);			/* Wyswietlenie okregu w kolorze c */

    fill_circle_d(int x,int y,int r);	/* Wyswietlenie kola w kolorach domyslnych*/
    fill_circle(int x,int y,int r,
                     ssh_color c);			/* Wyswietlenie kola w kolorze c */

    fill_rect_d(int x1,int y1,int x2,int y2);/* Wyswietla prostokat w kolorach domyslnych*/
    fill_rect(int x1,int y1,int x2,int y2,
                   ssh_color c);			/* Wyswietla prostokat w kolorze c */

    fill_poly_d(int vx,int vy,
                     const ssh_point points[],int number);/* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
    fill_poly(int vx,int vy,
                   const ssh_point points[],int number,
                   ssh_color c);		/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */
#endif

    /* OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO */

    flush_plot();					/* Ostateczne uzgodnienie zawartości ekranu z zawartością pamięci */
    dump_screen("Test");	/* Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */
    delay_ms(1000);				/* Wymuszenie oczekiwania przez pewną liczbę milisekund */

    /* POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU) */
    if(input_ready()) 	/* Funkcja sprawdzająca, czy jest cos do wzięcia z wejścia */
    {
        int c= get_char();    	    /* Funkcja odczytywania znaków sterowania */
        set_char(c);		        /* Odesłanie znaku na wejście. Zwraca 0, jeśli nie ma miejsca */
                                    /* Pewne jest tylko odesłanie jednego znaku. */
        if(c=='\b')
        {
            int xpos,ypos,click;
            get_mouse_event(xpos,ypos,click); /* Funkcja odczytująca ostatnie zdarzenie myszy */
            clear_screen();			/* Czyści ekran/okno lub ekran wirtualny */
        }
        else if(c=='\r')
        {
            int x,y;
            unsigned width,height;
            repaint_area(&x,&y,&width,&height);/* Podaje obszar, który ma być odnowiony i zwraca 0 */
            /* Jeśli zwraca -1 to brak danych lub brak implementacji. Odrysować całość! */
            /* Jeśli zwraca -2 to znaczy, że te dane już zostały odczytane. Należy zignorować! */
        }
    }

    /* DLA close_plot() */
    WB_error_enter_before_clean/* =0 */; /* Czy zamykać od razu, czy dać szanse na przejrzenie zawartości */

    close_plot(); 					/* zamkniecie grafiki (kiedyś też semigrafiki) */
    /* Automatycznie instalowana w atexit() */
}

int main()
{
    test();
    return 0;
}

/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-19                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
