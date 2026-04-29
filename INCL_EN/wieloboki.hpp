/// \file
/// \brief Polygon class and a library of various polygons
///        ************************************************
/// @date 2026-04-29 (last modification)
/// \details Created as an example of defining a fairly simple class, which later became more complex.
///          However, for historical reasons, it retained Polish naming in the "camelCase" convention.
/// \author  borkowsk
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SYMSHELL_WIELOBOKI_HPP_INCLUDED_
#define SYMSHELL_WIELOBOKI_HPP_INCLUDED_ (1)

#include "symshell.h" // Required types from symshell
#include "maybe_unused.h"

/**
 * @defgroup SymShellUtilsEN Various additional tools for graphics
 * \brief    Defined colors and additional shapes.
 */
/// @{

/// \brief Definition of the Wielobok (Polygon) class — ALMOST without any method implementations inside.
class Wielobok
{
private:
    // Private fields — no one has direct access.
    // These fields will be in every object of this class:
    //*////////////////////////////////////////////
    ssh_point* Punkty; //!< Pointer to the list of points.
    unsigned   Ilobok; //!< How many points are on the list.

public:
    // Methods that also work for constant (const) Polygons:
    //======================================================

    /// \brief Access to a single vertex of the polygon. \return This method returns a "constant reference" to the point.
    ///  Allows reading, but does not allow modifying this point.
    MAYBE_UNUSED
    unsigned IlePunktow() const { return Ilobok; }
    /// \copybrief IlePunktow @see IlePunktow
    MAYBE_UNUSED
    inline unsigned GetPointCount() const { return IlePunktow(); }

    /// \brief Access to a single vertex of the polygon. \return This method returns a "constant reference" to the point.
    ///  Allows reading, but does not allow modifying this point.
    MAYBE_UNUSED
    const ssh_point& DajPunkt(unsigned pozycja) const;
    /// \copybrief DajPunkt @see DajPunkt
    MAYBE_UNUSED
    inline const ssh_point& GetPoint(unsigned position) const { return DajPunkt(position); }

    /// \brief Draws the polygon somewhere in a given indexed color.
    MAYBE_UNUSED
    void Rysuj(int x, int y, ssh_color color) const;
    /// \copybrief Rysuj @see Rysuj
    MAYBE_UNUSED
    inline void Draw(int x, int y, ssh_color color) const { Rysuj(x, y, color); }

    /// \brief Draws the polygon somewhere in a given RGB color.
    MAYBE_UNUSED
    void Rysuj(int x, int y, int R, int G, int B) const;
    /// \copybrief Rysuj @see Rysuj
    MAYBE_UNUSED
    inline void Draw(int x, int y, int R, int G, int B) const { Rysuj(x, y, R, G, B); }

    /// \brief Determining the boundaries of the polygon.
    /// \details The method reviews the points and provides information
    ///          - about the bounding rectangle,
    ///          - and about the radius of the bounding circle centered at point 0,0.
    /// \param MinX[out] means the smallest X.
    /// \param MinY[out] means the smallest Y.
    /// \param MaxX[out] means the largest X.
    /// \param MaxY[out] means the largest Y.
    /// \param r[out] means the radius of the circumscribing circle.
    MAYBE_UNUSED
    void Zakresy(double& MinX, double& MinY, double& MaxX, double& MaxY, double& r) const;
    /// \copybrief Zakresy @see Zakresy
    MAYBE_UNUSED
    inline void GetBounds(double& MinX, double& MinY, double& MaxX, double& MaxY, double& r) const
    { Zakresy(MinX, MinY, MaxX, MaxY, r); }

    // Constructors and Destructor:
    //=============================

    /// @name Constructors creating objects of the `Wielobok` class.
    /// @details
    ///          Creation is based on patterns, i.e., arrays or other `Wielobok` objects.
    ///          There is no possibility to create an empty "Wielobok", although it can be "zeroed" if forced.
    /// @{
    Wielobok(const Wielobok& model);    //!< Copy constructor.
    Wielobok(const ssh_point model[], unsigned n); //!< Constructor from an array of points.
    Wielobok(unsigned n, float r);     //!< Constructor for an n-gon with a variable number of sides and size.
    /// @}

    /// \brief Destructor. Frees the auxiliary array. \note "virtual" - because it is safer with inheritance.
    virtual ~Wielobok();

    // Polygon transformations:
    //======================================================

    /// @name Polygon transformations.
    /// @details Modify the list of points of a given polygon.
    /// \note Rotation, scaling, and centering methods are "destructive" because points are stored
    ///       as integers and the results do NOT have to be integers at all!
    ///       It is reasonably safe to use them once or twice, as the shape may become distorted thereafter.
    /// @{

    /// Changes the polygon into its vertical mirror reflection.
    MAYBE_UNUSED
    void OdbijWPionie();
    /// \copybrief OdbijWPionie @see OdbijWPionie
    MAYBE_UNUSED
    inline void FlipVertical() { OdbijWPionie(); }

    /// Changes the polygon into its horizontal mirror reflection.
    MAYBE_UNUSED
    void OdbijWPoziomie();
    /// \copybrief OdbijWPoziomie @see OdbijWPoziomie
    MAYBE_UNUSED
    inline void FlipHorizontal() { OdbijWPoziomie(); }

    /// Rotates the polygon by a number of radians.
    MAYBE_UNUSED
    void ObracajORad(double radian);
    /// \copybrief ObracajORad @see ObracajORad
    MAYBE_UNUSED
    inline void RotateRad(double radian) { ObracajORad(radian); }

    /// Scales the size of the polygon.
    MAYBE_UNUSED
    void Skaluj(double sx, double sy);
    /// \copybrief Skaluj @see Skaluj
    MAYBE_UNUSED
    inline void Scale(double sx, double sy) { Skaluj(sx, sy); }

    //!< Changes coordinates so they are centered around the polygon's center of gravity.
    MAYBE_UNUSED
    void Centruj();
    /// \copybrief Centruj @see Centruj
    MAYBE_UNUSED
    inline void Center() { Centruj(); }

    /// @}

    // Library of basic shapes:
    //=========================

    /// @name Library of basic shapes within the `Wielobok` class namespace.
    /// ***************************************************************************
    /// @details
    /// Implemented as static methods, as this allows for different implementations
    /// of storing these polygons, e.g., creating them only when needed
    /// or loading them from disk.
    /// Functions return "constant references," meaning you can read or redraw, but not change them.
    /// "static" regarding a method means it is a member of the class as such,
    /// rather than of each object of that class individually. An object is not needed for the call!
    /// @{

    /// Listing polygon names.
    /// \param poz - next position in the library.
    /// \returns the name of the i-th polygon from the library. If NULL, there are no more.
    MAYBE_UNUSED
    static const char* NazwyWielobokow(int poz);
    /// \copybrief NazwyWielobokow @see NazwyWielobokow
    MAYBE_UNUSED
    static inline const char* GetPolygonName(int position) { return NazwyWielobokow(position); }

    /// \brief Provides a shape from the library. \return If no correct one exists, returns the default shape.
    MAYBE_UNUSED
    static const Wielobok& WielobokWgNazwy(const char* tag);
    /// \copybrief WielobokWgNazwy @see WielobokWgNazwy
    MAYBE_UNUSED
    static inline const Wielobok& GetPolygonByName(const char* tag) { return WielobokWgNazwy(tag); }

    /// \brief Provides a shape from the library. \return If it doesn't exist, returns NULL
    /// TODO ... or tries to load from file.
    MAYBE_UNUSED
    static const Wielobok* SprobujWielobok(const char* tag);
    /// \copybrief SprobujWielobok @see SprobujWielobok
    MAYBE_UNUSED
    static inline const Wielobok* TryPolygon(const char* tag) { return SprobujWielobok(tag); }

    /// \brief Provides the default/substitute shape if the required one is missing.
    MAYBE_UNUSED
    static const Wielobok& Domyslny();
    /// \copybrief Domyslny @see Domyslny
    MAYBE_UNUSED
    static inline const Wielobok& Default() { return Domyslny(); }

    // Shortcuts to frequently used polygons from the library:
    //*///////////////////////////////////////////////////////

    /// Tent shape. Shortcut access to the object from the shape library.
    MAYBE_UNUSED
    static const Wielobok& Namiot();
    /// \copybrief Namiot @see Namiot
    MAYBE_UNUSED
    static inline const Wielobok& Tent() { return Namiot(); }

    /// Rhombus shape. Shortcut access to the object from the shape library.
    MAYBE_UNUSED
    static const Wielobok& Romb();
    /// \copybrief Romb @see Romb
    MAYBE_UNUSED
    static inline const Wielobok& Rhombus() { return Romb(); }

    /// House shape. Shortcut access to the object from the shape library.
    MAYBE_UNUSED
    static const Wielobok& Domek();
    /// \copybrief Domek @see Domek
    MAYBE_UNUSED
    static inline const Wielobok& House() { return Domek(); }

    /// UFO shape. Shortcut access to the object from the shape library.
    MAYBE_UNUSED
    static const Wielobok& Ufo();
    /// \copybrief Ufo @see Ufo
    MAYBE_UNUSED
    static inline const Wielobok& UFO() { return Ufo(); }

    /// Human figure shape. Shortcut access to the object from the shape library.
    MAYBE_UNUSED
    static const Wielobok& Ludzik(int typ);
    /// \copybrief Ludzik @see Ludzik
    MAYBE_UNUSED
    static inline const Wielobok& Humanoid(int type) { return Ludzik(type); }

    /// @}
};

/// \brief Alias for the Wielobok class for English-speaking developers.
using Polygon = Wielobok;

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

