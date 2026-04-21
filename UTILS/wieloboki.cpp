/// @file
/// @brief  Implementacja klasa "wielobok" i biblioteki różnych wieloboków
//         ****************************************************************
///** @date 2026-04-21 (last modification) */

#include <cassert>
#include <cstring> //Bo musimy/chcemy używać funkcji "memcpy" - kopiowania pamięci oraz strcmp
#include <ctype.h> //Nazwy wieloboków mają być NON CASE SENSITIVE
#define _USE_MATH_DEFINES // For MSVC — gdzie indziej chyba nie przeszkadza
#include <cmath> //sin, cos, M_PI
#include <cfloat>

//#include <cstring> //stricmp() raz jest a raz go nie ma :-) zależnie od systemu

using namespace std; //Trochę to niebezpieczne, ale skraca zapis
#include "wieloboki.hpp"

//  Definicja konturów różnych figur:
//*==================================

static ssh_point kNamiot[]=
{{0,0},{10,30},{-10,30}};

static ssh_point kRomb[]=
{{0,0},{10,10},{0,20},{-10,10}};

static ssh_point kKwadrat[]=
{{-10,0},{10,0},{10,20},{-10,20}};

static ssh_point kDomek[]=
{{0,0},{10,10},{10,20},{-10,20},{-10,10}};

static ssh_point kUfo[]=
{{0,0},
 {10,10},{20,10},{30,20},{20,30},{10,30},{0,40},
 {-10,30},{-20,30},{-30,20},{-20,10},{-10,10}};

static ssh_point kPies[]={{0,0},{50,0},{50,-30},{60,-30},{60,10},{50,10},{50,80},{40,80},{40,40},{30,40},{30,90},
                    {20,90},{20,40},{-20,40},{-20,80},{-30,80},{-30,40},{-40,40},{-40,90},{-50,90},
                    {-50,20},{-90,20},{-90,10},{-70,-10},{-70,-20},{-50,0},{0,0}};

static ssh_point kPojazd0[]=
{{6,-2},{4,-1},{1,-2},{07,-5},{-15,-5},{-15,-2},{-17,-1},{-20,-2},{-20,-5},{-30,-5},
{-30,-10},{-20,-10},{-15,-15},{-0,-20},{5,-20},{20,-20},{20,-5},{0,-5}};

static ssh_point kPojazd1[]=
{{0,-2},{-2,-1},{-5,-2},{-5,-5},{-15,-5},{-15,-2},{-17,-1},{-20,-2},
       {-20,-5},{-30,-5},{-30,-10},{-20,-10},{-15,-15},{-0,-15},
       {5,-10},{10,-10},{10,-5},{0,-5}};

static ssh_point kPojazd2[]=
{{0,-2},{-2,-1},{-5,-2},{-5,-5},{-15,-5},{-15,-2},{-17,-1},{-20,-2},
       {-20,-5},{-30,-5},{-30,-10},{-20,-10},{-15,-15},{-0,-15},
       {8,-15},{10,-10},{10,-5},{0,-5}};

static ssh_point kPojazd3[]=
{{0,-2},{-2,-1},{-5,-2},{-5,-5},{-15,-5},{-15,-2},{-17,-1},{-20,-2},
       {-20,-5},{-30,-5},{-30,-10},{-20,-10},{-15,-15},{-0,-15},
       {5,-15},{10,-15},{10,-5},{0,-5}};

static ssh_point kPojazd4[]=
{{0,-2},{-2,-1},{-5,-2},{-5,-5},{-15,-5},{-15,-2},{-17,-1},{-20,-2},{-20,-5},
    {-30,-5},{-30,-10},{-20,-10},{-15,-15},{-0,-15},{5,-20},{10,-20},{10,-5},{0,-5}};

static ssh_point kLudzik0[]= //Prawie bezpłciowy (gender-neutral)
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,0},{3,1},{2,11},{3,11},{3,12},{0,12},{0,2},{0,12},{-3,12},{-3,11},{-2,11},{-3,1}, {-4,0},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}};

static ssh_point kLudzik1[]= //Prawie równie bezpłciowy
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,3},{3,2},{3,-3},{3,0},{2,11},{3,11},{3,12},{0,12},{0,2},{0,12},{-3,12},{-3,11},{-2,11},{-3,0},{-3,-3},{-3,2},{-4,3},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}};

static ssh_point kLudzik2[]= //jak zerowy
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,-2},{3,0},{2,11},{3,11},{3,12},{0,12},{0,2},{0,12},{-3,12},{-3,11},{-2,11},{-3,0},{-4,-2},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}};

static ssh_point kFacet1[]= //Facet 1.
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,0},{3,1},{2,11},{3,11},{3,12},{1,12},{0,2},{-1,12},{-3,12},{-3,11},{-2,11},{-3,1}, {-4,0},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}};

static ssh_point kFacet2[]= //Facet 2.
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,3},{3,2},{3,-3},{3,0},{2,11},{3,11},{3,12},{1,12},{0,2},
{-1,12},{-3,12},{-3,11},{-2,11},{-3,0}, {-3,-3},{-3,2}, {-4,3}, {-4,-6}, {-1,-8}, {-2,-10},{-2,-11},{-1,-12}};

static ssh_point kBabeczka1[]= //Kobieta 1.
{{0,-12},{1,-12},{2,-11}, {3,-9},{2,-10},{1,-9},{1,-8},{3,-7},{4,-5},{4,0},{3,1},{3,-4},{2,-2},{3,1},{1,11},{2,11},{2,12},{0,12},
{0,2},{0,12},{-2,12},{-2,11},{-1,11},{-3,1},{-2,-2},{-3,-4},{-3,1},{-4,0},{-4,-5},{-3,-7},{-1,-8},{-1,-9},{-2,-10},{-3,-9},{-2,-11},{-1,-12}};

static ssh_point kBabeczka2[]= //Kobieta 2.
{{0,-12},{1,-12},{2,-11}, {3,-9},{3,-10},{1,-9},{1,-8},{3,-7},{4,-5},{5,-1},{4,1},{3,-4},{2,-2},{3,1},{1,11},{2,11},{2,12},{0,12},
{0,2},{0,12}, {-2,12},{-2,11},{-1,11},{-3,1},{-2,-2},{-3,-4},{-4,1},{-5,-1},{-4,-5},{-3,-7},{-1,-8},{-1,-9},{-3,-10},{-3,-9},{-2,-11},{-1,-12}
};


static ssh_point kGmach1[]=
{{0,0},{10,3},{20,3},{20,23},{-20,23},{-20,3}};

static ssh_point kGmach2[]=
{{-20,5},{-10,5},{-10,0},{10,0},{10,5},{20,5},{20,30},{-20,30}};

static ssh_point kGmach3[]=
{{0,0},{25,10},{20,10},{20,30},{16,30},{16,10},{2,10},{2,30},{-2,30},{-2,10},{-16,10},{-16,30},{-20,30},{-20,10},{-25,10}};

static ssh_point kGmach4[]=
{{0,0},{20,10},{30,10},{40,20},{40,40},{-40,40},{-40,20},{-30,10},{-20,10}};


static ssh_point kMinisterstwo1[]=
{{0,0},{10,3},{20,3},{20,23},{-20,23},{-20,3}};

static ssh_point kMinistestwo2[]=
{{-20,5},{-10,5},{-10,0},{10,0},{10,5},{20,5},{20,30},{-20,30}};

static ssh_point kMinisterstwo3[]=
{{0,0},{25,10},{20,10},{20,30},{16,30},{16,10},{2,10},{2,30},{-2,30},{-2,10},{-16,10},{-16,30},{-20,30},{-20,10},{-25,10}};


// Definiujemy obiekty dla przykładowych kształtów — jest ich w sumie niewiele
static Wielobok WNamiot(kNamiot,sizeof(kNamiot)/sizeof(kNamiot[0]));
static Wielobok WRomb(kRomb,sizeof(kRomb)/sizeof(kRomb[0]));
static Wielobok WKwadrat(kKwadrat,sizeof(kKwadrat)/sizeof(kKwadrat[0]));
static Wielobok WDomek(kDomek,sizeof(kDomek)/sizeof(kDomek[0]));
static Wielobok WUfo(kUfo,sizeof(kUfo)/sizeof(kUfo[0]));
static Wielobok WPies(kPies,sizeof(kPies)/sizeof(kPies[0]));
static Wielobok WLudzik0(kLudzik0,sizeof(kLudzik0)/sizeof(kLudzik0[0]));
static Wielobok WLudzik1(kLudzik1,sizeof(kLudzik1)/sizeof(kLudzik1[0]));
static Wielobok WLudzik2(kLudzik2,sizeof(kLudzik2)/sizeof(kLudzik2[0]));
static Wielobok WFacet1(kFacet1,sizeof(kFacet1)/sizeof(kFacet1[0]));
static Wielobok WFacet2(kFacet2,sizeof(kFacet2)/sizeof(kFacet2[0]));
static Wielobok WBabeczka1(kBabeczka1,sizeof(kBabeczka1)/sizeof(kBabeczka1[0]));
static Wielobok WBabeczka2(kBabeczka2,sizeof(kBabeczka2)/sizeof(kBabeczka2[0]));
static Wielobok WPojazd0(kPojazd0,sizeof(kPojazd0)/sizeof(kPojazd0[0]));
static Wielobok WPojazd1(kPojazd1,sizeof(kPojazd1)/sizeof(kPojazd1[0]));
static Wielobok WPojazd2(kPojazd2,sizeof(kPojazd2)/sizeof(kPojazd2[0]));
static Wielobok WPojazd3(kPojazd3,sizeof(kPojazd3)/sizeof(kPojazd3[0]));
static Wielobok WPojazd4(kPojazd4,sizeof(kPojazd4)/sizeof(kPojazd4[0]));
static Wielobok WGmach1 (kGmach1,sizeof(kGmach1)/sizeof(kGmach1[0]));
static Wielobok WGmach2 (kGmach2,sizeof(kGmach2)/sizeof(kGmach2[0]));
static Wielobok WGmach3 (kGmach3,sizeof(kGmach3)/sizeof(kGmach3[0]));
static Wielobok WGmach4 (kGmach4,sizeof(kGmach4)/sizeof(kGmach4[0]));


// Właściwa biblioteka podstawowych kształtów w przestrzeni nazw klasy Wielobok:
//==============================================================================

struct ElementListy
{ const char* Nazwa; Wielobok* Ksztalt;
//ElementListy(){Nazwa=NULL;Ksztalt=NULL;}
};

static ElementListy ListaKsztaltow[1024]=//Lista jest ze sporym zapasem
{
{"romb",&WRomb},
{"kwadrat",&WKwadrat},
{"namiot",&WNamiot},
{"domek",&WDomek},
{"ufo",&WUfo},
{"pies",&WPies},
{"ludzik0",&WLudzik0},
{"ludzik1",&WLudzik1},
{"ludzik2",&WLudzik2},
{"facet1",&WFacet1},
{"facet2",&WFacet2},
{"kobieta1",&WBabeczka1},
{"kobieta2",&WBabeczka2},
{"pojazd0",&WPojazd0},
{"pojazd1",&WPojazd1},
{"pojazd2",&WPojazd2},
{"pojazd3",&WPojazd3},
{"pojazd4",&WPojazd4},
{"budynek1",&WGmach1},
{"budynek2",&WGmach2},
{"budynek3",&WGmach3},
{"budynek4",&WGmach4},
//{"",},
{"diamond",&WRomb},
{"square",&WKwadrat},
{"tent",&WNamiot},
{"house",&WDomek},
{"dog",&WPies},
{"human0",&WLudzik0},
{"human1",&WLudzik1},
{"human2",&WLudzik2},
{"man1",&WFacet1},
{"man2",&WFacet2},
{"woman1",&WBabeczka1},
{"woman2",&WBabeczka2},
{"vehicle0",&WPojazd0},
{"vehicle1",&WPojazd1},
{"vehicle2",&WPojazd2},
{"vehicle3",&WPojazd3},
{"vehicle4",&WPojazd4},
{"building1",&WGmach1},
{"building2",&WGmach2},
{"building3",&WGmach3},
{"building4",&WGmach4},
//{"",&},
{NULL,NULL} //STRA�NIK!!!
};

// Zrobione jako statyczne metody, bo to pozwala robić potem różne implementacje
// przechowywania tych wieloboków, np. tworzyć je dopiero wtedy, gdy będą potrzebne
// lub ściągać z dysku.
const Wielobok& Wielobok::Ludzik(int typ)
{
      switch(typ){
      default:
      case 0: return WLudzik0;
      case 1: return WLudzik1;
      case 2: return WLudzik2;
      case 3: return WFacet1;
      case 4: return WBabeczka1;
      case 5: return WFacet2;
      case 6: return WBabeczka2;
      }
}

const char*     Wielobok::NazwyWielobokow(int pos)
{
    if(pos<sizeof(ListaKsztaltow)/sizeof(ListaKsztaltow[0]))
        return ListaKsztaltow[pos].Nazwa;
    else
        return NULL; //Jak NULL to nie ma więcej
}

static char* strcplower( char* t, const char* s )
{
  char* p = t;
  while ( (*p = tolower( *s++ )) ) p++;
  return t;
}

// \note Ewentualnie może być nazwa pliku
const Wielobok& Wielobok::WielobokWgNazwy(const char* Nazwa)
{
    char locNazwa[255]; //Nie jest to w pełni bezpieczne
    strcplower(locNazwa,Nazwa); //Zmieniamy na same małe litery

    unsigned size=sizeof(ListaKsztaltow)/sizeof(ListaKsztaltow[0]);
    for(unsigned i=0;i<size && ListaKsztaltow[i].Nazwa!=NULL;i++)
    {
#ifdef _MSC_VER
            if(_stricmp(ListaKsztaltow[i].Nazwa,locNazwa)==0) //_stricmp jest w MSVC a stricmp jest niby stare z POSIXa
#else
            if( strcasecmp(ListaKsztaltow[i].Nazwa,locNazwa)==0 )
#endif
                return *ListaKsztaltow[i].Ksztalt;
    }
    return Domyslny();
}

// \return obiekt z tablicy lub załadowany.
// Jak nie ma to NULL
// \note TODO: Może próbować załadować z pliku *.poly
const Wielobok* Wielobok::SprobujWielobok(const char* Nazwa)
{
    char locNazwa[255];//Nie jest to w pe?ni bezpieczne
    strcplower(locNazwa,Nazwa); //Zmieniamy na same małe litery

    unsigned size=sizeof(ListaKsztaltow)/sizeof(ListaKsztaltow[0]);
    for(unsigned i=0;i<size && ListaKsztaltow[i].Nazwa!=NULL;i++)
    {
#ifdef _MSC_VER
            if(_stricmp(ListaKsztaltow[i].Nazwa,locNazwa)==0)//_stricmp jest niby ISO a stricmp jest niby stare z POSIXa
#else
            if(strcasecmp(ListaKsztaltow[i].Nazwa,locNazwa)==0)
#endif
                return ListaKsztaltow[i].Ksztalt;
    }
    //Jednak nie ma
    //Proba ładowania z pliku - raz załadowane zostaje do konca działania programu!
    // TODO!!!
    return NULL;
}

// Skróty do częściej używanych:
// /////////////////////////////
const Wielobok&  Wielobok::Domyslny() {return WRomb;}
const Wielobok&  Wielobok::Romb() {return WRomb;}
const Wielobok&  Wielobok::Namiot() {return WNamiot;}
const Wielobok&  Wielobok::Domek(){return WDomek;}
const Wielobok&  Wielobok::Ufo() {return WUfo;}

//  WŁAŚCIWA IMPLEMENTACJA KLASY WIELOBOK
// //////////////////////////////////////

// Transformacje. 
// ////////////// 

// Zmienia współrzędne tak, żeby były wokół środka ciężkości.
// Modyfikuje listę punktów, żeby było wygodniej (?)
void Wielobok::Centruj()
{
  unsigned i;
  
  double x=0,y=0;
  for(i=0;i<Ilobok;i++) //Najpierw trzeba znaleźć środek ciężkości figury
  {
     x+=Punkty[i].x;
     y+=Punkty[i].y;
  }   
  x/=double(Ilobok);
  y/=double(Ilobok);
  
  for(i=0;i<Ilobok;i++) //A teraz można przesunąć do środka
  {
     Punkty[i].x-=x;
     Punkty[i].y-=y;
  }         
}

// Metoda przelatuje punkty i daje informacje o otaczaj�cym prostok�cie
// Oraz otaczającym okręgu ze środkiem w punkcie 0,0
void Wielobok::Zakresy( double& MinX,double& MinY,  
                        double& MaxX,double& MaxY,
                        double& R) const
{
    MinX=MinY=DBL_MAX;
    MinX=MinY=R=-DBL_MAX;

    for(unsigned i=0;i<Ilobok;i++)
    {
     double pomX=Punkty[i].x;
     double pomY=Punkty[i].y;
     if(pomX<MinX) MinX=pomX;
     if(pomY<MinY) MinY=pomY;
     if(pomX>MaxX) MaxX=pomX;
     if(pomY>MaxY) MaxY=pomY;

     double D=pomX*pomX+pomY*pomY;
     if(D>0) D=sqrt(D);
     if(D>R) R=D;
    }
}

// Skaluje o jakiś x i y, czyli różnie dla każdego wymiaru.
// \note Zmienia wszystkie współrzędne!
void Wielobok::Skaluj(double x,double y)
{
    for(unsigned i=0;i<Ilobok;i++)
    {
     Punkty[i].x*=x;
     Punkty[i].y*=y;        
    }         
}

// Zmienia w odbicie lustrzane pionowo.
// \note Zmienia wszystkie współrzędne!
void Wielobok::OdbijWPionie()
{
    for(unsigned i=0;i<Ilobok;i++)
    {
     Punkty[i].y=-Punkty[i].y;        
    }        
}

// Zmienia w odbicie lustrzane poziomo
// \note Zmienia wszystkie współrzędne!
void Wielobok::OdbijWPoziomie()
{
    for(unsigned i=0;i<Ilobok;i++)
    {
     Punkty[i].x=-Punkty[i].x;     
    } 
}  

// Obraca o ileś radianów.
// \note Zmienia wszystkie współrzędne co na dłuższą metę jest niszczące dla obiektu!
void Wielobok::ObracajORad(double Radiany)
{
    double cosR=cos(Radiany);
    double sinR=sin(Radiany);

    for(unsigned i=0;i<Ilobok;i++)
    {
     double x = Punkty[i].x;
     double y = Punkty[i].y;
     Punkty[i].x = x*cosR-y*sinR;
     Punkty[i].y = x*sinR+y*cosR;
    }
}

// \note Konstruktory - tworzą wieloboki na bazie wzorca, tablicy albo innego Wieloboku.
//		  Nie ma możliwości stworzenia pustego "Wieloboku", choć może być "zerowy",
//		  ale to już na odpowiedzialność użytkownika klasy!
Wielobok::Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorka)
{																assert(RozmiarWzorka>2);
   Punkty=new ssh_point[RozmiarWzorka];							assert(Punkty!=NULL);
   Ilobok=RozmiarWzorka;
   memcpy(Punkty,Wzorek,Ilobok*sizeof(ssh_point));           
}

// Konstruktor kopiujący
Wielobok::Wielobok(const Wielobok& Wzorek)
{
   Punkty=new ssh_point[Wzorek.Ilobok];							assert(Punkty!=NULL);
   Ilobok=Wzorek.Ilobok;
   memcpy(Punkty,Wzorek.Punkty,Ilobok*sizeof(ssh_point));           
}

// Konstruktor N-boków o zmiennym rozmiarze
Wielobok::Wielobok(unsigned IleBokow,float R)
{
   Punkty=new ssh_point[IleBokow];								assert(Punkty!=NULL);
   Ilobok=IleBokow;
   double alfa=(2*M_PI)/Ilobok;
   for(unsigned i=0;i<Ilobok;i++)
   {
     Punkty[i].x=double(R)*sin(alfa*i);
     Punkty[i].y=-double(R)*cos(alfa*i);
   }
}

// Destruktor: musi zwolnić pomocniczą tablicę
Wielobok::~Wielobok()
{
  delete []Punkty;
}


// \note
//		Rysowanie figury w miejscu i kolorze zdefiniowanym gdzie indziej
//		zrobione jako osobna procedura, bo możemy rozbudować figure
//     o dodatkowe elementy niebędące składowymi samego wielokąta.

// Rysuje gdzieś wielobok w zadanym kolorze. Nie modyfikuje stanu obiektu!
void Wielobok::Rysuj(int StartX,int StartY,ssh_color Color) const
{        
   fill_poly(StartX,StartY,Punkty,Ilobok,Color);
}

// Rysuje gdzieś wielobok w zadanym kolorze RGB. Nie modyfikuje stanu obiektu!
void Wielobok::Rysuj(int StartX,int StartY,int R,int G,int B) const
{
   set_brush_rgb(R,G,B);
   set_pen_rgb(R,G,B,0,1);
   fill_poly_d(StartX,StartY,Punkty,Ilobok);
}

// Po prostu udostępnia wartość konkretnego punktu, ale bez możliwości zmiany.
const ssh_point&  Wielobok::DajPunkt(unsigned pozycja) const
{
    return Punkty[pozycja];
}

/* *******************************************************************/
/*                   SYMSHELLLIGHT  version 2026                     */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/


