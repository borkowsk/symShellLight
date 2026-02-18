/// @file
/// Definicja flagi kończenia z zapytaniem.
//****************************************
#ifdef __cplusplus
extern "C" {
#endif

    /// \brief flaga kończenia z zapytaniem.
    /// \details
    /// Sterowanie reakcją na kończące błędy/wyjątki.
    /// Jest używana w bibliotece SymShell i wb_rtm, w tym w wb_ptr.
    /// Powinna być definiowana w programie głównym, ale w razie
    /// zapomnienia może być pobrana z biblioteki.
    int WB_error_enter_before_clean=0;

#ifdef  __cplusplus
}
#endif

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


