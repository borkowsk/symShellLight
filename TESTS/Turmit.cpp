////////////////////////////////////////////////////////////////////////////////
//		Turmit prototypowy - "Mrówka Langtona" z mo¿liwoœci¹ wariacji na temat
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs³uga grafiki, ale z odtwarzaniem ekranu i obslug¹ zdarzen.
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
// Turmit ma alement aktywny i œrodowisko, tzw. "g³owicê" albo "czo³o" oraz pamieæ
// poniewa¿ turmit jest tak naprawdê 2 wymiarowym uogólnieniem maszyny Turinga

#include "symshell.h"
#include <stdio.h> //Wyjœcie na konsole a la jêzyk C - printf(....)
#include <math.h>
#include <fstream>

#define NAZWAMODELU  "Turmit_przyspieszony" //U¿ycie define u³atwia montowanie sta³ych ³¹ñcuchów
//Wyjœciowy rozmiar œwiata i "ekranu" symulacji
const int size=700;

unsigned char World[size][size];//Tablica œwiata - wyzerowana na poczatku bo globalna
         //unsigned char ¿eby by³o od 0 do 255, bo typ char bywa te¿ "signed" (zaleznie od kompilatora)
unsigned step_counter=0;//Licznik realnych kroków modelu

void init_world()
//Funkcja do zapoczatkowania œwiata
{
   //World[0][0]=255;//TODO: ewentualna inicjalizacja losowa
}

struct Turmit //Struktura definiuj¹ca stan turmita
{
  int x,y;//po³o¿enie
  int stan;//I pamiêæ robocza - np. kierunek
  Turmit(int ix,int iy,int is):x(ix),y(iy),stan(is){} //KONSTRUKTOR!
};

struct //Struktura anonimowa dla kierunków ruchu
{
  int dx,dy;
} Directions[4]={{0,-1},{1,0},{0,1},{-1,0}}; //góra,prawo,dó³,lewo

Turmit LaAnt(size/2,size/2,0); //Inicjalizacja mrówki na œrodku z kierunkiem "góra"

void single_step()
//Funkcja robi¹ca jeden krok symulacji
{
    LaAnt.x=( LaAnt.x+Directions[LaAnt.stan].dx //Przesuniêcie po X
    + size ) % size; //i zabezpieczenie ¿eby nie wyjœæ za tablicê
    LaAnt.y=( LaAnt.y+Directions[LaAnt.stan].dy //Przeusniecie po Y
    + size ) % size; //i zabezpieczenie ¿eby nie wyjœæ za tablicê
    
    if(World[LaAnt.y][LaAnt.x]==0)
    {
      plot(LaAnt.x,LaAnt.y,128);
      World[LaAnt.y][LaAnt.x]=1;//Mo¿e byæ inna liczba - to tylko marker odwiedzin
      LaAnt.stan=(LaAnt.stan+1)%4;//Mrówka jest czterokierunkowa
    }
    else
    {
      plot(LaAnt.x,LaAnt.y,255);
      World[LaAnt.y][LaAnt.x]=0;
      LaAnt.stan=(LaAnt.stan+4-1)%4;//Mrówka jest czterokierunkowa
    }    
    //Licznik kroków mrówki
    step_counter++;
}

void stats()
//Funkcja do obliczenia statystyk
{
  //TODO: Np œredniej liczny odwiedzeñ ju¿ odwiedzonych oraz liczby pustych
}


//Do wizualizacji obs³ugi zdarzeñ
const int DELA=0;//Jak d³ugie oczekiwanie w obrêbie pêtli zdarzeñ
const int VISUAL=1000;//Co ile kroków symulacji odrysowywaæ widok
const char* CZEKAM="Klikaj lub patrz! "; //Monit w pêtli zdarzeñ
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

//Kilka deklaracja zapowiadaj¹cych inne funkcje obs³uguj¹ce model
void replot(); //Funkcja odrysowyj¹ca
void read_mouse(); // Obslyga myszy - uzywaæ o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u¿ywaæ o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje coœ na ekranie
{
  for(int x=0;x<size;x++)
    for(int y=0;y<size;y++)  
      {
        unsigned z=World[y][x]*20;//Co tam by³o? Wzmocnione
        z%=256; //¯eby nie przekroczyæ kolorów
        //z%=512; //Albo z szaroœciami
        plot(x,y,z);
      }
  printc(size/2,size,128,255,"%06u  ",step_counter);//Licznik kroków    
  //Ostatnie polozenie kliku - bia³y krzy¿yk   
  //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 fix_size(1);       // Czy udajemy ¿e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz bêdzie obs³ugiwana?   
 buffering_setup(1);// Czy bêdzie pamietaæ w bitmapie zawartosc ekranu? PAMIÊTANIE PRZYŒPIESZA!
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u¿yciem parametrów wywo³ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo¿na rysowaæ i pisac w oknie 
 init_world();
 replot();
 flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa 

 bool not_finished=true;//Zmienna steruj¹ca zakoñczeniem programu
 unsigned loop=0;    //Do zliczania nawrotów pêtli zdarzeñ
 while(not_finished) //PÊTLA OBS£UGI ZDARZEÑ
    { 
      int pom; //NA ZNAK Z WEJŒCIE OKNA GRAFICZNEGO
      loop++; 
      if(step_counter%(VISUAL/10)==0 && //Dla przyœpieszenia tylko czasami sprawdzamy 
         input_ready()) //Czy jest zdarzenie do obs³ugi?
      {
       pom=get_char(); //Przeczytaj nades³any znak
       switch(pom)
       {
       case 'd': screen_to_file();break; //Zrzut grafiki          
       case 'p': write_to_file();break;//Zapis do pliku tekstowego
       case '\r': replot(); flush_plot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Jest zdarzenie myszy
       case EOF:  //Typowe zakoñczenie
       case  27:  //ESC
       case 'q':  //Zakoñczenie zdefiniowane przez programiste
       case 'Q': not_finished=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            printf("Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Grafika gotowa   
            break;
        }
      }
      else //Symulacja - jako akcja na wypadek braku zdarzeñ do obslugi
      {
       single_step(); //Nastêpny krok
       if(step_counter%VISUAL==0) //Odrysuj gdy reszta z dzielenia równa 0
       {
             flush_plot();//¯eby to co rysowane w trakcie choæ przez chwile b³ysnê³o
             replot();//Jest najbardziej kosztowny!
             printc(0,screen_height()-char_height('C'),128,(step_counter%3?255:250),CZEKAM);
             flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
             delay_ms(DELA);//Wymuszenie oczekiwania - ¿eby pêtla nie zjada³a ca³ego procesora  
       }
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}

void read_mouse()
//Procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      xmouse=xpos;ymouse=ypos;
      //TODO - zaimplementowaæ jeœli potrzebne                                   
      //...
   }                      
}

void write_to_file()
{
     const char* NazwaPliku=NAZWAMODELU ".out";//U¿ywamy sztuczki ze zlepianiem sta³ych 
                                        //³añcuchowych przez kompilator
     std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z góry
     //TODO - funkcja powinna zapisaæ wyniki modelu do pliku zamiast wyrysowaywaæ na ekranie
     //Format - tabela liczb odpowieniego typu rozdzielanych tabulacjami
     //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
     
     out.close();
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{
   char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
   dump_screen(bufor);
}

