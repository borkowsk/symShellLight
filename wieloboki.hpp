//  Klasa wielobok i biblioteka r�nych wielobok�w
// Przyk�ad definiowania dosy� prostej klasy
// ////////////////////////////////////////////////////////////
#ifndef __WBORK_WIELOBOKI_HPP__
#define __WBORK_WIELOBOKI_HPP__

#include "symshell.h" //Potrzebne typy z symshell'a

//Definicja klasy Wielobok - niemal bez �adnej implementacji metod
class   Wielobok
// ////////////////////////////////
{
private: //Pola prywatne - nikt nie ma bezpo�redniego dost�pu    
//Takie pola b�d� w ka�dym obiekcie tej klasy: 
ssh_point* Punkty; //Wska�nik do listy punkt�w      
unsigned   Ilobok;//"Ilobok"? Czyli ile jest punkt�w na li�cie

public: //Interface - metody widoczne dla u�ytkownik�w klasy

//Metody dzia�aj�ce te� dla Wielobok�w sta�ych (const)  
unsigned IlePunktow() const {return Ilobok;} //To mo�e by� "inline" - szybkie   
const ssh_point& DajPunkt(unsigned Ktory) const;//A to lepierj nie tu, bo "zaciemni" kod
		//Ta metoda zwraca "sta�� referencj�" do punktu, co pozwala na czytanie ale nie pozwala modyfikowa� tego punktu
void Zakresy(double& MinX,double& MinY,  //Metoda przelatuje punkty i daje informacje
			   double& MaxX,double& MaxY,  //O otaczaj�cym prostok�cie
			   double& R) const;		   //oraz okr�gu ze srodkiem w punkcie 0,0

//Rysuj() - rysuje gdzie� wielobok w zadanym kolorze. Nie modyfikuje stanu!
void Rysuj(int x,int y,ssh_color Color) const;
void Rysuj(int x,int y,int R,int G,int B) const;

//Konstruktory - tworz� obiekty klasy Wielobok 
//Na bazie wzorc�w - tablic albo innych Wielobok�w
//Oraz metod� wpisania w ko�o o zadanym promieniu
//Nie ma mo�liwo�ci stworzenia pustego "Wieloboku",
//cho� na upartego mo�e by� "zerowy"
Wielobok(const Wielobok& Wzorek); //Konstruktor kopuj�cy
Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorka);//Konstruktor z tablicy
Wielobok(unsigned IleBokow,float R);//Konstruktor N-bok�w o zmiennym rozmiarze

//Destruktor - bo trzeba zwolni� pomocnicz� tablice.
//"virtual" - bo tak jest bezpieczniej.
//Co to znaczy b�dzie innym razem :-)
virtual ~Wielobok();

//Transformacje. Modyfikuj� list� punkt�w �eby by�o wygodniej
void OdbijWPionie();//Zmienia w odbicie lustrzane pionowo
void OdbijWPoziomie();//Zmienia w odbicie lustrzane poziomo

//Uwaga - nast�pne trzy ostatnie s� "niszcz�ce", bo punkty s� pami�tane
//na liczbach ca�kowitych i wyniki WCALE nie musz� takie by�!
//W miar� bezpiecznie mo�na u�y� raz lub dwa, potek kszta�t si� krzywi
void ObrocORad(double Radiany);//Obraca o ile� radian�w
void Skaluj(double x,double y);//Zmienia wsp�rz�dne
void Centruj();//Zmienia wsp�rz�dne tak, �eby by�y wok� �rodka ci�ko�ci


//Biblioteka podstawowych kszta�t�w w przestrzeni nazw klasy Wielobok, 
//Zrobione jako statyczne metody bo to pozwala robi� potem r�ne implementacje 
//przechowywania tych wielobok�w, np. tworzy� je dopiero gdy b�d� potrzebne
//lub �ci�ga� z dysku.
//Zwracaj� "sta�e referencje" czyli mo�na czyta�, rysowa�, ale nie zmienia�. 
//"static" w odniesieniu do metody oznacza, �e jest to sk�adowa klasy jako takiej
// a nie ka�dego obiektu tej klasy z osobna. Do wywo�ania obiekt nie jest potrzebny!
static const Wielobok& Domyslny(); //Kszta�t domylsny, jak nie ma �adanego!
static const Wielobok& WielobokWgNazwy(const char* Nazwa);//Jak nie ma to domylsny
static const Wielobok* SprobujWielobok(const char* Nazwa);//Jak nie ma to NULL i probuje za�adowac z pliku *.poly
static const char*     NazwyWielobokow(int pos); //Jak NULL to nie ma wi�cej
//Skr�ty do cz�sciej u�ywanych
static const Wielobok& Namiot();
static const Wielobok& Romb();
static const Wielobok& Domek();
static const Wielobok& Ufo();
static const Wielobok& Ludzik(int typ);
};

#endif //__WBORK_WIELOBOKI_HPP__

