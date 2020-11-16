/* SYMSHELL.H - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++    */
/************************************************************************/
/* designed by W.Borkowski http://www.iss.uw.edu.pl/~borkowsk           */
/* Last changed: 22.06.2007                                			    */

#include "symshell.h"

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o dzia�aniu gwarantowanym przed inicjacja */
void test()
{
 unsigned char r=100,g=200,b=150;
 shell_setup("Just a linking test",0,NULL);/* Przekazanie parametrow wywolania */
 buffering_setup(1);			/* Przelaczanie buforowanie okna - moze nie dzialac po inicjacji*/
 fix_size(0);					/* Czy symulowa� niezmiennosc rozmiarow okna */
 set_background(128);		    /* Ustala index koloru do czyszczenia - moze nie dzialac po inicjacji*/
 set_rgb(128,10,100,200);       /* Zmienia definicje koloru w palecie kolorow. Indeksy 0..255 */
 set_gray(512,250);             /* Zmiania definicje odcienia szarosci w palecie szarosci. Indeksy 256..511 */
 init_plot(100,100,1,1); /* inicjacja grafiki/semigrafiki */

/* Operacje przestawiania wlasnosci pracy okna graficznego */
 mouse_activity(1);		/* Ustala czy mysz ma byc obslugiwana. Zwraca poprzedni stan flagi */
 print_transparently(1);	/* Wlacza drukowanie tekstu bez zamazywania t�a. Zwraca stan poprzedni */
 line_width(2);		/* Ustala szerokosc lini - moze byc kosztowne. Zwraca stan poprzedni */
 line_style(SSH_LINE_SOLID);          /* Ustala styl rysowania lini: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
 put_style(SSH_SOLID_PUT);           /* Ustala stosunek nowego rysowania do starej zawartosci ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */
 set_pen(254,1,SSH_SOLID_PUT/*?*/);			/* Ustala aktualny kolor linii za pomoca typu ssh_color */
 set_brush(ssh_color c);			/* Ustala aktualny kolor wypelnien za pomoca typu ssh_color */
 set_pen_rgb(r,g,b,2,SSH_SOLID_PUT); /* Ustala aktualny kolor linii za pomoca skladowych RGB */
 set_brush_rgb(r,g,b);/* Ustala aktualny kolor wypelnien za pomoca skladowych RGB */

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEN OKNA GRAFICZNEGO*/
 int ret=buffered();				    /* Zwraca 1 jesli buforowane */
 ret=fixed();					        /* Czy okno ma zafiksowana wielkosc */
 ssh_color bcg=background();		    /* Aktualny kolor tla... */
 ret=get_line_width(void);				/* Aktualna grubosc linii */
 ssh_color pn=get_pen(void);			/* Aktualny kolor linii jako ssh_color */
 ssh_color br=get_brush(void);			/* Aktualny kolor wypelnien jako ssh_color */
 ret=screen_height(void);  				/* Aktualne rozmiary okna po przeliczeniach z init_plot*/
 ret=screen_width(void);   				/*  ...i ewentualnych zmianach uczynionych "recznie" przez operatora */
 ret=char_height('X');			    /* Aktualne rozmiary znaku  */
 ret=char_width('X');				/* ...potrzebne do pozycjonowania tekstu */
 ret=string_height("Blablably");	/* Aktualne rozmiary lancucha */
 ret=string_width("Blablably");		/* ...potrzebne do jego pozycjonowania */

/* WYPROWADZANIE TEKSTU */

void printbw(int x,int y,const char* format,...);/* Drukuje w kolorach domyslnych*/
void printc(int x,int y,
			ssh_color fore,ssh_color back,
					   const char* format,...);/* Drukuje w kolorach uzytkownika*/

/* PUNKTOWANIE  */
void plot(int x,int y,
				ssh_color c);		/* Wyswietlenie punktu na ekranie */
void plot_rgb(int x,int y,
				int r,int g,int b);
void fill_flood(int x,int y,
				ssh_color fill,ssh_color border);/* Wypelnia powodziowo lub algorytmem siania */
void fill_flood_rgb(int x,int y,
				int rf,int gf,int bf,int rb,int gb,int bb);
/* RYSOWANIE  */
void line_d(int x1,int y1,int x2,int y2);/* Wyswietlenie lini w kolorze domyslnym */
void line(int x1,int y1,int x2,int y2,
				ssh_color c);			/* Wyswietlenie lini w kolorze c */

void circle_d(int x,int y,int r);		/* Wyswietlenie okregu w kolorze domyslnym */
void circle(int x,int y,int r,
				ssh_color c);			/* Wyswietlenie okregu w kolorze c */

void fill_circle_d(int x,int y,int r);	/* Wyswietlenie kola w kolorach domyslnych*/
void fill_circle(int x,int y,int r,
				ssh_color c);			/* Wyswietlenie kola w kolorze c */

void fill_rect_d(int x1,int y1,int x2,int y2);/* Wyswietla prostokat w kolorach domyslnych*/
void fill_rect(int x1,int y1,int x2,int y2,
				ssh_color c);			/* Wyswietla prostokat w kolorze c */

void fill_poly_d(int vx,int vy,
				const ssh_point points[],int number);/* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
void fill_poly(int vx,int vy,
				const ssh_point points[],int number,
				ssh_color c);		/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */

/* OPERACJE DOTYCZACE CALEGO OKNA GRAFICZNEGO */
  delay_ms(1000);					/* Wymuszenie oczekiwania przez pewn� liczb� ms */
  flush_plot();						/* Ostateczne uzgodnienie zawartosci ekranu z zawartoscia pamieci */
  dump_screen("Test");	/* Zapisuje zawartosc ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */
  clear_screen();						/* Czysci ekran lub ekran wirtualny */


/* POBIERANIE ZNAKOW Z KLAWIATURY i ZDAZEN OKIENNYCH (w tym z MENU) */
if(input_ready()) 	/* Funkcja sprawdzajaca czy jest cos do wziecia z wejscia */
{
 int c= get_char();    	/* Funkcja odczytywania znakow sterowania */
 set_char(c);		/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
						/* Pewne jest tylko odeslanie jednego znaku. */
 if(c=='\b')
 {
	 get_mouse_event(int* xpos,int* ypos,int* click);/* Funkcja odczytujaca ostatnie zdazenie myszy */
 }
 else if(c=='\r')
 {
	 repaint_area(int* x,int* y,int* width,int* height);/* Podaje obszar ktory ma byc odnowiony i zwraca 0 */
						/* Jesli zwraca -1 to brak danych lub brak implementacji ! Odrysowac calosc. */
						/* Jesli zwraca -2 to znaczy ze dane juz zostaly odczytane. Nalezy zignorowac. */
 }
}

/* DLA close_plot() */
WB_error_enter_before_clean/* =0 */; /* Czy zamykac od razu czy dac szanse na przejzenie zawartosci */

close_plot(); 					/* zamkniecie grafiki/semigrafiki */
										/* Automatycznie instalowana w atexit - stad durne (void) */
										/* zeby uniknac warningu          */

}

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                               (Don't change or remove this note) */
/********************************************************************/



