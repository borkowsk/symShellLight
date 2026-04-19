/** @file
 * @brief SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++ (EN Doxygen).
 * @date 2026-04-19 (translated)                                             */
/* ========================================================================= */
 /**
 * \details
 * The whole file changed massively: 15.11.2020
 * Comments changed massively: 3-4.01.2022
 *
 * \note
 * - https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI
 * - https://github.com/borkowsk
 *
 ** \author     Designed by W. Borkowski from the University of Warsaw
 **
 ** \library    SYMSHELLLIGHT  version 2026b
 */
#ifndef SYMSHELL_H_INCLUDED_
#define SYMSHELL_H_INCLUDED_ (1)

/**
* @defgroup	GrxInterfaceEN Basic functions of the graphical interface
* @brief	Portable drawing and associated functions between X11 and Windows.
* @details
*		Most are C modules, or at least provide such an interface.
*		A C++ version that saves to SVG files is also implemented.
*/
/// @{

/* TYPES */
typedef unsigned char		uchar8b;		/**< \brief BASIC CHARACTER TYPE. MUST HAVE AT LEAST 8 BITS. */

typedef	uchar8b				ssh_bool;		/**< \brief Substitute logical type. Can be 0 or 1. */
typedef	signed	int			ssh_msg;		/**< \brief Keyboard character or other special numbers, especially from menus. */
typedef	signed	int			ssh_mode;		/**< \brief Usually 0 or 1, maybe 2, but -1 indicates an error. */
typedef	signed	int			ssh_stat;		/**< \brief Return values used as status for some functions. */
typedef	signed	int			ssh_coordinate;	/**< \brief Any screen coordinates. */
typedef unsigned	int		ssh_length;		/**< \brief Situations where 0 is allowed, but not negative values, e.g., array lengths. */
typedef	unsigned	int		ssh_natural;	/**< \brief Numbers greater than zero, where zero is an unexpected situation. */
typedef	unsigned	int		ssh_intensity;	/**< \brief Color components etc., values from 0 upwards. */
typedef	unsigned	int		ssh_color;		/**< \brief Indexed color. TODO change name to ssh_color_index? */
typedef	float				ssh_radian;		/**< \brief Angles in radians for arcs. */
/** \brief A point in screen coordinates. */
typedef	struct	ssh_point	{ssh_coordinate x,y;}		ssh_point;
/** \brief Type for a set of RGB components. TODO What about alpha? Union with uint32?  */
typedef	struct	ssh_rgb		{uchar8b r,g,b;}			ssh_rgb;
/** \brief TODO RGB with alpha. Union with uint32?  */
typedef	struct	ssh_rgba	{uchar8b r,g,b,a;}			ssh_rgba;

#ifdef __cplusplus
extern "C" {
const ssh_mode  PALETTE_LENGTH=512;             /**< \brief Length of the predefined color palette. */
const ssh_mode  PALETE_LENGHT=512;              /**< \brief Length of the predefined color palette (old name). */
const ssh_mode  SSH_SOLID_TEXT=0;               /**< \brief Text on a filled background bar. */
const ssh_mode  SSH_TRANSPARENT_TEXT=1;         /**< \brief Text on a transparent background. */
const ssh_mode  SSH_SOLID_PUT=1;                /**< \brief Content overlaid on the background. */
const ssh_mode  SSH_XOR_PUT=2;                  /**< \brief Content XOR-ed with the background. */
const ssh_mode  SSH_LINE_SOLID=1;               /**< \brief Solid line. */
const ssh_mode  SSH_LINE_DOTTED=2;              /**< \brief Dotted line. */
const ssh_mode  SSH_LINE_DASHED=3;              /**< \brief Dashed line. */
const ssh_mode  SSH_YES=1;                      /**< \brief Confirmation flag. */
const ssh_mode  SSH_NO=0;                       /**< \brief Negation flag. */
#else
#define PALETTE_LENGTH      (512)               /**< Length of the predefined color palette. */
#define PALETE_LENGHT       (512)               /**< \brief Length of the predefined color palette (old name). */
#define SSH_SOLID_TEXT       (0)                /**< \brief Text on a filled background bar. */
#define SSH_TRANSPARENT_TEXT (1)                /**< \brief Text on a transparent background. */
#define SSH_SOLID_PUT        (1)                /**< \brief Content overlaid on the background. */
#define SSH_XOR_PUT          (2)                /**< \brief Content XOR-ed with the background. */
#define SSH_LINE_SOLID       (1)                /**< \brief Solid line. */
#define SSH_LINE_DOTTED      (2)                /**< \brief Dotted line. */
#define SSH_LINE_DASHED      (3)                /**< \brief Dashed line. */
#define SSH_YES              (1)                /**< \brief Confirmation flag. */
#define SSH_NO               (0)                /**< \brief Negation flag. */
#endif

/* MODULE-DEPENDENT VARIABLES AND CONSTANTS
 * ======================================== */
/** \brief Name of the currently used implementation for graphics.
 * Could be "X11", "WINDOWS" or "SVG". */
extern const char* _ssh_grx_module_name;

/** \brief If not 0, the window is usable. */
extern unsigned long _ssh_window;

/** \brief Determines whether to close immediately or allow viewing the content.
* Used to control `close_plot` - whether it requires user confirmation. */
extern int WB_error_enter_before_clean/* =0 */;

/* OPENING AND CLOSING GRAPHIC MODE (WINDOW) */
/* ========================================= */

/* Configuration operations guaranteed before initialization */
/* --------------------------------------------------------- */

/** \brief Passing execution parameters and window name. */
void shell_setup(const char* title,                      /**< Application name used as window title or part of it. */
                 const int   i_argc,                     /**< Number of ALL execution parameters. */
                 const char* i_argv[]                    /**< Passing execution parameters. */
                );

/** \brief Changes window title bar. */
void set_title(const char* title);

/** \brief Toggling window buffering. Might not work after initialization. */
void buffering_setup(ssh_mode yes);

/** \brief Determines whether to simulate fixed window dimensions.
 * In this mode, resizing the window scales pixels by an integer multiple. */
void fix_size(ssh_mode yes);

/** \brief Changes color definition in the color palette. Indices 0..255. */
void set_rgb(ssh_color color,                                  /**< Color index. */
             ssh_intensity r,                                  /**< Red component value. */
             ssh_intensity g,                                  /**< Green component value. */
             ssh_intensity b                                   /**< Blue component value. */
             );

/** \brief Changes grayscale shade definition in the gray palette. Indices 256 to 511. */
void set_gray(ssh_color shade,ssh_intensity intensity);

/** \brief Sets the color index for clearing. Might not work after initialization. */
void set_background(ssh_color c);

/* Window initialization and total window closing */
/* ---------------------------------------------- */

/** \brief Platform-specific graphics initialization (formerly semi-graphics too!).
* \return Returns 1 if successful. */
ssh_stat  init_plot(ssh_natural  a,                  /**< Window width in pixels. */
                    ssh_natural  b,                  /**< Window height in pixels. */
                    ssh_natural ca,                  /**< Additional text columns on the sides. */
                    ssh_natural cb                   /**< Additional text lines at the bottom using default font. */
                    );


/** \brief Closing graphics (or semi-graphics 😁 formerly).
* \details Automatically installed in `atexit`, hence the foolish `(void)` to avoid warnings (obsolete?). */
void close_plot(void);

/* OPERATIONS CONCERNING THE ENTIRE GRAPHICS WINDOW
 * ================================================ */

/** \brief Forced to wait for a certain number of milliseconds. */
void delay_ms(ssh_natural ms);

/** \brief Forced to wait for a certain number of microseconds. */
void delay_us(ssh_natural us);

/** \brief Final reconciliation of screen content with previously requested actions.
 * \details For X11 over the network, it guarantees transmission; it may transfer virtual screen to actual, etc. */
void flush_plot();

/** \brief Absolutely clears the screen/window or virtual screen. Can be time-consuming. */
void clear_screen();

/** \brief Silently forgets previous screen content.
 * \details Used when it is expected that content would be overwritten anyway, and such action is cheap (especially for vector modules).
 * \return Returns 1 if successful.
 * \warning DOES NOTHING UNDER X11 and MS Windows */
ssh_stat  invalidate_screen();

/** \brief Saves screen content to a graphic file in the platform's a native format: BMP, XBM, SVG, etc.
 * \details Might not work in non-buffered window/screen mode.
 * \return Returns 1 if successful. */
ssh_stat  dump_screen(const char* file_name);

/* Operations for changing graphics window operating properties
 * ============================================================ */

/** \brief Sets whether the mouse should be handled. \return Previous flag state. */
ssh_mode    mouse_activity(ssh_mode yes);

/** \brief Enables text printing without overwriting the background. \return Previous setting. */
ssh_mode    print_transparently(ssh_mode yes);

/** \brief Sets line width. Thick lines are expensive! TODO WHAT ABOUT 0?
 * \return Previous setting. */
ssh_natural line_width(ssh_natural width);

/** \brief Sets line drawing style: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED.
 * \return Previous setting. */
ssh_mode    line_style(ssh_mode style);

/** \brief Sets the relation of new drawing to old screen content: SSH_SOLID_PUT, SSH_XOR_PUT.
 * \return Previous setting. */
ssh_mode    put_style(ssh_mode style);

/** \brief Sets the current line and outline color using ssh_color type, along with style and width. */
void set_pen(ssh_color c, ssh_natural width, ssh_mode style);

/** \brief Sets the current fill color using ssh_color type. */
void set_brush(ssh_color c);

/** \brief Sets the current line color using RGB components, along with style and width. */
void set_pen_rgb(ssh_intensity r,                              /**< Red component. */
                 ssh_intensity g,                              /**< Green component. */
                 ssh_intensity b,                              /**< Blue component. */
                 ssh_natural width,                            /**< Line width. */
                 ssh_mode style                                /**< Line drawing style as in `line_style`. */
                );

/** \brief Sets current color and transparency for lines using RGBA components, along with style and width.
 * \note Alpha component may be completely ignored! */
void set_pen_rgba(ssh_intensity r,                             /**< Red component. */
                  ssh_intensity g,                             /**< Green component. */
                  ssh_intensity b,                             /**< Blue component. */
                  ssh_intensity a,                             /**< Alpha channel. */
                  ssh_natural width,                           /**< Line width. */
                  ssh_mode style                               /**< Line drawing style as in `line_style`. */
                  );

/** \brief Sets current fill color using RGB components. */
void set_brush_rgb(ssh_intensity r,                            /**< Red component. */
                   ssh_intensity g,                            /**< Green component. */
                   ssh_intensity b                             /**< Blue component. */
                   );

/** \brief Sets current color and transparency for fills using RGBA components. */
void set_brush_rgba(ssh_intensity r,                            /**< Red component. */
                    ssh_intensity g,                            /**< Green component. */
                    ssh_intensity b,                            /**< Blue component. */
                    ssh_intensity a                             /**< Alpha channel. */
                    );

/* READING CURRENT GRAPHICS WINDOW SETTINGS
 * ======================================== */

/** \brief Checks buffering. \return Returns 1 if buffered. */
ssh_mode  buffered();

/** \brief Checks if the window has a fixed size. \return Returns SSH_YES or SSH_NO.
 * \details Currently, both options are available only in MS Windows environment.
 * In SVG, the size is always fixed, and in X11 it is externally flexible but multiplied.
 * Thus, graphics are scaled by multiples, but labels remain at the same size. */
ssh_mode  fixed();

/** \brief Get the RGB settings of a specific color in the palette. */
ssh_rgb   get_rgb_from(ssh_color c);

/** \brief Current background color... */
ssh_color background();

/** \brief Current line color as ssh_color (color index in the table).
    \details
        If "pen" was set in RGB(A) mode, it returns (unsigned)-1.
        In the case of SVG implementation, it always returns -1024. */
ssh_color get_pen();

/** \brief Current fill color as ssh_color.
    \details
        If "brush" was set in RGB(A) mode, it returns (unsigned)-1.
        In the case of SVG implementation, it always returns 0 (black). */
ssh_color get_brush();

/** \brief Current line width. */
ssh_natural  get_line_width();

/** \brief Current window dimensions after init_plot calculations
 * ...and any "manual" changes made by the operator. */
ssh_natural  screen_width();                                    /**< Total usable window width in pixels. */
ssh_natural  screen_height();                                   /**< Total usable window height in pixels. */

/* Character and text sizes
   ========================  */

/** \brief Current character dimensions needed for text positioning. */
ssh_natural  char_height(char sample);         /**< Character height. Best to provide 'X'. */
ssh_natural  char_width(char sample);          /**< Character width. @note In 99% of situations, we have a monospaced font. */
ssh_natural  string_height(const char* str);   /**< Height of the text string on the screen. */
ssh_natural  string_width(const char* str);    /**< Width of the text string on the screen. */

/* PRINTING ON SCREEN
 * ================== */

/** \brief Draws text starting from given graphic coordinates. Otherwise, works like standard `printf`. */
/** \details The default is to print a black text on a white background in the window. */
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...);

/** \brief Draws text in the window using default colors. */
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...);

/** \brief Outputting text to the screen/window from given coordinates and in specified colors. */
void printc(ssh_coordinate x,                                         /**< Horizontal coordinate. */
            ssh_coordinate y,                                         /**< Vertical coordinate. */
            ssh_color fore,                                           /**< Text color index. */
            ssh_color back,                                           /**< Background color index, if printing non-transparently. */
            const char* format,                                       /**< Text format as in \see `printf`. */
            ...                                                       /**< Variadic parameters depending on format. */
            );

/** \brief Outputting text to the screen from given coordinates with the possibility to set "ink" via RGB. */
void print_rgb(ssh_coordinate x,                                      /**< Horizontal coordinate. */
               ssh_coordinate y,                                      /**< Vertical coordinate. */
               ssh_intensity  r,                                      /**< Red component. */
               ssh_intensity  g,                                      /**< Green component. */
               ssh_intensity  b,                                      /**< Blue component. */
               ssh_color   back,                                      /**< Background color index, if printing non-transparently. */
               const char* format,                                    /**< Text format as in \see `printf`. */
               ...                                                    /**< Variadic parameters depending on format. */
               );

/* Lighting up points on the screen
   ================================  */

void plot_d(ssh_coordinate x,ssh_coordinate y);                       /**< Display a point on the screen in the default color. */
void plot(ssh_coordinate x,ssh_coordinate y, ssh_color c);            /**< Display a point in a palette color. */

/** \brief Display a point on the screen in RGB color. */
void plot_rgb(ssh_coordinate x,                                       /**< Horizontal coordinate. */
              ssh_coordinate y,                                       /**< Vertical coordinate. */
              ssh_intensity  r,                                       /**< Red component. */
              ssh_intensity  g,                                       /**< Green component. */
              ssh_intensity  b                                        /**< Blue component. */
              );

/** \brief Flood fill or seed algorithm. */
void fill_flood(ssh_coordinate x,                                     /**< Horizontal coordinate of starting point. */
                ssh_coordinate y,                                     /**< Vertical coordinate of starting point. */
                ssh_color fill,                                       /**< Fill color index. */
                ssh_color border                                      /**< Border color index. */
                );

/** \brief Flood fill or seed algorithm. */
void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,                /**< Coordinates of starting point. */
                ssh_intensity rf,                                     /**< Red component of fill color. */
                ssh_intensity gf,                                     /**< Green component of fill color. */
                ssh_intensity bf,                                     /**< Blue component of fill color. */
                ssh_intensity rb,                                     /**< Red component of border color. */
                ssh_intensity gb,                                     /**< Green component of border color. */
                ssh_intensity bb                                      /**< Blue component of border color. */
                );

/* DRAWING LINES
   =============  */

/** \brief Display a line in the default color from point x1y1 to x2y2. */
void line_d(ssh_coordinate x1,                                        /**< Horizontal coordinate of a start point. */
            ssh_coordinate y1,                                        /**< Vertical coordinate of a start point. */
            ssh_coordinate x2,                                        /**< Horizontal coordinate of an end point. */
            ssh_coordinate y2                                         /**< Vertical coordinate of an end point. */
            );

/** \brief Display a line in the specified color from point x1y1 to x2y2. */
/** \details From point x1y1 to x2y2. */
void line(ssh_coordinate x1,                                          /**< Horizontal coordinate of start point. */
          ssh_coordinate y1,                                          /**< Vertical coordinate of start point. */
          ssh_coordinate x2,                                          /**< Horizontal coordinate of end point. */
          ssh_coordinate y2,                                          /**< Vertical coordinate of end point. */
          ssh_color c                                                 /**< Color index for the line. */
          );

/** \brief Display a line in RGB color. */
/** \details From point x1y1 to x2y2. */
void line_rgb(ssh_coordinate x1,                                      /**< Horizontal coordinate of start point. */
              ssh_coordinate y1,                                      /**< Vertical coordinate of start point. */
              ssh_coordinate x2,                                      /**< Horizontal coordinate of end point. */
              ssh_coordinate y2,                                      /**< Vertical coordinate of end point. */
              ssh_intensity r,ssh_intensity g,ssh_intensity b         /**< Color components. */
              );

/* DRAWING CIRCLES, ELLIPSES AND ARCS
   ================================== */

/** \brief Display a circle with radius 'r' in the default color. */
void circle_d(ssh_coordinate x,                                       /**< Horizontal coordinate of a center. */
              ssh_coordinate y,                                       /**< Vertical coordinate of a center. */
              ssh_natural    r                                        /**< Radius of the circle. */
              );

/** \brief Display a circle in indexed color 'c'. */
void circle(ssh_coordinate x,                                         /**< Horizontal coordinate of a center. */
            ssh_coordinate y,                                         /**< Vertical coordinate of a center. */
            ssh_natural    r,                                         /**< Radius of the circle. */
            ssh_color      c                                          /**< Color index. */
            );

/** \brief Display a circle in a color specified by RGB components. */
void circle_rgb(ssh_coordinate x,                                     /**< Horizontal coordinate of a center. */
                ssh_coordinate y,                                     /**< Vertical coordinate of a center. */
                ssh_natural    r,                                     /**< Radius of the circle. */
                ssh_intensity rd,ssh_intensity gr,ssh_intensity bl    /**< Color components. */
                );

/** \brief Display an ellipse with SEMIAXES of length 'a' and 'b' in the default color. */
void ellipse_d(ssh_coordinate x,                                      /**< Horizontal coordinate of a center. */
               ssh_coordinate y,                                      /**< Vertical coordinate of a center. */
               ssh_natural    a,                                      /**< Length of SEMIAXIS 'a' (horizontal). */
               ssh_natural    b                                       /**< Length of SEMIAXIS 'b' (vertical). */
               );

/** \brief Display an ellipse with SEMIAXES of length 'a' and 'b' in color 'c'. */
void ellipse(ssh_coordinate x,                                        /**< Horizontal coordinate of a center. */
             ssh_coordinate y,                                        /**< Vertical coordinate of a center. */
             ssh_natural    a,                                        /**< Length of SEMIAXIS 'a' (horizontal). */
             ssh_natural    b,                                        /**< Length of SEMIAXIS 'b' (vertical). */
             ssh_color c                                              /**< Color index. */
             );

/** \brief Display an ellipse with SEMIAXES 'a' and 'b' in color specified by RGB components. */
void ellipse_rgb(ssh_coordinate x,                                    /**< Horizontal coordinate of a center. */
                 ssh_coordinate y,                                    /**< Vertical coordinate of a center. */
                 ssh_natural    a,                                    /**< Length of SEMIAXIS 'a' (horizontal). */
                 ssh_natural    b,                                    /**< Length of SEMIAXIS 'b' (vertical). */
                 ssh_intensity rd,ssh_intensity gr,ssh_intensity bl   /**< Color components. */
                 );

/** \brief Drawing a circular arc with radius 'r' in the default color. */
void arc_d(ssh_coordinate x,                                          /**< Horizontal coordinate of a center. */
           ssh_coordinate y,                                          /**< Vertical coordinate of a center. */
           ssh_natural    r,                                          /**< Radius of circle. */
           ssh_radian start,                                          /**< Start angle in radians. */
           ssh_radian  stop                                           /**< End angle in radians. */
           );

/** \brief Drawing a circular arc with radius 'r' in indexed color 'c'. */
void arc(ssh_coordinate x,                                         /**< Horizontal coordinate of a center. */
         ssh_coordinate y,                                         /**< Vertical coordinate of a center. */
         ssh_natural    r,                                         /**< Radius of circle. */
         ssh_radian start,                                         /**< Start angle in radians. */
         ssh_radian  stop,                                         /**< End angle in radians. */
         ssh_color c                                               /**< Color index. */
         );

/** \brief Drawing an elliptical arc with SEMIAXES 'a' and 'b' in the default color.  */
void earc_d(ssh_coordinate x,                                      /**< Horizontal coordinate of a center. */
            ssh_coordinate y,                                      /**< Vertical coordinate of a center. */
            ssh_natural a,                                         /**< Length of SEMIAXIS 'a' (horizontal). */
            ssh_natural b,                                         /**< Length of SEMIAXIS 'b' (vertical). */
            ssh_radian start,                                      /**< Start angle in radians. */
            ssh_radian stop                                        /**< End angle in radians. */
            );

/** \brief Drawing an elliptical arc with SEMIAXES 'a' and 'b' in indexed color 'c'. */
void earc(ssh_coordinate x,                                        /**< Horizontal coordinate of a center. */
          ssh_coordinate y,                                        /**< Vertical coordinate of a center. */
          ssh_natural a,                                           /**< Length of SEMIAXIS 'a' (horizontal). */
          ssh_natural b,                                           /**< Length of SEMIAXIS 'b' (vertical). */
          ssh_radian start,                                        /**< Start angle in radians. */
          ssh_radian stop,                                         /**< End angle in radians. */
          ssh_color c                                              /**< Color index. */
          );

/** \brief Fill a circle with radius "r" in default colors. */
void fill_circle_d(ssh_coordinate x,                               /**< Horizontal coordinate of a center. */
                   ssh_coordinate y,                               /**< Vertical coordinate of a center. */
                   ssh_natural    r                                /**< Radius of circle. */
                   );

/** \brief Fill a circle with indexed color 'c'. */
void fill_circle(ssh_coordinate x,                             /**< Horizontal coordinate of a center. */
                 ssh_coordinate y,                             /**< Vertical coordinate of a center. */
                 ssh_natural    r,                             /**< Radius of circle. */
                 ssh_color      c                              /**< Color index. */
                 );

/** \brief Fill an ellipse with SEMIAXES "a" and "b" in default color. */
void fill_ellipse_d(ssh_coordinate x,                          /**< Horizontal coordinate of a center. */
                    ssh_coordinate y,                          /**< Vertical coordinate of a center. */
                    ssh_natural a,                             /**< Length of SEMIAXIS 'a' (horizontal). */
                    ssh_natural b                              /**< Length of SEMIAXIS 'b' (vertical). */
                    );

/** \brief Fill an ellipse with SEMIAXES "a" and "b" in indexed color "c". */
void fill_ellipse(ssh_coordinate x,                            /**< Horizontal coordinate of a center. */
                  ssh_coordinate y,                            /**< Vertical coordinate of a center. */
                  ssh_natural a,                               /**< Length of SEMIAXIS 'a' (horizontal). */
                  ssh_natural b,                               /**< Length of SEMIAXIS 'b' (vertical). */
                  ssh_color c                                  /**< Color index. */
                  );

/** \brief Fill a circular arc with radius `r` in default color. */
void fill_arc_d(ssh_coordinate x,                              /**< Horizontal coordinate of virtual center. */
                ssh_coordinate y,                              /**< Vertical coordinate of virtual center. */
                ssh_natural r,                                 /**< Radius of circle. */
                ssh_radian start,                              /**< Start angle in radians. */
                ssh_radian stop,                               /**< End angle in radians. */
                ssh_bool pie                                   /**< Determines whether to fill as a pie slice. */
                );

/** \brief Fill a circular arc with radius `r` in indexed color 'c'. */
void fill_arc(ssh_coordinate x,                                /**< Horizontal coordinate of virtual center. */
              ssh_coordinate y,                                /**< Vertical coordinate of virtual center. */
              ssh_natural r,                                   /**< Radius of circle. */
              ssh_radian start,                                /**< Start angle in radians. */
              ssh_radian stop,                                 /**< End angle in radians. */
              ssh_bool pie,                                    /**< Determines whether to fill as a pie slice. */
              ssh_color c                                      /**< Color index. */
              );

/** \brief Fill an elliptical arc with semiaxes 'a' and 'b' in default color. */
void fill_earc_d(ssh_coordinate x,                             /**< Horizontal coordinate of virtual center. */
                 ssh_coordinate y,                             /**< Vertical coordinate of virtual center. */
                 ssh_natural a,                                /**< Length of virtual SEMIAXIS 'a' (horizontal). */
                 ssh_natural b,                                /**< Length of virtual SEMIAXIS 'b' (vertical). */
                 ssh_radian start,                             /**< Start angle in radians. */
                 ssh_radian stop,                              /**< End angle in radians. */
                 ssh_bool pie                                  /**< Determines whether to fill as a pie slice. */
                 );

/** \brief Fill an elliptical arc with semiaxes 'a' and 'b' in indexed color 'c'. */
void fill_earc(ssh_coordinate x,                               /**< Horizontal coordinate of virtual center. */
               ssh_coordinate y,                               /**< Vertical coordinate of virtual center. */
               ssh_natural a,                                  /**< Length of virtual SEMIAXIS 'a' (horizontal). */
               ssh_natural b,                                  /**< Length of virtual SEMIAXIS 'b' (vertical). */
               ssh_radian start,                               /**< Start angle in radians. */
               ssh_radian stop,                                /**< End angle in radians. */
               ssh_bool pie,                                   /**< Determines whether to fill as a pie slice. */
               ssh_color c                                     /**< Color index. */
               );


/* POLYGONS
   ======== */

/** \brief Fill a rectangle stretched between corners x1y1 and x2y2 in default color. */
void fill_rect_d(ssh_coordinate x1,                                  /**< Horizontal coordinate of a start corner. */
                 ssh_coordinate y1,                                  /**< Vertical coordinate of a start corner. */
                 ssh_coordinate x2,                                  /**< Horizontal coordinate of an end corner. */
                 ssh_coordinate y2                                   /**< Vertical coordinate of an end corner. */
                 );

/** \brief Fill a rectangle stretched between corners x1y1 and x2y2 in indexed color 'c'. */
void fill_rect(ssh_coordinate x1,                                    /**< Horizontal coordinate of a start corner. */
               ssh_coordinate y1,                                    /**< Vertical coordinate of a start corner. */
               ssh_coordinate x2,                                    /**< Horizontal coordinate of an end corner. */
               ssh_coordinate y2,                                    /**< Vertical coordinate of an end corner. */
               ssh_color c                                           /**< Color index. */
               );

/** \brief Fill a rectangle stretched between corners x1y1 and x2y2 in RGB color. */
void fill_rect_rgb(ssh_coordinate x1,                                /**< Horizontal coordinate of a start corner. */
                   ssh_coordinate y1,                                /**< Vertical coordinate of a start corner. */
                   ssh_coordinate x2,                                /**< Horizontal coordinate of an end corner. */
                   ssh_coordinate y2,                                /**< Vertical coordinate of an end corner. */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b   /**< Fill color specified by components. */
                   );

/** \brief Fills a polygon shifted by "vx", "vy" in default color. */
void fill_poly_d(ssh_coordinate vx,                                   /**< Horizontal component of translation vector. */
                 ssh_coordinate vy,                                   /**< Vertical component of translation vector. */
                 const ssh_point points[],                            /**< Array of polygon vertices. */
                 ssh_length length                                  /**< Array length. */
                 );

/** \brief Fills a polygon shifted by "vx", "vy" in indexed color 'c'. */
void fill_poly(ssh_coordinate vx,                                    /**< Horizontal component of translation vector. */
               ssh_coordinate vy,                                    /**< Vertical component of translation vector. */
               const ssh_point points[],                             /**< Array of polygon vertices. */
               ssh_length length,                                    /**< Array length. */
               ssh_color c                                           /**< Color index. */
               );



/* RETRIEVING CHARACTERS FROM KEYBOARD AND WINDOW EVENTS (INCLUDING MENUS)
   ======================================================================= */

ssh_mode  input_ready(); /**< \brief Function checking if there is something to take from input. */

ssh_msg   get_char();    /**< \brief Function for reading control characters and events.
                          * \return  Keyboard character index, special character, or menu code.
                          * '/r': Redrawing of at least a screen fragment is required.
                          * '/b': There is a mouse event to process.
                          * EOF: Graphics window closed.
                          * NNN: Number representing a menu command (usually large).
                          * '/0': Neutral character. Should be ignored.
                          * */

ssh_stat  set_char(ssh_msg ch); /**< \brief Sending a character back to input. \return Returns 0 if no space.
                                * \details Guaranteed to send back only one character! */

/** \brief Function reading the last mouse event. \return ??? */
ssh_stat  get_mouse_event(ssh_coordinate* x_pos,         /**< [out] Address to write horizontal cursor position. */
                          ssh_coordinate* y_pos,         /**< [out] Address to write vertical cursor position. */
                          ssh_coordinate* click          /**< [out] Address to write click information or 0.  */
                          );

/** \brief Function provides the area to be renewed upon '/r' request.
 * \return  Returns 0 if successful (TODO?)
 * If returns -1, no data or no implementation. Redraw everything.
 * If returns -2, data has already been read. Should be ignored. */
ssh_stat  repaint_area(ssh_coordinate* x,          /**< [out] Address to write horizontal coordinate of area corner. */
                       ssh_coordinate* y,          /**< [out] Address to write vertical coordinate of area corner. */
                       ssh_natural* width,         /**< [out] Address to write area width. */
                       ssh_natural* height         /**< [out] Address to write area height. */
                       );

/* CONTEXT MENU HANDLING */
/* ===================== */

/** \brief Structure for defining a simple menu. */
typedef struct ssh_menu_item_definition {
    const char* item_text;  /**< Menu line text. Can also be a label differing in that value is 0. */
    long long   item_value; /**< Value passed through `get_char()` function. For labels, 0. */
} ssh_menu_item_definition;

/** \brief Structure for passing absolute click position and other data to trigger a context menu. */
typedef struct ssh_basic_win_place_context {
    unsigned long long ScrIdentifier; /**< Display system identification data. E.g., Display handle in X11. */
    unsigned long long WinIdentifier; /**< Calling window identification data. E.g., Window handle in X11. */
    unsigned X; /**< Absolute horizontal `x` cursor position in display layout or -1 if cannot be calculated. */
    unsigned Y; /**< Absolute vertical `y` cursor position in display layout or -1 if cannot be calculated. */
} ssh_basic_win_place_context;

/** Function triggering a context menu after right-clicking.
 * Called from the library, from the event loop. The library user can propose their own version,
 * and the default version is located in the appropriate library source directory, e.g., "X11/wb_context_menu_expected_rofi.c"
 * @param x - horizontal coordinate of the mouse cursor.
 * @param y - vertical coordinate of the mouse cursor.
 * @param other_data - pointer to user data record containing at least Display handle and window handle.
 * @return 0 when a menu returned nothing or we expect a result later as a message.
 *  -1 when function declined handling and click should be passed to normal application handling (via `\b`).
 *  Any positive value is treated as a message to be returned by the `get_char` function.
 *  Any other negative value causes error info display, checking the `errno` variable value.
 * @details Function can be blocking or non-blocking (e.g., fire a separate thread). Basic implementation
 *  is provided by the SYMSHELL library, but defining one's own by the library user blocks linking the default version.
 */
extern long long ssh_context_menu_expected(unsigned x, unsigned y, struct ssh_basic_win_place_context* other_data);

/** Default context menu definition. In X11 provided from the library, but can be replaced at link level. */
extern ssh_menu_item_definition  context_menu_default[];

/** Number of items in the default context menu. Must accompany `context_menu_default`. */
extern unsigned context_menu_default_size;

#ifdef __cplusplus
} //extern C
#endif

#ifdef __cplusplus
static_assert( sizeof(uchar8b)==1 , "Type `uchar8b` has more than 1 byte" ); //???

/// \warning INLINE FUNCTIONS ARE ONLY AVAILABLE FROM C++ !!!\n
//TODO namespace SYMSHELL ???

/// \brief Building RGB value from components \return ssh_rgb
inline ssh_rgb RGB( ssh_intensity r,                                       /**< Red component. */
                    ssh_intensity g,                                       /**< Green component. */
                    ssh_intensity b                                        /**< Blue component. */
                    )
{
    ssh_rgb po;
    po.r=(uchar8b)(r & 0xff);
    po.g=(uchar8b)(g & 0xff);
    po.b=(uchar8b)(b & 0xff);
    return po;
}

/// \brief Temporary implementation set_background RGB. TODO REAL IMPLEMENTATION!
inline void set_background(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{   // TODO!
    set_background(r);
}

/// \brief Converter to references to avoid using addresses.
inline ssh_stat  get_mouse_event(int& xpos,int& ypos,int& click)
{
    return get_mouse_event(&xpos,&ypos,&click);
}

/// \brief Converter to references to avoid using addresses.
inline ssh_stat  repaint_area(ssh_coordinate& x, ssh_coordinate& y,ssh_natural& width, ssh_natural& height)
{
    return repaint_area(&x,&y,&width,&height);
}

/// \warning OBSOLETE FUNCTIONS ALSO AVAILABLE ONLY FOR C++ COMPILER
inline ssh_mode   get_buffering() { return 	buffered(); }        /**< Returns 1 if buffered. */
inline ssh_mode   get_fixed() { return fixed(); }                /**< Whether window has fixed size. */
inline ssh_color  get_background(void) { return background(); }  /**< Current background color... */
#endif

/// @}

/* ****************************************************************** */
/*                     SYMSHELLLIGHT 2026                             */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
#endif /* SYMSHELL_H_INCLUDED_ */
