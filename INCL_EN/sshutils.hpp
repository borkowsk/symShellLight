/** @file   sshutils.hpp
 * @brief  Implementation of auxiliary symshell functions in a platform-independent manner.
 * @date 2026-04-29 (last modification)
* @details
*       Written ALMOST NON-OBJECT-ORIENTED but in C++
*       Contains: print_width(), empty rect(), bar3D(), arrow() ...etc...
* @author borkowsk                                                                               */
/* ********************************************************************************************* */
#ifndef SYMSHELL_UTILS_HPP_INCLUDED_
#define SYMSHELL_UTILS_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

#include <cmath>
#include "symshell.h"
#include "maybe_unused.h"

#ifndef M_PI
/** @name M_PIx
 * @brief Custom definitions of constants related to Pi.
 * @details Defined when not provided by the compiler. E.g., older MVC++ did not define them.
 * @{
 */
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
/** @} */
#endif

/**
 * @defgroup SymShellUtilsEN Various additional tools for graphics
 * \brief    Defined colors, additional shapes and other auxiliary functionalities.
 */
/// @{

/// Auxiliary quadratic function to shorten the code here and there.
template<class NUM>
inline NUM sqr(const NUM& x)
{ return x*x; }

/// \brief Euclidean distance calculation. Often needed in such programs.
/// @note UNINTUITIVE PARAMETER LAYOUT!
MAYBE_UNUSED
double distance(double X1,double X2,double Y1,double Y2);

/// \brief Alias for the `ssh_color` type.
/// \note Indexed color is used in the drawing functions of the 'sshutils' module.
typedef ssh_color wb_color;

/**
 * @name Set of 'sshutils' color identifiers.
 * @details These are selected indices from a set of 256 colors and 256 grayscale shades.
 * @{
 */
const wb_color default_transparent=wb_color(-1);     ///< Default color to indicate transparency.
const wb_color default_black=0;                      ///< Default black indexed color.
const wb_color default_white=255;                    ///< Default white indexed color.
MAYBE_UNUSED
const wb_color default_color=default_transparent;    ///< Default indexed color.
MAYBE_UNUSED
const wb_color default_dark_gray=256+64;             ///< Default dark gray indexed color.
MAYBE_UNUSED
const wb_color default_half_gray=256+128;            ///< Default medium gray indexed color.
MAYBE_UNUSED
const wb_color default_light_gray=256+128+64;        ///< Default light gray indexed color.
/** @} */

/// \brief Function interpreting a string as an RGB value.
/// \param s should contain text with the RGB color definition.
/// \param end_ptr allows checking for errors.
/// \return RGB color encoded as a 32-bit number. TODO — should return ssh_rgba, but not used for now.
/// \details Acceptable formats are: xffffff  b111111111111111111111111  rgb(255,255,255) RGB(255,255,255)
MAYBE_UNUSED
unsigned strtorgb(const char *s, char **end_ptr);

/**
 * @name Settings for thickness and sizes of 'sshutils' elements.
 * @details Configuration values for frames, crosses, arrows, etc...
 * @{
 */

extern int def_frame_width; /* =1;*/                  ///< Default frame thickness.
extern int def_cross_width; /* =5;*/                  ///< Default cross width.
extern int def_scale_width; /* =10;*/                 ///< Default scale width.
extern double def_arrow_size; /* =15;*/               ///< Default arrow head length.
extern double def_arrow_theta; /*=M_PI/6.0+M_PI;*/    ///< Default arrow head opening angle.

/// \brief Configuration structure for 3D bars.
struct settings_bar3d
{
    ///Constructor for the 3D bar configuration structure.
    settings_bar3d(
            int ia=10,
            int ib=10,
            int ic=6,
            wb_color wcol=default_white,
            wb_color bcol=default_black
    ):
            a(ia),b(ib),c(ic),wire(wcol),back(bcol)
    {}
    int         a; ///< Length of a bar segment 'a'.
    int         b; ///< Length of a bar segment 'b'.
    int         c; ///< Length of a bar segment 'c'.
    wb_color wire; ///< Frame color.
    wb_color back; ///< Background color information.
};

/// \brief  3D bar configuration function.
/// \return Returns the previous configuration or NULL if restoring the previously saved one.
MAYBE_UNUSED
const settings_bar3d* bar3d_config(settings_bar3d* st);

/** @} */

/// \brief  Draws a 3D bar using indexed colors.
MAYBE_UNUSED
void bar3d(int x,int y,int h,wb_color col1,wb_color col2);

/// \brief  Draws a 3D bar in RGB color with a shadow.
MAYBE_UNUSED
void bar3dRGB(int x,int y,int h,int R,int G,int B,int ShadowDiv);

/// \brief  Draws a square frame with a specified thickness.
MAYBE_UNUSED
void rect(int x1,int y1,int x2,int y2,wb_color frame_c,int width=def_frame_width);

/// \brief  Draws a vertical color scale.
MAYBE_UNUSED
void ver_scale(int x1,int y1,int width=def_scale_width,wb_color start=0,wb_color end=255);

/// \brief  Draws a horizontal color scale.
MAYBE_UNUSED
void hor_scale(int x1,int y1,int high=def_scale_width,wb_color start=0,wb_color end=255);

/// \brief  Draws a cross.
MAYBE_UNUSED
void cross(int x,int y,wb_color color,int line_width=def_cross_width);

/// \brief  Draws a freely oriented arrow from point x1,y1 to x2,y2.
MAYBE_UNUSED
void arrow(int x1,int y1,int x2,int y2,wb_color color,double size=def_arrow_size,double theta=def_arrow_theta);

/// \brief  Efficiently draws a horizontal arrow.
MAYBE_UNUSED
void vert_arrow(int x1,int x2,int y,wb_color color,double size=def_arrow_size);

/// \brief  Efficiently draws a vertical arrow.
MAYBE_UNUSED
void hor_arrow(int x,int y1,int y2,wb_color  color,double size=def_arrow_size);

/// \brief   Prints text within an area no wider than `max_width`.\
/// \param x, y starting point coordinates for the text.
/// \param max_width maximum length in pixels.
/// \param col, bcg text and background colors.
/// \param format text format as in `printf`.
/// \param ... variables to fill the format definition.
/// \return  Returns width or 0.
/// \details The internal buffer has no more than 1024 characters.
MAYBE_UNUSED
int print_width(int x,int y,int max_width,wb_color col,wb_color bcg,const char* format ...);

extern "C" {
/// \brief Displaying an HTML file via the system shell.
/// \details In fact, it can be used for all types of files
///          that a browser can display.
/// \param URL - full URL, but sometimes a filename will do :-D ...
/// \note It comes from the WB_RTM library and is declared in "sshutils.h" for convenience.
/// \return should return the execution code of the "child" program.
MAYBE_UNUSED
int ViewHtml(const char* url);
// TODO `int view_html(const char* url);`
}

/// @name Simple support for a multi-language interface.
/// @details Selection of text strings from one of two or three national languages.
/// @{
extern unsigned lang_selector; ///< @brief User variable for the `lang` set of functions.

MAYBE_UNUSED inline  /// Selection of one of two text strings based on the `lang_selector` variable.
const char* lang(const char* def,const char* alt)
{ if(lang_selector!=0) return alt; else return def;}

MAYBE_UNUSED inline  /// Selection of one of three text strings based on the `lang_selector` variable.
const char* lang(const char* def,const char* alt1,const char* alt2)
{ if(lang_selector==2) return alt2; if(lang_selector==1) return alt1; else return def;}
/// @}

/// @}


/* ******************************************************************/
/*                 SYMSHELLLIGHT  version 2026                      */
/* ******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif


