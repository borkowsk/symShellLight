/// \file
/// \brief Klasa wielobok i biblioteka różnych wieloboków
///        ***********************************************
/// \details Powstała jako przykład definiowania dosyć prostej klasy
/// \author  borkowsk
// ////////////////////////////////////////////////////////////
#ifndef __WBORK_WIELOBOKI_HPP__
#define __WBORK_WIELOBOKI_HPP__

#include "symshell.h" //Potrzebne typy z symshell'a

/// \brief Definicja klasy Wielobok - NIEMAL bez żadnej implementacji metod wewnątrz
class   Wielobok
{
private: // Pola prywatne - nikt nie ma bezpośredniego dostępu
         // Takie pola będą w każdym obiekcie tej klasy:
ssh_point* Punkty; ///< Wskaźnik do listy punktów
unsigned   Ilobok; ///< Ile jest punktów na liście

public:  // Interface - metody widoczne dla użytkowników klasy

// Metody działające też dla Wieloboków stałych (const)
unsigned IlePunktow() const {return Ilobok;}     ///< Ile ma wierzchołków. To może być metoda "inline" - szybsza
const ssh_point& DajPunkt(unsigned Ktory) const; ///< A to lepiej już nie tu, bo "zaciemni" kod
                                                 ///< \return Ta metoda zwraca "stałą referencję" do punktu,
                                                 ///< co pozwala na czytanie, ale nie pozwala modyfikować tego punktu

/// \brief   Określenie otoczenia figury
/// \details Metoda przelatuje punkty i daje informacje
///          - O otaczającym prostokącie
///          - oraz o promieniu otaczajacego okręgu ze środkiem w punkcie 0,0
void Zakresy(  double& MinX,  ///< [out] najmniejszy X
               double& MinY,  ///< [out] najmniejszy Y
			   double& MaxX,  ///< [out] największy X
               double& MaxY,  ///< [out] największy Y
			   double& R      ///< [out] promień opisującego okręgu
               ) const;

/// \brief Rysuje gdzieś wielobok w zadanym kolorze indeksowanym. \note Nie modyfikuje stanu wielokąta!
void Rysuj(int x,int y,ssh_color Color) const;

/// \brief Rysuje gdzieś wielobok w zadanym kolorze RGB. \note Nie modyfikuje stanu wielokąta!
void Rysuj(int x,int y,int R,int G,int B) const;

// Konstruktory tworzą obiekty klasy Wielobok.
// Na bazie wzorców, czyli tablic albo innych Wieloboków.
// Nie ma możliwości stworzenia pustego "Wieloboku",
// choć na upartego może być "zerowy"
Wielobok(const Wielobok& Wzorek); ///< Konstruktor kopiujący
Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorka); ///< Konstruktor z tablicy punktów
Wielobok(unsigned IleBokow,float R); ///< Konstruktor N-kąta o zmiennej liczbie boków i rozmiarze

/// \brief Destruktor - bo trzeba zwolnić pomocniczą tablice.
//  Ale i tak każda klasa powinna mieć
/// \note "virtual" - bo tak jest bezpieczniej.
//  Co to znaczy będzie innym razem :-)
virtual ~Wielobok();

// Transformacje. Modyfikują listę punktów, żeby było wygodniej
void OdbijWPionie();   ///< Zmienia w odbicie lustrzane pionowo
void OdbijWPoziomie(); ///< Zmienia w odbicie lustrzane poziomo

// Uwaga! Następne trzy metody są "niszczące", bo punkty są pamiętane
// na liczbach całkowitych i wyniki WCALE nie muszą takie być!
// W miarę bezpiecznie można użyć raz lub dwa, bo potem kształt się krzywi.
void ObracajORad(double Radiany); ///< Obraca o ileś radianów
void Skaluj(double x,double y); ///< Zmienia współrzędne
void Centruj(); ///< Zmienia współrzędne tak, żeby były wokół środka ciężkości


// Biblioteka podstawowych kształtów w przestrzeni nazw klasy Wielobok
// *******************************************************************
// Zrobione jako statyczne metody, bo to pozwala robić potem różne implementacje
// przechowywania tych wieloboków, np. tworzyć je dopiero wtedy, gdy będą potrzebne
// lub ściągać z dysku.
// Funkcje zwracają "stałe referencje" czyli można czytać, rysować, ale nie zmieniać.
// "static" w odniesieniu do metody oznacza, że jest to składowa klasy jako takiej,
// a nie każdego obiektu tej klasy z osobna. Do wywołania obiekt nie jest potrzebny!

static const char*     NazwyWielobokow(int pos);           ///< Zwraca nazwę i-tego wieloboku z biblioteki.
                                                           ///< \return Jak NULL to nie ma już więcej
static const Wielobok& WielobokWgNazwy(const char* Nazwa); ///< Kształt z biblioteki \return Jak nie ma takiego to domyślny
static const Wielobok* SprobujWielobok(const char* Nazwa); ///< Jak nie ma to zwraca NULL (TODO lub probuje załadować z pliku)
static const Wielobok& Domyslny();                         ///< Kształt domyślny, jak nie ma potrzebnego!

// Skróty do częściej używanych wieloboków z biblioteki
static const Wielobok& Namiot();            ///< Kształt namiotu. Skrótowy dostęp do obiektu z biblioteki kształtów.
static const Wielobok& Romb();              ///< Kształt rombu. Skrótowy dostęp do obiektu z biblioteki kształtów.
static const Wielobok& Domek();             ///< Kształt domu. Skrótowy dostęp do obiektu z biblioteki kształtów.
static const Wielobok& Ufo();               ///< Kształt UFO. Skrótowy dostęp do obiektu z biblioteki kształtów.
static const Wielobok& Ludzik(int typ);     ///< Kształt człowieka. Skrótowy dostęp do obiektu z biblioteki kształtów.
};

/* ****************************************************************** */
/*              SYMSHELLLIGHT  version 2022-10-27                     */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
#endif //__WBORK_WIELOBOKI_HPP__

