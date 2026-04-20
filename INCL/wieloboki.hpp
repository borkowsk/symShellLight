/// \file
/// \brief Klasa wielobok i biblioteka różnych wieloboków
//        ************************************************
/** @date 2026-04-20 (last modification */
/// \details Powstała jako przykład definiowania dosyć prostej klasy, a potem się skomplikowała.
///          Jednak z przyczyn historycznych zachowała polskie nazewnictwo w konwencji "wielbłądowej".
/// \author  borkowsk
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SYMSHELL_WIELOBOKI_HPP_INCLUDED_
#define SYMSHELL_WIELOBOKI_HPP_INCLUDED_ (1)

#include "symshell.h" //Potrzebne typy z symshell'a
#include "maybe_unused.h"

/**
 * @defgroup SymShellUtils Różne dodatkowe narzędzia do grafiki
 * \brief    Zdefiniowane kolory i dodatkowe kształty.
 */
/// @{

/// \brief Definicja klasy Wielobok — NIEMAL bez żadnej implementacji metod wewnątrz.
class   Wielobok
{
private: // Pola prywatne — nikt nie ma bezpośredniego dostępu.
         // Takie pola będą w każdym obiekcie tej klasy:
         //*////////////////////////////////////////////
    ssh_point* Punkty; //!< Wskaźnik do listy punktów.
    unsigned   Ilobok; //!< Ile jest punktów na liście.

public:  // Interface — metody widoczne dla użytkowników klasy:
         //*///////////////////////////////////////////////////

    // Metody działające też dla Wieloboków stałych (const):
    //======================================================
    MAYBE_UNUSED
    unsigned IlePunktow() const {return Ilobok;}     //!< Ile ma wierzchołków. To może być metoda "inline" - szybsza.

    /// \brief Dostęp do pojedynczego wierzchołka wielokąta. \return Ta metoda zwraca "stałą referencję" do punktu,
    ///  Pozwala na odczytanie, ale nie pozwala modyfikować tego punktu.
    MAYBE_UNUSED
    const ssh_point& DajPunkt(unsigned pozycja) const;

    /// \brief Rysuje gdzieś wielobok w zadanym kolorze indeksowanym. \note Nie modyfikuje stanu wielokąta!
    MAYBE_UNUSED
    void Rysuj(int x,int y,ssh_color color) const;

    /// \brief Rysuje gdzieś wielobok w zadanym kolorze RGB. \note Nie modyfikuje stanu wielokąta!
    MAYBE_UNUSED
    void Rysuj(int x,int y,int R,int G,int B) const;

    /// \brief   Określenie otoczenia wieloboku.
    /// \details Metoda przegląda punkty i daje informacje
    ///          - o otaczającym prostokącie,
    ///          - oraz o promieniu otaczającego okręgu ze środkiem w punkcie 0,0.
    MAYBE_UNUSED
    void Zakresy(  double& MinX,  //!< [out] oznacza najmniejszy X.
                   double& MinY,  //!< [out] oznacza najmniejszy Y.
                   double& MaxX,  //!< [out] oznacza największy X.
                   double& MaxY,  //!< [out] oznacza największy Y.
                   double& r      //!< [out] oznacza promień opisującego okręgu.
                   ) const;

    // Metody działające też dla Wieloboków stałych z możliwością zapisu:
    //===================================================================

    /// @name Konstruktory tworzące obiekty klasy `Wielobok`.
    /// @details
    ///          Tworzenie odbywa się na bazie wzorców, czyli tablic albo innych `Wielobok`-ów.
    ///          Nie ma możliwości stworzenia pustego "Wieloboku", choć na upartego może być "zerowy".
    /// @{
    Wielobok(const Wielobok& wzorek);                           //!< Konstruktor kopiujący.
    Wielobok(const ssh_point wzorek[],unsigned rozmiar_wzorka); //!< Konstruktor z tablicy punktów.
    Wielobok(unsigned ile_bokow,float r);                       //!< Konstruktor N-kąta o zmiennej liczbie boków i rozmiarze.
    /// @}

    /// \brief Destruktor. Zwalnia pomocniczą tablicę. \note "virtual" - bo taki jest bezpieczniej przy dziedziczeniu.
    virtual ~Wielobok();

    /// @name Transformacje wieloboku.
    /// @details Modyfikują listę punktów danego wieloboku.
    /// \note Metody obracania, skalowania i centrowania są "niszczące", bo punkty są pamiętane
    ///       na liczbach całkowitych i wyniki WCALE nie muszą takie być!
    ///       W miarę bezpiecznie można użyć raz lub dwa, bo potem kształt się krzywi.
    /// @{
    MAYBE_UNUSED
    void OdbijWPionie();   ///< Zmienia wielobok w odbicie lustrzane pionowo

    MAYBE_UNUSED
    void OdbijWPoziomie(); ///< Zmienia wielobok w odbicie lustrzane poziomo

    MAYBE_UNUSED
    void ObracajORad(double radiany);  //!< Obraca wielobok o ileś radianów.

    MAYBE_UNUSED
    void Skaluj(double sx,double sy);  //!< Przeskalowuje rozmiar wieloboku.

    MAYBE_UNUSED
    void Centruj();                    //!< Zmienia współrzędne tak, żeby były wokół środka ciężkości wieloboku.
    /// @}

    /// @name Biblioteka podstawowych kształtów w przestrzeni nazw klasy `Wielobok`
    /// ***************************************************************************
    /// @details
    /// Zrobione jako statyczne metody, bo to pozwala robić potem różne implementacje
    /// przechowywania tych wieloboków, np. tworzyć je dopiero wtedy, gdy będą potrzebne
    /// lub ściągać z dysku.
    /// Funkcje zwracają "stałe referencje" czyli można czytać, odrysować, ale nie zmieniać.
    /// "static" w odniesieniu do metody oznacza, że jest to składowa klasy jako takiej,
    /// a nie każdego obiektu tej klasy z osobna. Do wywołania obiekt nie jest potrzebny!
    /// @{

    /// Listowanie nazw wieloboków.
    /// \param pos - kolejna pozycja w bibliotece.
    MAYBE_UNUSED
    static const char*     NazwyWielobokow(int pozycja);       //!< \brief Zwraca nazwę i-tego wieloboku z biblioteki.
                                                               //!< \return Jak NULL to nie ma już więcej.

    MAYBE_UNUSED
    static const Wielobok& WielobokWgNazwy(const char* nazwa); //!< \brief Daje kształt z biblioteki. \return Jak nie ma właściwego to domyślny.

    MAYBE_UNUSED
    static const Wielobok* SprobujWielobok(const char* nazwa); //!< \brief Daje kształt z biblioteki. \return Jak nie ma to, zwraca NULL (TODO lub stara się załadować z pliku).

    MAYBE_UNUSED
    static const Wielobok& Domyslny();                         //!< \brief Daje kształt domyślny/zastępczy jak nie ma potrzebnego.

    // Skróty do częściej używanych wieloboków z biblioteki:
    //*/////////////////////////////////////////////////////
    MAYBE_UNUSED
    static const Wielobok& Namiot();            //!< Kształt namiotu. Skrótowy dostęp do obiektu z biblioteki kształtów.
    MAYBE_UNUSED
    static const Wielobok& Romb();              //!< Kształt rombu. Skrótowy dostęp do obiektu z biblioteki kształtów.
    MAYBE_UNUSED
    static const Wielobok& Domek();             //!< Kształt domu. Skrótowy dostęp do obiektu z biblioteki kształtów.
    MAYBE_UNUSED
    static const Wielobok& Ufo();               //!< Kształt UFO. Skrótowy dostęp do obiektu z biblioteki kształtów.
    MAYBE_UNUSED
    static const Wielobok& Ludzik(int typ);     //!< Kształt człowieka. Skrótowy dostęp do obiektu z biblioteki kształtów.

    /// @}
};


/// @}

/* ****************************************************************** */
/*              SYMSHELLLIGHT  version 2026                           */
/* ****************************************************************** */
/*             THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
#endif //SYMSHELL_WIELOBOKI_HPP_INCLUDED_

