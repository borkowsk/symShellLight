/// \file wb_ptr.hpp
/// \brief Simple templates for smart pointers and dynamic arrays.
/// @date 2026-04-30 (modified)
///
/// \details
/// CONTENTS:
///	    - wb_sptr		: smart pointer for scalars.
///	    - wb_ptr		: smart pointer for structures/classes.
///	    - wb_pchar		: smart pointer for character arrays on the heap (char[]).
///	    - wb_dynarray	: dynamic array of anything.
///	    - wb_dynmatrix	: dynamic matrix of anything.
///
/// Somewhat modeled after early STL, but different and with very limited compatibility.
/// The pointed content is "passed like a baton" between objects of the above types,
/// which means it is moved, not copied, in the copy constructor or assignment.
/// The donor object becomes EMPTY! Therefore, these objects must always be passed
/// to functions by REFERENCE!
///
/// \copyright Wojciech Borkowski wborkowski (_at_) uw.edu.pl
/// \author    borkowsk
/// @note If you're using the `SymShellLight` library with the full version of WB_RTM, make sure this file and its version
///       in the full library are compatible. Ideally, arrange the include paths so that you only use the full version.
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#include <cstdarg>			//Jest konstruktor z nieznana liczba parametrow
#include <cassert>
#include <iostream>

using namespace std;

#include "wb_clone.hpp"
#include "maybe_unused.h"

namespace wbrtm {

    using std::ostream;

/**
 * @defgroup DynMemoryEN Heap data management
 * \brief Smart pointers, simple dynamic data structures (arrays and matrices), cloning, etc.
 */
/// @{

// ////////////////////////////////////////////////////////////////////////////////
// SMART POINTERS
// ////////////////////////////////////////////////////////////////////////////////

    /// \brief Smart pointer template for scalar types \ingroup DynMemory
    template<class T>
    class wb_sptr
    {
    protected:
        T* ptr; ///< The actual content of the smart pointer.

    public:
        /// \brief Constructor taking a raw pointer under management.
        explicit wb_sptr(T* ini=NULL):ptr(ini){}

        /// \brief "Baton-passing" (transferring) constructor.
        wb_sptr(wb_sptr& nini):ptr(nini.give()){}

        /// \brief Destructor that releases the content, if it exists.
        ~wb_sptr(){ dispose(); }

        /// \brief Method for clearing content, equivalent to the destructor.
        void dispose()
        {
            if(ptr)
            {
                WBPTRLOG( "wb_sptr::dispose :"<<ptr );
                delete ptr;
            }
            ptr=NULL;
        }

        /// \brief Operator for taking a raw pointer under management.
        wb_sptr& operator = (T* nini)
        {
            dispose();
            ptr=nini;
            return *this;
        }

        /// \brief Explicit method for taking a raw pointer under management.
        void take(T* nini)
        {
            dispose();
            ptr=nini;
        }

        /// \brief Explicit method for taking a raw pointer under management.
        /// \details Allows chaining subsequent calls by returning *this.
        wb_sptr& set(T* nini)
        {
            dispose();
            ptr=nini;
            return *this;
        }

        /// \brief Assignment operator for smart pointers. \warning It is "baton-passing". Transfers content to the L-value.
        wb_sptr<T>& operator = (wb_sptr<T>& nini)
        {
            dispose();
            ptr=nini.give();
            return *this;
        }

        /// \brief Explicit method mimicking the baton-passing assignment operator.
        /// \details Allows chaining subsequent calls by returning *this.
        wb_sptr<T>& transfer_from(wb_sptr<T>& nini)
        {
            dispose();
            ptr=nini.give();
            return *this;
        }

        /// \brief Content access operator.
        T& operator* () const
        {                                                                                                 assert(ptr!=NULL);
            return *ptr;
        }

        /// \brief Conversion to `int` for checks — for conditions (SHOULD BE bool! TODO!).
        operator int () const
        {
            return ptr!=NULL;
        }

        /// \brief Explicit check if it contains anything (SHOULD BE bool! TODO!).
        int OK() const
        {
            return ptr!=NULL;
        }

        /// \brief Method for reading the raw pointer value. @note USE WITH CAUTION!
        /// \details Does not check for NULL — let the caller worry about it.
        T* get_ptr_val() const
        {
            return ptr;
        }

        /// \brief Method that takes the raw pointer away from the smart pointer's management.
        /// \details Someone else must now worry about deallocation.
        T* give()
        {
            T* pom=ptr;
            ptr=NULL;
            return pom;
        }

        /// \brief Method almost equivalent to the destructor. \warning OBSOLETE
        void finalise(){ dispose(); }

    };

    /// \brief Smart pointer template for structural types. \ingroup DynMemory
    template<class T>
    class wb_ptr:public wb_sptr<T>
    {
        /// \brief Cloning constructor. Does not apply to abstract classes (??? TODO TEST ???).
        wb_ptr(const wb_ptr<T>& nini):wb_sptr<T>(clone(nini.get_ptr_val()))
        {
            assert("DO NOT USE wb_ptr(const wb_ptr<T>&)"==NULL);
        }

        /// \brief DISABLED Assignment operator for const. Can it be used with abstract classes (??? TODO TEST ???)
        wb_ptr& operator = (const wb_ptr<T>& nini)
        {
            assert("DO NOT USE wb_ptr& operator = (const wb_ptr<T>&)"==NULL);
        }
    public:
        // Constructors are exactly the same as in the base class. (could be imported):
        // ///////////////////////////////////////////////////////////////////////////////////

        /// \brief Copy constructor is "baton-passing" (transferring ownership)!
        wb_ptr(wb_ptr& nini);

        /// \brief Construction from a pointer created on the heap.
        explicit wb_ptr(T* ini=NULL);

        /// \brief Destructor releasing content, if any exists.
        ~wb_ptr(){ wb_sptr<T>::dispose(); }

        /// \brief Explicit method for taking a raw pointer under management, allowing call chaining.
        /// ` wb_ptr& set(T* nini);`.
        using wb_sptr<T>::set;

        /// \brief Assignment operator for a pointer on the heap. `wb_ptr& operator = (T* nini);`
        using wb_sptr<T>::operator = ; // Same as in the base class, so it could be imported.

        /// \brief "Baton-passing" (transferring) assignment operator.
        wb_ptr& operator = (wb_ptr<T>& nini);

        /// \brief Explicit method mimicking the baton-passing assignment operator, allowing call chaining.
        MAYBE_UNUSED
        wb_ptr& transfer_from(wb_ptr<T>& nini) ;

        /// \brief Added -> operator providing access to the class/structure pointer.
        T* operator -> () const;
    };

// INLINE IMPLEMENTATIONS:
// ///////////////////////

    template<class T> MAYBE_UNUSED
    wb_ptr<T>::wb_ptr(T* ini):wb_sptr<T>(ini)
    {
        WBPTRLOG( "wb_ptr::EXPLICIT FROM PTR CONSTRUCTOR :"<< wb_sptr<T>::ptr );
    }

    template<class T> MAYBE_UNUSED
    wb_ptr<T>::wb_ptr(wb_ptr& nini):wb_sptr<T>(nini)
    {
        WBPTRLOG( "wb_ptr::TRANSFER CONSTRUCTOR :"<< wb_sptr<T>::ptr );
    }

//template<class T> MAYBE_UNUSED
//wb_ptr<T>& wb_ptr<T>::operator = (T* nini)
//    {
//    return set(nini);
//    }

    template<class T>
    T* wb_ptr<T>::operator -> () const
    {                                                                                 assert    (wb_sptr<T>::ptr!=NULL);
        return wb_sptr<T>::ptr;
    }

//template<class T>
//wb_ptr<T>& wb_ptr<T>::set(T* nini)
//    {
//    wb_sptr<T>::dispose();
//    wb_sptr<T>::ptr=nini;
//    return *this;
//    }

    //not `sptr` because it would be an unauthorized extension of the operation (TODO CHECK)
    template<class T>
    wb_ptr<T>& wb_ptr<T>::operator = (wb_ptr<T>& nini)
    {
        WBPTRLOG( "wb_ptr::TRANSFER oper = :"<<nini.ptr<<"->"<<wb_sptr<T>::ptr );
        wb_sptr<T>::dispose();
        wb_sptr<T>::ptr=nini.give();
        return *this;
    }

    template<class T> MAYBE_UNUSED
    wb_ptr<T>& wb_ptr<T>::transfer_from(wb_ptr<T>& nini) //Explicitly named assignment "operator"
    {
        wb_sptr<T>::dispose();
        wb_sptr<T>::ptr=nini.give();
        return *this;
    }


// ////////////////////////////////////////////////////////////////////////////////
// DYNAMIC STRUCTURES
// ////////////////////////////////////////////////////////////////////////////////

/// \brief Smart pointer for a \0 terminated character array \ingroup DynMemory
/// \details A class from the smart pointer family designed as a handle for a text string.
///          Between `wb_pchar` objects without the const attribute, the content is, as usual, "baton-passed" (transferred),
///          so it must be passed by reference as a parameter to functions and methods.
///          It does not store the string length, so it also does not check it (usually?).
///          It copies from `char*` and `const char*`, expecting them to end with a '\0' character.
    class wb_pchar:public wb_sptr<char>
    {
    public:
        /// \brief Allocating constructor (unless size==0, then no allocation occurs).
        explicit wb_pchar(size_t size=0):wb_sptr<char>(NULL)
        {
            if(size>0) ptr=new char[size];
            if(ptr!=NULL) *ptr='\0';
        }

        /// \brief Constructor for initializing from `const char*`.
        /// \warning It performs data cloning because string constants ("") are not allocated on the heap!
        explicit wb_pchar(const char* nini):wb_sptr<char>(NULL)
        {
            WBPTRLOG( "wb_pchar::FROM char* CONSTRUCTOR :"<<(ptr?ptr:"@") );
            if(nini!=NULL) ptr=clone_str(nini);
        }

        /// \brief Baton-passing (transferring) constructor, mainly for implicit copying when returning from a function.
        /// \warning The initiator loses its data. If not, it means an unintended copy occurred!
        wb_pchar(wb_pchar& nini):wb_sptr<char>(nini)
        {
            WBPTRLOG( "wb_pchar::TRANSFER CONSTRUCTOR :"<<(ptr?ptr:"@") )                           assert(nini.ptr==NULL);
        }

        /// \brief Copy constructor of this class. Since the initiator is formally constant, the content is cloned.
        wb_pchar(const wb_pchar& nini):wb_sptr<char>(NULL)
        {
            WBPTRLOG( "wb_pchar::COPY CONSTRUCTOR :"<<(nini.ptr?nini.ptr:"@") );
            ptr=clone_str(nini.get_ptr_val());//Data copying
        }

        /// \brief Assigning a constant character array (const char*) changes the content of `wb_pchar` to a new data clone.
        /// \note Assigning NULL only forces deallocation.
        wb_pchar& operator = (const char* nini)
        {
            WBPTRLOG( "wb_pchar::oper = (const char*) :"<<(nini?nini:"@")<<"->"<<(ptr?ptr:"@") )
            dispose();
            if(nini!=NULL) ptr=clone_str(nini);
            else ptr=NULL;
            return *this;
        }

        /// \brief Baton-passing (transferring) assignment operator.
        wb_pchar& operator = (wb_pchar& nini)
        {
            WBPTRLOG( "wb_pchar::TRANSFER oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
            dispose();
            ptr=nini.give();
            return *this;
        }

        /// \brief Cloning assignment operator in case of assignment from a constant source.
        wb_pchar& operator = (const wb_pchar& nini)
        {
            WBPTRLOG( "wb_pchar::CLONE oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
            dispose();
            ptr=clone_str(nini.get_ptr_val());
            return *this;
        }

        /// \brief Indexing operator giving access to individual characters.
        /// \note Allows changing the character at a given position, but not the pointer to it!
        char& operator [] (size_t index) const
        {                                                                                                 assert(ptr!=NULL);
            return ptr[index];
        }

        /// \brief Method changing the length of the allocated string.
        /// \warning Old data is lost.
        void alloc(size_t s)
        {
            dispose();
            if(s>0)
            {
                ptr=new char[s];                                                                        assert(ptr!=NULL);
                *ptr='\0';
            }
        }

        /// \brief Method calculates the string size and returns it. Returns 0 when there is no string.
        MAYBE_UNUSED
        size_t get_size() const
        {
            return ptr?(::strlen(ptr)):(0);
        }

        /// \brief Alias for `get_ptr_val`.
        /// \note It is not checked for NULL — let the caller worry about it.
        const char* get() const
        {
            return get_ptr_val();
        }

        /// \brief Method outputs formatted data to the content of the `wb_pchar` object.
        /// \param format : as for `vsprintf`
        /// \note It does not check the size!!!
        MAYBE_UNUSED
        wb_pchar& prn(const char* format,...);

        /// \brief Method appends to the content. \note It does not check the size!!!
        MAYBE_UNUSED
        wb_pchar& add(const char* format,...);

        /// \brief Writing to a stream with a check whether it needs to be enclosed in double quotes '\"' or another.
        MAYBE_UNUSED
        static void write(ostream& s,const char* p,char enclos='\"');

        /// \brief Replaces all `for_rep` strings contained in the `wb_pchar` object with `what_ins` strings.
        /// It is case sensitive. \note The buffer is baton-passed (transferred)!!!
        friend MAYBE_UNUSED
        bool replace(wb_pchar& bufor, const char* for_rep, const char* what_ins, bool full_words, unsigned where_start/*=0*/);

        /// \brief Inserts a text string into the buffer at a specified position. \note The buffer is baton-passed (transferred)!!!
        friend MAYBE_UNUSED
        bool insert(wb_pchar& bufor, unsigned where, const char* what_ins);

        /// \brief Equivalent to standard 'strlen'.
        friend MAYBE_UNUSED
        size_t strlen(const wb_pchar& what)
        {
            return ::strlen(what.get());
        }

        /// \brief Equivalent to standard 'strcmp'.
        friend MAYBE_UNUSED
        size_t strcmp(const wb_pchar& f,const wb_pchar& s)
        {
            return ::strcmp(f.get(),s.get());
        }

        /// \brief Equivalent to standard 'strcmp', but with different parameters.
        friend MAYBE_UNUSED
        size_t strcmp(const char* f,const wb_pchar& s)
        {
            return ::strcmp(f,s.get());
        }

        /// \brief Equivalent to standard 'strcmp', but with different parameters and in reverse order.
        friend MAYBE_UNUSED
        size_t strcmp(const wb_pchar& f,const char* s)
        {
            return ::strcmp(f.get(),s);
        }

        /// \brief Equivalent to standard 'strchr'.
        friend MAYBE_UNUSED
        const char* strchr(const wb_pchar& what,const char c)
        {
            return ::strchr(what.get(),c);
        }

        /// \brief Equivalent to standard 'strstr'.
        friend MAYBE_UNUSED
        const char* strstr(const wb_pchar& what,const char* s)
        {
            return ::strstr(what.get(),s);
        }

        /// \brief Equivalent to standard 'strstr'.
        friend MAYBE_UNUSED
        const char* strstr(const wb_pchar& what,const wb_pchar& s)
        {
            return ::strstr(what.get(),s.get());
        }

    };

    /// \brief Template for a very simple array with dynamic size. \ingroup DynMemory
/// \note Supports index testing in references (assertions!!!)
    template<class T>
    class wb_dynarray
    {
        T* ptr;                    ///< The actual content of the smart pointer
        size_t size;               ///< Content size, i.e., number of elements of type `T`.

        // /// Transferring constructor.
        // /// Unfortunately, it prevents some strange constructions because it "shadows" the one with `const wb_dynarray&`.
        //wb_dynarray(wb_dynarray& nini):size(nini.size),ptr(nini.ptr)
        //  {
        //  WBPTRLOG( "wb_dynarray::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" )
        //  nini.ptr=NULL;
        //  nini.size=0;
        //  }

    public:
        /// \brief Allocating constructor. \details also accepts 0, but then nothing is allocated.
        explicit wb_dynarray(size_t s=0):size(s)
        {
            WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<')' )
            if(size>0)  ptr=new T[s];
            else ptr=NULL;
        }

        /// \brief "Copying" constructor. @note must be forced with const so it is dangerous (whatever I meant by that. :-/ ).
        //explicit? (TODO TEST!)
        wb_dynarray(const wb_dynarray& nini/*,bool copy=false*/):size(nini.size),ptr(nini.ptr)
        {
            WBPTRLOG( "wb_dynarray::COPY CONSTRUCTOR("<<((void*)&nini)<<")" )                               assert(size!=0);
            const_cast<wb_dynarray<T>&>(nini).ptr=NULL;
            const_cast<wb_dynarray<T>&>(nini).size=0;
        }

        /// \brief Multi-parameter constructor initializing items.
        explicit wb_dynarray(size_t s,T first/*,second,third, etc...*/...):size(s)
        {
            WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<",T ...)" )                                        assert(size>=1);
            ptr=new T[s];                                                             /*After allocation*/assert(ptr!=NULL);
            ptr[0]=first;
            va_list list;
            va_start(list,first);
            for(size_t i=1;i<s;i++)
                ptr[i]=va_arg(list,T);
            va_end(list);
        }

        /// \brief Destructor releases memory using the `dispose` method.
        ~wb_dynarray()
        {
            WBPTRLOG( "wb_dynarray::DESTRUCTOR" )
            dispose();
        }

        /// \brief Method checks if the array has been allocated.
        MAYBE_UNUSED
        int OK() const
        {
            return ptr!=NULL;
        }

        /// \brief Method checks if the array has been allocated. \warning OBSOLETE
        MAYBE_UNUSED
        int IsOK() const
        {
            return ptr!=NULL;
        }

        /// \brief Transferring assignment operator.
        wb_dynarray<T>& operator = (wb_dynarray<T>& nini)
        {
            WBPTRLOG( "wb_dynnarray::operator = (wb_dynarray& "<<((void*)&nini)<<")" )
            this->take(nini);
            return *this;
        }

        /// \brief Method for raw vector allocation. Previous content is released.
        /// \warning If 's' is to be 0, just use "dispose"!
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

        /// \brief Truncating the vector. E.g., useful when reading from a file when the amount of data is unknown.
        /// \warning If 's' is to be 0, use "dispose"!
        MAYBE_UNUSED
        size_t trunc(size_t s)
        {
            WBPTRLOG( "wb_dynnarray::trunc("<<s<<")" )                                                          assert(s>0);
            if(ptr==NULL) //Because then there is nothing to truncate
            {
                return 0;
            }
            if(s>0)
            {
                T* tmp=new T[s];
                memcpy(tmp,ptr,s*sizeof(T));
                dispose();  //Deletion of the old vector
                ptr=tmp;    //Saving the new vector
                size=s;
            } else dispose();
            return s;
        }

        /// \brief Vector expansion.
        /// \details
        ///     Useful for expanding the array, but somewhat dangerous (because it uses 'memcpy')
        ///     , or costly (when it uses `for (... ) ...` and T has constructors and destructors).
        MAYBE_UNUSED
        size_t expand(size_t s,const T& fillVal);

        /// \brief Vector deallocation.
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

        /// \brief Indexing operator — access to a single item.
        T& operator [] (size_t index) const
        {
#ifndef _NDEBUG
            if(ptr==NULL || index>=size)
            {   //When DEBUG! Set breakpoint below!
                cerr<<"Invalid use of wb_dynarray "<<this<<" tabptr:"<<ptr<<"index:"<<index<<" size:"<<size<<endl;
                assert(ptr!=NULL);
                assert(index<size);
            }
#endif
            return ptr[index];
        }

        /// \brief Returns the size of the internal allocated vector (possibly 0).
        size_t get_size() const
        { return size;}

        /// \brief Forces placement of a dynamically allocated vector as an array inside `dynarray`.
        void take(T* iptr,size_t isiz)
        {
            dispose();
            ptr=iptr;
            if(ptr!=NULL)
                size=isiz;
            else
                size=0;
        }

        /// \brief Takes the internal vector from the donor 'nini' and places it on this object (this).
        void take(wb_dynarray& nini)
        {
            if(size>0)
                dispose();
            ptr=nini.ptr;
            size=nini.size;
            nini.ptr=NULL;
            nini.size=0;
        }

        /// \brief Forces placement of a dynamically allocated vector as an array inside `wb_dynarray`.
        /// \note Allows performing another operation on the result or passing it immediately as a function parameter.
        MAYBE_UNUSED
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

        /// \brief Gives direct access to the internal vector.
        /// \note However, it must not be deallocated!!!
        /// \return If something outside the array is requested, it returns NULL.
        T* get_ptr_val(size_t offset=0,size_t interest=0) const
        {
            if(ptr==NULL
               || offset>=size
               || offset+interest>=size+1
                    )
                return NULL;
            return ptr+offset;
        }

        /// \brief Takes the internal raw ptr away from the array's management.
        /// \note Also retrieves information about the current size into the 'outsize' parameter.
        T* give_dynamic_ptr_val(size_t& outsize)
        {
            T* pom=ptr;
            outsize=size;
            //clearing
            ptr=NULL;size=0;
            return pom;
        }

        /// \brief Fills the array with a given element/value.
        void fill(const T& Val)
        {
            size_t i,H=get_size();
            for(i=0;i<H;i++)//Over elements
            {
                (*this)[i]=Val;//Fill this element
            }
        }

        /// \brief Shifting a single array element to the end.
        MAYBE_UNUSED
        void shift_left(size_t index)
        {
            if(index>=size-1) return;//Exceptionally nothing needs to be done
            char bufor[sizeof(T)];                                                                        assert(ptr!=NULL);
            assert(index<size);
            memcpy(bufor,ptr+index,sizeof(T));/* we move via memcpy to avoid using assignment */

            size_t ile=size-index-1;
            memcpy(ptr+index,ptr+(index+1),sizeof(T)*ile);
            memcpy(ptr+(size-1),bufor,sizeof(T));
        }

        /// \brief Brutal copying from a raw array.
        /// \warning In the case of objects with non-trivial destructors, it may not end well.
        MAYBE_UNUSED
        void raw_copy_from(const T Where[],size_t how_many_elements)
        {                                                                                   assert(how_many_elements<=size);
            memcpy(ptr,Where,how_many_elements*sizeof(T));
        }


        /// \brief Brutal copying from a dynamic array.
        /// \warning In the case of objects with non-trivial destructors, it may not end well!
        MAYBE_UNUSED
        void raw_copy_from(const wb_dynarray<T>& Where)
        {                                                                                   assert(Where.get_size()<=size);
            memcpy(ptr,Where.get_ptr_val(),Where.get_size()*sizeof(T));
        }

    };

    /// \brief Szablon prostej tablicy dwuwymiarowej o dowolnej liczbie wierszy i dowolnej długości każdego wiersza.
    /// \details Kontrole zakresów itp. asercje dziedziczy po klasie bazowej  \ingroup DynMemory
    template<class T>
    class wb_dynmatrix:public wb_dynarray< wb_dynarray<T> >
    {
    public:
        /// \brief Konstruktor alokujący, o ile nie ma żadnego zera w parametrach.
        explicit wb_dynmatrix(size_t y=0,size_t x=0):wb_dynarray< wb_dynarray<T> >(y)
        {
            WBPTRLOG( "wb_dynmatrix::CONSTRUCTOR("<<y<<','<<x<<')' )

            if(y>0 && x>0)
                for(size_t Y=0;Y<y;Y++)
                    (*this)[Y].alloc(x);
        }

        /// \brief Konstruktor inicjujący listą wskaźników. @note DZIWNY I CHYBA NIE PRZETESTOWANY. TODO?
        MAYBE_UNUSED
        explicit wb_dynmatrix(size_t s,wb_dynarray<T>* first ...):wb_dynarray<wb_dynarray<T> >(s)
        {
            WBPTRLOG( "wb_dynmatrix::CONSTRUCTOR("<<get_size()<<",wb_dynarray<T>* ...)" )
                    assert( wb_sptr<T>::get_size()>=1 );

            alloc(s);
            (*this)[0]=first;

            va_list list;
            va_start(list,first);
            for(size_t i=1;i<s;i++)
                (*this)[i]=*(va_arg(list,wb_dynarray<T>*));//Czy to wskaźnik, czy referencja to rybka

            va_end(list);
        }

        /// \brief KONSTRUKTOR formalnie KOPIUJĄCY, a tu SZTAFETUJĄCY raczej.
        wb_dynmatrix(wb_dynmatrix& nini):wb_dynarray< wb_dynarray<T> >(nini)
        {
            WBPTRLOG( "wb_dynmatrix::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" )
        }

        /// \brief DESTRUKTOR. Dba o dealokacje.
        ~wb_dynmatrix()
        {
            WBPTRLOG( "wb_dynmatrix::DESTRUCTOR" )
            dispose();
        }

        /// \brief Dealokacja wektora wektorów.
        /// \note Metoda chyba potrzebna tylko do debugowania, bo całą robotę robi metoda klasy bazowej!
        void dispose()
        {
            WBPTRLOG( "wb_dynmatrix::dispose :"<<get_size() )
            wb_dynarray< wb_dynarray<T> >::dispose();
        }

        /// \brief Rozbudowana alokacja zstępująca.
        MAYBE_UNUSED
        size_t alloc(size_t y,size_t x);

        /// \brief Wypełnianie zadaną wartością. \param Val - wzorcowa wartość do skopiowania.
        void fill(const T& Val);

    };

    /// \details Rozbudowana alokacja zstępująca macierzy.
    template<class T> inline MAYBE_UNUSED
    size_t wb_dynmatrix<T>::alloc(size_t y,size_t x)
    {
        WBPTRLOG( "wb_dynmatrix::alloc("<<y<<','<<x<<")" )                                                     assert(y>0);
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

    /// \details Metoda wypełnianie macierzy dynamicznej zadaną wartością.
    template<class T> inline
    void wb_dynmatrix<T>::fill(const T& Val)
    {
        size_t i,H=this->get_size();
        for(i=0;i<H;i++) // Wiersz po wierszu
        {
            size_t L=(*this)[i].get_size();
            for(size_t j=0;j<L;j++) // Po elementach wiersza
                (*this)[i][j]=Val;  // Wypełnij ten element!
        }
    }

    /// \details Funkcja wypełnianie tablicy dynamicznej zadaną wartością. Wersja nieobiektowa.
    /// \param Val - wzorcowa wartość do skopiowania.
    template<class T> inline MAYBE_UNUSED
    void fill(wb_dynarray<T>& Tab,const T& Val)
    {
        Tab.fill(Val);
    }

    /// \details Funkcja wypełnianie macierzy dynamicznej zadaną wartością. Wersja nieobiektowa.
    /// \param Val - wzorcowa wartość do skopiowania.
    template<class T> inline MAYBE_UNUSED
    void fill(wb_dynmatrix<T>& Mat,const T& Val)
    {
        Mat.fill(Val);
    }

    /// \details Rozszerzenie wektora. Przydatne do rozbudowy tablicy
    ///          , ale albo niebezpieczne (bo `memcpy`) lub kosztowne (bo `for`) jak T ma konstruktory i destruktory.
    ///          Powiększamy, więc s > size (od poprzedniego rozmiaru)
    template<class T> inline MAYBE_UNUSED
    size_t wb_dynarray<T>::expand(size_t s,const T& fillVal)
    {					                                                                                 assert(s>size);
        WBPTRLOG( "wb_dynnarray::expand("<<s<<")" )
        if(ptr==nullptr)
            return 0;			//Bo wtedy reszta to bzdura

        T* tmp=new T[s]; // Alokacja. Zadziałają konstruktory!

        // Wariant ryzykowny
        //  = new char[sizeof(T)*s];
        //	memcpy(tmp,ptr,s*sizeof(T));
        //	delete (void*)ptr; // Zwalnianie bez wywoływania możliwych destruktorów

        //Bezpieczne, choć nieefektywny przepisanie zawartości komórek
        for(size_t i=0;i<size;i++)
            tmp[i]=ptr[i]; //Dla klas użyty tu operator przypisania

        //Jakieś wypełnienie nowych komórek
        for(size_t i=size;i<s;i++)
            tmp[i]=fillVal; // Dla klas użyty tu operator przypisania

        dispose();  //Kasacja starego wektora. Działają destruktory.

        // Zapamiętanie nowego wskaźnika
        ptr=tmp;
        size=s;
        return size;
    }

#if HIDE_WB_PTR_IO != 1
    // Stream Input/Output for smart pointers.
    // Only declarations here. Implementation must be in another file (inlines in wb_pchario.h)

    ostream& operator<<(ostream&,const wb_sptr<char>&); ///< Is this implemented anywhere? Yes, in wb_pchario.h!
    istream& operator>>(istream&,wb_sptr<char>&);       ///< Is this implemented anywhere?
    ostream& operator<<(ostream&,const wb_pchar&);      ///< Is this implemented anywhere?
    istream& operator>>(istream&,wb_pchar&);            ///< Is this implemented anywhere?

    template<class T>
    ostream& operator<<(ostream&,const wb_sptr<T>&); ///< Is this implemented anywhere?

    template<class T>
    istream& operator>>(istream&,wb_sptr<T>&); ///< Is this implemented anywhere?

    template<class T>
    ostream& operator<<(ostream&,const wb_dynarray<T>&); ///< Is this implemented anywhere?

    template<class T>
    istream& operator>>(istream&,wb_dynarray<T>&); ///< Is this implemented anywhere?

    template<class T>
    ostream& operator<<(ostream&,const wb_dynmatrix<T>&); ///< Is this implemented anywhere?

    template<class T>
    istream& operator>>(istream&,wb_dynmatrix<T>&); ///< Is this implemented anywhere?

    /// \brief Implementation of writing a character string to streams with checking if a character needs to be "escaped" (e.g. `\`).
    void escaped_pchar_write(std::ostream& s,const char* p,char enclos='\"');

    MAYBE_UNUSED
    inline void write(ostream& o,const char* p)
    {
        escaped_pchar_write(o,p);
    }

    inline MAYBE_UNUSED
    void wb_pchar::write(ostream &s, const char *p, char enclos)
    {
        escaped_pchar_write(s,p,enclos);
    }

#endif //HIDE_WB_PTR_IO

/// @}

} //namespace wbrtm

/// From the C language namespace
extern "C"
{
/// Control of reaction to terminating errors. Taken from SymShell.h, but sometimes used without it.
extern int WB_error_enter_before_clean;
}

/* ***************************************************************** */
/*               WB_RTM for SymShell  version 2026                   */
/* ***************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*    GITHUB: https://github.com/borkowsk                            */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */
#endif
