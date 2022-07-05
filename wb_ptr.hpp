/// \file wb_ptr.hpp
/// \brief Proste szablony inteligentnych wskaźników oraz tablic dynamicznych.
/// \author borkowsk
// *******************************************************************************************************************
/// ZAWARTOŚĆ:
///	\n wb_sptr     : scalar only ptr
///	\n wb_ptr	    : struct/class ptr
///	\n wb_pchar    : ptr to char[]
///	\n wb_dynarray : dynamic 1D array of something
///	\n wb_dynmatrix: dynamic matrix of something
///
/// \details
/// Trochę na wzór wczesnego STL, ale inne i raczej mało kompatybilne.
/// Zawartość wskazywana jest "sztefetowana" pomiędzy obiektami powyższych typów,
/// co oznacza, że w konstruktorze kopiującym albo przypisaniu jest przenoszona,
/// a nie kopiowana. Obiekt donor staje się PUSTY! Dlatego obiekty te do funkcji
/// muszą być zawsze przekazywane przez referencję
///
/// \author Wojciech Borkowski wborkowski (_at_) uw.edu.pl
// *******************************************************************************************************************
#ifndef __WB_PTR_HPP__
#define __WB_PTR_HPP__

#ifndef HIDE_WB_PTR_IO
#define HIDE_WB_PTR_IO 0
#endif

#ifndef DEBUG_WB_PTR
#define DEBUG_WB_PTR 0
#endif

#if DEBUG_WB_PTR == 1
#define WBPTRLOG( _P_ )  { std::cerr<<((void*)this)<<"->"<< (_P_) <<'\n';}
#else
#define WBPTRLOG( _P_ )  {}
#endif

#include <stdarg.h>			//Jest konstruktor z nieznana liczba parametrow
#include <assert.h>

#include <iostream>
using namespace std;

#include "wb_clone.hpp"

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// Szablon inteligentnego wskaźnika dla typów skalarnych
template<class T>
class wb_sptr
{
protected:
    T* ptr; ///< Właściwa zawartość inteligentnego wskaźnika

public:
    /// Konstruktor pobierający surowy wskaźnik pod zarząd
	explicit wb_sptr(T* ini=NULL):ptr(ini){}

    /// Konstruktor sztafetujący
	wb_sptr(wb_sptr& nini):ptr(nini.give()){}

    /// Destruktor zwalniający zawartość jeśli jest
	~wb_sptr(){ dispose(); }

    ///Metoda czyszcząca zawartość równoważna destruktorowi
    void dispose()
	{
	if(ptr)
		{
		WBPTRLOG( "wb_sptr::dispose :"<<ptr )
		delete ptr;
		}
	ptr=NULL;
	}

    ///Operator przekazania surowego wskaźnika pod zarząd
    wb_sptr& operator = (T* nini)
    {
    dispose();
    ptr=nini;
    return *this;
    }

    ///Metoda jawnego przekazania surowego wskaźnika pod zarząd
    void take(T* nini)
	{
	dispose();
	ptr=nini;
	}

    ///Metoda jawnego przekazania surowego wskaźnika pod zarząd
    /// \details  Pozwalająca dołączyć kolejne wywołanie dzięki zwracaniu *this
    wb_sptr& set(T* nini)
    {
    dispose();
    ptr=nini;
    return *this;
    }

    ///Operator przypisania inteligentnych wskaźników. \warning Jest sztafetujący. Oddaje zawartość na L-value
    wb_sptr<T>& operator = (wb_sptr<T>& nini)
	{
	dispose();
	ptr=nini.give();
	return *this;
	}

    ///Jawna metoda naśladująca sztafetujący operator przypisania
    /// \details  Pozwalająca dołączyć kolejne wywołanie dzięki zwracaniu *this
    wb_sptr<T>& transfer_from(wb_sptr<T>& nini)
	{
	dispose();
	ptr=nini.give();
	return *this;
	}

    ///Operator dostępu do zawartości
    T& operator* () const
    {                                                                                                 assert(ptr!=NULL);
        return *ptr;
    }

    ///Konwersja sprawdzająca na int - do warunków (POWINNO BYĆ bool! TODO!)
    operator int () const
	{
	return ptr!=NULL;
	}

    ///Jawne sprawdzenie czy coś zawiera (POWINNO BYĆ bool! TODO!)
    int OK() const
	{
	return ptr!=NULL;
	}

    ///Metoda odczytu surowej zawartości - UŻYWAĆ OSTROŻNIE!
    ///\details Nie sprawdzamy czy NULL - niech sie martwi wołający metodę.
    T* get_ptr_val() const
	{
	return ptr;
	}

    ///Metoda zabiera surowy wskaźnik spod zarządu inteligentnego wskaźnika!
    ///\details O dealokację musi się już martwić ktoś inny.
    T*  give()
	{
	T* pom=ptr;
	ptr=NULL;
	return pom;
	}

    ///Metoda niemal równoważna destruktorowi \warning OBSOLETE
    void finalise(){ dispose(); }

};

///Szablon inteligentnego wskaźnika dla dla typów strukturalnych
template<class T>
class wb_ptr:public wb_sptr<T>
{
    /// WYŁĄCZONY konstruktor klonujący, bo nie ma zastosowania do klas abstrakcyjnych (??? TODO TEST ???)
    wb_ptr(const wb_ptr<T>& nini):wb_sptr<T>(clone(nini.get_ptr_val()))
    {
    		assert("DO NOT USE wb_ptr(const wb_ptr<T>&)"==NULL);
    }

    /// WYŁĄCZONY Operator przypisania dla const. Czy można stosować z klasami abstrakcyjnymi (??? TODO TEST ???)
    wb_ptr& operator = (const wb_ptr<T>& nini)
    {
           assert("DO NOT USE wb_ptr& operator = (const wb_ptr<T>&)"==NULL);
    }
public:
    //Konstruktory są dokładnie takie same jak w klasie bazowej. (Może zatem zbędne? TODO?)

	/// Konstruktor kopiujący jest sztafetujący!
    wb_ptr(wb_ptr& nini);

    /// Konstruowanie ze wskaźnika utworzonego na stercie!
    explicit wb_ptr(T* ini=NULL);

    /// Destruktor zwalniający zawartość jeśli jakaś jest
    ~wb_ptr(){ wb_sptr<T>::dispose(); }

    /// Metoda jawnego przekazania surowego wskaźnika pod zarząd, ale pozwalająca dołączyć kolejne wywołanie
    wb_ptr& set(T* nini);

    /// Przypisania są dokładnie takie same jak w klasie bazowej. (Może zatem zbędne? TODO TEST?)

    /// Operator przypisania wskaźnika na stercie
    wb_ptr& operator = (T* nini);

    /// Sztafetujący operator przypisania
    wb_ptr& operator = (wb_ptr<T>& nini);

    ///Jawna metoda naśladująca sztafetujący operator przypisania, ale pozwalająca dołączyć kolejne wywołanie
    wb_ptr& transfer_from(wb_ptr<T>& nini) ;

    ///Dodadany operator -> dający dostęp do wskaźnika do klasy/struktury
    T* operator -> () const;
};

template<class T>
wb_ptr<T>::wb_ptr(T* ini):wb_sptr<T>(ini)
	{
        WBPTRLOG( "wb_ptr::EXPLICIT FROM PTR CONSTRUCTOR :"<< wb_sptr<T>::ptr );
    }

template<class T>
wb_ptr<T>::wb_ptr(wb_ptr& nini):wb_sptr<T>(nini)
	{
		WBPTRLOG( "wb_ptr::TRANSFER CONSTRUCTOR :"<< wb_sptr<T>::ptr );
	}

template<class T>
wb_ptr<T>& wb_ptr<T>::operator = (T* nini)
	{
	return set(nini);
	}

// Dodadany operator -> dla wskaźnika do klasy/struktury
template<class T>
T* wb_ptr<T>::operator -> () const
	{                                                                                 assert	(wb_sptr<T>::ptr!=NULL);
	return wb_sptr<T>::ptr;
	}

template<class T>
wb_ptr<T>& wb_ptr<T>::set(T* nini)
	{
	wb_sptr<T>::dispose();
	wb_sptr<T>::ptr=nini;
	return *this;
	}

//nie sptr bo wtedy byłoby nieuprawnione rozszerzenie operacji (TODO CHECK)
template<class T>
wb_ptr<T>& wb_ptr<T>::operator = (wb_ptr<T>& nini)
	{
	WBPTRLOG( "wb_ptr::TRANSFER oper = :"<<nini.ptr<<"->"<<wb_sptr<T>::ptr );
	wb_sptr<T>::dispose();
	wb_sptr<T>::ptr=nini.give();
	return *this;
	}

template<class T>
wb_ptr<T>& wb_ptr<T>::transfer_from(wb_ptr<T>& nini) //Jawnie nazwany operator przypisania
	{
	wb_sptr<T>::dispose();
	wb_sptr<T>::ptr=nini.give();
	return *this;
	}


///\brief Inteligentny wskaźnik na tablicę znaków zakończonych \0
///\details Klasa z rodziny inteligentnych wskaźników przeznaczona na uchwyt do łańcucha tekstowego
///Pomiędzy obiektami klasy wb_pchar bez atrybutu const zawartość jest, jak zwykle, "sztafetowana",
///więc jako parametr funkcji i metod musi być przekazywany przez referencję.
///Nie przechowuje długości łańcucha, wiec też jej nie sprawdza (zazwyczaj?)
///Z char* const char* kopiuje oczekując ze kończą się '\0' .
class wb_pchar:public wb_sptr<char>
{
public:
    ///Konstruktor alokujący (chyba że s==0, wtedy nie)
    explicit wb_pchar(size_t size=0):wb_sptr<char>(NULL)
	{
        if(size>0) ptr=new char[size];
        if(ptr!=NULL) *ptr='\0';
	}

    ///Konstruktor do inicjalizowania z const char*
    ///Ma klonowanie danych, bo stałe łańcuchy ("") nie są alokowane na stercie!
    explicit wb_pchar(const char* nini):wb_sptr<char>(NULL)
	{
        WBPTRLOG( "wb_pchar::FROM char* CONSTRUCTOR :"<<(ptr?ptr:"@") )
        if(nini!=NULL) ptr=clone_str(nini);
	}

    ///Konstruktor sztafetujący - głównie do niejawnego kopiowania przy wyjściu z funkcji
    ///Inicjator traci swoje dane. Jak nie to znaczy ze zaszło niezamierzone kopiowanie
    wb_pchar(wb_pchar& nini):wb_sptr<char>(nini)
	{
	    WBPTRLOG( "wb_pchar::TRANSFER CONSTRUCTOR :"<<(ptr?ptr:"@") )                           assert(nini.ptr==NULL);
	}

    ///Konstruktor kopiujący tej klasy ograniczony do sytuacji gdy inicjator jest stały
    wb_pchar(const wb_pchar& nini):wb_sptr<char>(NULL)
    {
        WBPTRLOG( "wb_pchar::COPY CONSTRUCTOR :"<<(nini.ptr?nini.ptr:"@") )
        ptr=clone_str(nini.get_ptr_val());//Kopiowanie danych
    }

    ///Przypisanie stałej tablicy znaków (const char*) zmienia zawartość wb_pchar na nowy klon danych
    ///Przypisanie NULL wymusza tylko dealokacje
    wb_pchar& operator = (const char* nini)
	{
        WBPTRLOG( "wb_pchar::oper = (const char*) :"<<(nini?nini:"@")<<"->"<<(ptr?ptr:"@") )
        dispose();
        if(nini!=NULL) ptr=clone_str(nini);
        else ptr=NULL;
        return *this;
	}

    ///Sztafetujący operator przypisania
    wb_pchar& operator = (wb_pchar& nini)
	{
        WBPTRLOG( "wb_pchar::TRANSFER oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
        dispose();
        ptr=nini.give();
        return *this;
	}

    ///Klonujący operator przypisania na wypadek przypisywania od stałego źródła
    wb_pchar& operator = (const wb_pchar& nini)
	{
        WBPTRLOG( "wb_pchar::CLONE oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
        dispose();
        ptr=clone_str(nini.get_ptr_val());
        return *this;
	}

    ///Operator indeksowania dający dostęp do pojedynczych znaków
    ///Pozwala na zmianę znaku na danej pozycji, ale nie wskaźnika do niego!
    char& operator [] (size_t index) const
    {                                                                                               assert(ptr!=NULL);
        return ptr[index];
    }

    ///Metoda zmieniająca długość zaalokowanego łańcucha
    ///Stare dane są tracone
    void alloc(size_t s)
	{
        dispose();
        if(s>0)
        {
            ptr=new char[s];                                                                        assert(ptr!=NULL);
            *ptr='\0';
        }
	}

    ///Metoda oblicza rozmiar łańcucha i go zwraca. Gdy nie ma łańcucha zwraca 0.
    size_t get_size() const
	{
		return ptr?(::strlen(ptr)):(0);
	}

    ///Alias do get_ptr_val
    ///Nie jest sprawdzane czy nie NULL - niech sie martwi wołający
    const char* get() const
	{
	    return get_ptr_val();
	}

    ///Metoda wyprowadza formatowane dane na zawartość obiektu wb_pchar. \note  Nie sprawdza rozmiaru!!!
    wb_pchar& prn(const char* format,...);

    ///Metoda dopisuje do zawartości. \note Nie sprawdza rozmiaru!!!
    wb_pchar& add(const char* format,...);

    ///Implementacja zapisu na strumień ze sprawdzaniem czy trzeba zamknąć w cudzysłowy '\"' lub inne.
    static void write(ostream& s,const char* p,char enclos='\"');

    /// Zamienia wszystkie łańcuchy 'forrep' zawarte w obiekcie wb_pchar
    /// na łańcuchy 'whatins'. \note Bufor jest sztafetowany!!!
    /// Jest "case sensitive".
    friend
    bool replace(wb_pchar& bufor,const char* forrep,const char* whatins,bool fullwords,unsigned startpos/*=0*/);

    /// Wstawia łańcuch tekstowy do bufora na określonej pozycji. \note Bufor jest sztafetowany!!!
    friend
    bool insert(wb_pchar& bufor,unsigned pos,const char* whatins);

    /// wb_pchar odpowiednik 'strlen' dla wskaźników na znak
    friend
    size_t strlen(const wb_pchar& what)
    {
        return ::strlen(what.get());
    }

    /// wb_pchar odpowiednik 'strcmp' dla wskaźników na znak
    friend
    size_t strcmp(const wb_pchar& f,const wb_pchar& s)
    {
        return ::strcmp(f.get(),s.get());
    }

    /// wb_pchar odpowiednik 'strcmp' dla wskaźników na znak - różne parametry
    friend
    size_t strcmp(const char* f,const wb_pchar& s)
    {
        return ::strcmp(f,s.get());
    }

    /// wb_pchar odpowiednik 'strcmp' dla wskaźników na znak - różne parametry i w odwrotnej kolejności
    friend
    size_t strcmp(const wb_pchar& f,const char* s)
    {
        return ::strcmp(f.get(),s);
    }

    /// wb_pchar odpowiednik 'strchr' dla wskaźników na znak
    friend
    const char* strchr(const wb_pchar& what,const char c)
    {
        return ::strchr(what.get(),c);
    }

    /// wb_pchar odpowiednik 'strstr' dla wskaźników na znak
    friend
    const char* strstr(const wb_pchar& what,const char* s)
    {
        return ::strstr(what.get(),s);
    }

    /// wb_pchar odpowiednik 'strstr' dla wskaźników na znak
    friend
    const char* strstr(const wb_pchar& what,const wb_pchar& s)
    {
        return ::strstr(what.get(),s.get());
    }

};

///Szablon bardzo prostej tablicy o rozmiarze dynamicznym
///i możliwym testowaniu indeksów przy odwołaniach (assercja!!!)
template<class T>
class wb_dynarray
{
    T* ptr;                    ///< Właściwa zawartość inteligentnego wskaźnika
    size_t size;               ///< oraz jego rozmiar

    //konstruktor transferujacy. Niestety uniemozliwia niektore dziwne konstrukcje, bo jakos "zaslania" ten z "const wb_dynarray&"
    //wb_dynarray(wb_dynarray& nini):size(nini.size),ptr(nini.ptr)
    //	{
    //	WBPTRLOG( "wb_dynarray::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" )
    //	nini.ptr=NULL;
    //	nini.size=0;
    //	}

public:
	///Konstruktor alokujący \details akceptuje też 0, ale wtedy nic nie alokuje
	explicit wb_dynarray(size_t s=0):size(s)
		{
		WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<')' )
		if(size>0)	ptr=new T[s];
			else ptr=NULL;
		}

	///Konstruktor "kopiujący" - musi byc forsowany z const wiec jest niebezpieczny
	//explicit? (TODO TEST!)
	wb_dynarray(const wb_dynarray& nini/*,bool copy=false*/):size(nini.size),ptr(nini.ptr)
		{
		WBPTRLOG( "wb_dynarray::COPY CONSTRUCTOR("<<((void*)&nini)<<")" )                               assert(size!=0);
		const_cast<wb_dynarray<T>&>(nini).ptr=NULL;
		const_cast<wb_dynarray<T>&>(nini).size=0;
		}

	///Konstruktor wieloparametrowy inicjujący itemy
	explicit wb_dynarray(size_t s,T /*first,second,...*/...):size(s)
    {
		WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<",T ...)" )                                        assert(size>=1);
		ptr=new T[s];                                                             /*After allocation*/assert(ptr!=NULL);

		va_list list;
		va_start(list,s);
		for(size_t i=0;i<s;i++)
			ptr[i]=va_arg(list,T);
		va_end(list);
    }

	/// Destruktor zwalnia pamięć za pomocą metody dispose()
	~wb_dynarray()
		{
		WBPTRLOG( "wb_dynarray::DESTRUCTOR" )
		dispose();
		}

    /// Metoda sprawdza czy tablica została zaalokowana
    int OK() const
        {
        return ptr!=NULL;
        }

    /// Metoda sprawdza czy tablica została zaalokowana. \warning OBSOLETE
    int IsOK() const
        {
        return ptr!=NULL;
        }

    /// Transferujacy operator przypisania
    wb_dynarray<T>& operator = (wb_dynarray<T>& nini)
    {
        WBPTRLOG( "wb_dynnarray::operator = (wb_dynarray& "<<((void*)&nini)<<")" )
        this->take(nini);
        return *this;
    }

    ///Metoda alokacji surowego wektora \warning Jak 's' ma być 0 to użyj "dispose"!
    size_t alloc(size_t s)
	{
        WBPTRLOG( "wb_dynnarray::alloc("<<s<<")" )                                                          assert(s>0);
        dispose();
        if(s>0)
        {
            ptr = new T[s];
            if (ptr == NULL)
                return 0;
        }
        size=s;
        return s;
	}

    ///Skrócenie wektora - przydatne przy czytaniu z pliku \warning Jak 's' ma być 0 to użyj "dispose"!
    size_t trunc(size_t s)
    {
        WBPTRLOG( "wb_dynnarray::trunc("<<s<<")" )                                                          assert(s>0);
        if(ptr==NULL) //Bo wtedy nie ma czego skracać
        {
            return 0;
        }
        if(s>0)
        {
            T* tmp=new T[s];
            memcpy(tmp,ptr,s*sizeof(T));
            dispose();  //Kasacja starego wektora
            ptr=tmp;    //Zapamiętanie nowego wektora
            size=s;
        } else dispose();
        return s;
    }

    ///Dealokacja wektora
    void dispose()
	{
        WBPTRLOG( "wb_dynarray::dispose() :"<<size )
#ifdef USES_NONSTD_ALLOCATORS
        if(ptr) delete [size]ptr;
#else
        if(ptr) delete []ptr;
#endif
        ptr=NULL;
        size=0;
	}

    ///Operator indeksowania - dostęp do pojedynczego itemu
    T& operator [] (size_t index) const
	{
#ifndef _NDEBUG
	if(ptr==NULL || index>=size)
	{   //When DEBUG! Poniżej postaw breakpoint!
		cerr<<"Invalid use of wb_dynarray "<<this<<" tabptr:"<<ptr<<"index:"<<index<<" size:"<<size<<endl;
                                                                                                        assert(ptr!=NULL);
                                                                                                        assert(index<size);
	}
#endif
	return ptr[index];
	}

    ///Zwraca rozmiar wewnętrznego zaalokowanego wektora (ewentualnie 0)
    size_t get_size() const
	{ return size;}

    /// Wymusza umieszczenie dynamicznie alokowanego
    /// wektora jako tablicy wewnątrz dynarray
    void take(T* iptr,size_t isiz)
    {
        dispose();
        ptr=iptr;
        if(ptr!=NULL)
            size=isiz;
        else
            size=0;
    }

    /// Pobiera wewnętrzny wektor od donora 'nini' i umieszcza na tym obiekcie (this)
    void take(wb_dynarray& nini)
    {
        if(size>0)
            dispose();
        ptr=nini.ptr;
        size=nini.size;
        nini.ptr=NULL;
        nini.size=0;
    }

    /// Wymusza umieszczenie dynamicznie alokowanego
    /// wektora jako tablicy wewnątrz dynarray i pozwala wykonać na wyniku
    /// jakąś inną operację albo od razu przekazać jako parametr funkcji
    wb_dynarray& set_dynamic_ptr_val(T* iptr,size_t isiz)
	{
	dispose();
	ptr=iptr;
	if(ptr!=NULL)
		size=isiz;
		else
		size=0;
	return *this;
	}

    ///Daje bezpośredni dostęp do wewnętrznego wektora
    ///ale nie wolno go zdealokować!!!
    ///Jak życzymy sobie coś poza tablicą to zwraca NULL
    T* get_ptr_val(size_t offset=0,size_t interest=0) const
    {
        if(ptr==NULL
           || offset>=size
           || offset+interest>=size+1
                )
            return NULL;
        return ptr+offset;
    }

    ///Zabiera wewnętrzny raw ptr spod zarządu tablicy!!!
    ///Pobiera też informacje o aktualnym rozmiarze
    T*  give_dynamic_ptr_val(size_t& outsize)
    {
        T* pom=ptr;
        outsize=size;
        //kasowanie
        ptr=NULL;size=0;
        return pom;
    }

    ///Wypełnia tablicę zadanym elementem/wartością
    void fill(const T& Val)
    {
        size_t i,H=get_size();
        for(i=0;i<H;i++)//Po elementach
        {
            (*this)[i]=Val;//Wypełnij ten element
        }
    }

    ///Przesuniecie pojedynczego elementu tablicy na koniec
    void shift_left(size_t index)
    {
        if(index>=size-1) return;//Wyjątkowo nic nie trzeba robić
        char bufor[sizeof(T)];                                                                        assert(ptr!=NULL);
                                                                                                      assert(index<size);
        memcpy(bufor,ptr+index,sizeof(T));/* przemieszczamy memcpy, żeby nie używać przypisania */

        size_t ile=size-index-1;
        memcpy(ptr+index,ptr+(index+1),sizeof(T)*ile);
        memcpy(ptr+(size-1),bufor,sizeof(T));
    }

    ///Brutalne kopiowanie z surowej tablicy.
    ///\warning W przypadku obiektów z nietrywialnymi destruktorami nie może skończyć się dobrze
    void raw_copy_from(const T Where[],size_t how_many_elements)
    {                                                                                   assert(how_many_elements<=size);
        memcpy(ptr,Where,how_many_elements*sizeof(T));
    }

    ///Brutalne kopiowanie z tablicy dynamicznej.
    ///W przypadku obiektów z nietrywialnymi destruktorami nie może skończyć się dobrze!
    void raw_copy_from(const wb_dynarray<T>& Where)
    {                                                                                   assert(Where.get_size()<=size);
        memcpy(ptr,Where.get_ptr_val(),Where.get_size()*sizeof(T));
    }

};

/// Szablon prostej tablicy dwuwymiarowej o dowolnej liczbie wierszy i dowolnej długości każdego wiersza.
/// \details Kontrole zakresów itp. assercje dziedziczy po klasie bazowej
template<class T>
class wb_dynmatrix:public wb_dynarray< wb_dynarray<T> >
{
public:
    /// Konstruktor alokujący o ile nie ma żadnego zera w parametrach
	explicit wb_dynmatrix(size_t y=0,size_t x=0):wb_dynarray< wb_dynarray<T> >(y)
    {
        WBPTRLOG( "wb_dynmatrix::CONSTRUCTOR("<<y<<','<<x<<')' );

        if(y>0 && x>0)
            for(size_t Y=0;Y<y;Y++)
                (*this)[Y].alloc(x);
    }

    /// Konstruktor inicjujący listą wskaźników - DZIWNY I CHYBA NIE PRZETESTOWANY. TODO?
	explicit wb_dynmatrix(size_t s,wb_dynarray<T>* ...):wb_dynarray<wb_dynarray<T> >(s)
    {
        WBPTRLOG( "wb_dynmatrix::CONSTRUCTOR("<<get_size()<<",wb_dynarray<T>* ...)" );
        assert( wb_sptr<T>::get_size()>=1 );

        va_list list;
        va_start(list,s);
        for(size_t i=0;i<s;i++)
            (*this)[i]=*(va_arg(list,wb_dynarray<T>*));//Czy to wskaźnik, czy referencja to rybka

        va_end(list);
    }

    ///KONSTRUKTOR KOPIUJĄCY - tu SZTAFETUJĄCY raczej
    wb_dynmatrix(wb_dynmatrix& nini):wb_dynarray< wb_dynarray<T> >(nini)
    {
        WBPTRLOG( "wb_dynmatrix::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" );
    }

	~wb_dynmatrix()
    {
        WBPTRLOG( "wb_dynmatrix::DESTRUCTOR" );
        dispose();
    }

    /// Dealokacja wektora wektorów.
    /// Metoda chyba potrzebna tylko do debugowania bo całą robotę robi metoda klasy bazowej
    void dispose()
    {
        WBPTRLOG( "wb_dynmatrix::dispose :"<<get_size() );
        wb_dynarray< wb_dynarray<T> >::dispose();
    }

    /// Rozbudowana alokacja zstępująca
    size_t alloc(size_t y,size_t x);

    /// Wypełnianie zadaną wartością
    void fill(const T& Val);

};

/// Rozbudowana alokacja zstępująca
template<class T> inline
size_t wb_dynmatrix<T>::alloc(size_t y,size_t x)
{
    WBPTRLOG( "wb_dynmatrix::alloc("<<y<<','<<x<<")" );                                                     assert(y>0);
    if(this->get_size()>0)
        dispose();

    if(wb_dynarray<wb_dynarray<T>>::alloc(y)==0)
                return 0;

    if(y>0 && x>0)
            for(size_t Y=0;Y<y;Y++)
                {
                if((*this)[Y].alloc(x)==0)
                        return 0;
                }

    return this->get_size();
}

/// Metoda wypełnianie macierzy dynamicznej zadaną wartością
template<class T> inline
void wb_dynmatrix<T>::fill(const T& Val)
{
    size_t i,H=this->get_size();
    for(i=0;i<H;i++)//Po wierszach
        {
        size_t L=(*this)[i].get_size();
        for(size_t j=0;j<L;j++)//Po elementach wiersza
            (*this)[i][j]=Val;//Wypełnij ten element!
        }
}

/// Funkcja wypełnianie tablicy dynamicznej zadaną wartością
template<class T> inline
void fill(wb_dynarray<T>& Tab,const T& Val)
{
    Tab.fill(Val);
}

/// Funkcja wypełnianie macierzy dynamicznej zadaną wartością
template<class T> inline
void fill(wb_dynmatrix<T>& Mat,const T& Val)
{
    Mat.fill(Val);
}

#if	HIDE_WB_PTR_IO != 1
/// Wejście wyjście strumieniowe dla inteligentnych wskaźników.
/// Tutaj tylko deklaracje - implementacja musi byc w innym pliku

ostream& operator<<(ostream&,const wb_sptr<char>&); ///< Czy to gdzieś jest zaimplementowane? TODO?
istream& operator>>(istream&,wb_sptr<char>&); ///< Czy to gdzieś jest zaimplementowane? TODO?
ostream& operator<<(ostream&,const wb_pchar&); ///< Czy to gdzieś jest zaimplementowane? TODO?
istream& operator>>(istream&,wb_pchar&); ///< Czy to gdzieś jest zaimplementowane? TODO?

template<class T>
    ostream& operator<<(ostream&,const wb_sptr<T>&); ///< Czy to gdzieś jest zaimplementowane? TODO?

template<class T>
    istream& operator>>(istream&,wb_sptr<T>&); ///< Czy to gdzieś jest zaimplementowane? TODO?

template<class T>
    ostream& operator<<(ostream&,const wb_dynarray<T>&); ///< Czy to gdzieś jest zaimplementowane? TODO?

template<class T>
    istream& operator>>(istream&,wb_dynarray<T>&); ///< Czy to gdzieś jest zaimplementowane? TODO?

template<class T>
    ostream& operator<<(ostream&,const wb_dynmatrix<T>&); ///< Czy to gdzieś jest zaimplementowane? TODO?

template<class T>
    istream& operator>>(istream&,wb_dynmatrix<T>&); ///< Czy to gdzieś jest zaimplementowane? TODO?
#endif

} //namespace wbrtm
/* ******************************************************************/
/*              SYMSHELLLIGHT  version 2022-07-04                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif
