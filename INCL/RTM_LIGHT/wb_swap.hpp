/** @file
 * @brief Szablon zamiany dwóch wartości tego samego typu niezależnie od typu.
 * @date 2026-04-30 (last modification)
 * Created by borkowsk on 06.01.26.
 * @note Jeśli używasz biblioteki `SymShellLight` razem z pełną wersją WB_RTM to upewnij się, czy ten plik i jego
 *         wersja w pełnej bibliotece są zgodne. A najlepiej tak ustal kolejność ścieżek INCLUDE, żeby korzystać
 *         tylko z pełnej wersji.
 * ****************************************************************************************************************** */
#ifndef WB_SWAP_HPP_INCLUDED_
#define WB_SWAP_HPP_INCLUDED_

/**
 * @defgroup TypesService Informacja o typach i nazewnictwo
 * \brief Ładne i przenośne nazwy typów, limity typów itp.
 */
/// @{

/**
 * @brief Zamienia wartościami dwa obiekty typu T.
 *        Klasyczna implementacja zamiany (swap) wykorzystująca zmienną tymczasową.
 * @tparam T Typ obiektów podlegających zamianie.
 * @param[in,out] a Referencja do pierwszego obiektu.
 * @param[in,out] b Referencja do drugiego obiektu.
 * @details
 *  Implementacja ta, choć uniwersalna, może generować następujące problemy:
 *  * __Wydajność:__ Funkcja wykonuje trzy operacje kopiowania (konstrukcja kopiująca
 *                oraz dwa przypisania). Dla dużych obiektów (np. kontenery `std::vector` lub duże macierze)
 *                jest to operacja bardzo kosztowna czasowo w porównaniu do
 *                mechanizmu przenoszenia (move semantics) dostępnego od C++11.
 *  * __Wymagania typu:__ Typ `T` musi posiadać __publiczny konstruktor kopiujący__
 *                oraz operator przypisania. Jeśli typ `T` jest "move-only" (jak `std::unique_ptr`),
 *                ta funkcja się nie skompiluje.
 *  * __Wyjątki:__ Jeśli konstruktor kopiujący lub operator przypisania typu `T` może rzucić wyjątek,
 *                funkcja nie gwarantuje silnej odporności na błędy (ang. strong exception guarantee).
 *                W przypadku przerwania operacji w połowie, obiekty `a` i `b` mogą pozostać w stanie
 *                nieokreślonym lub niespójnym.
 *  * __Brak optymalizacji:__ W przeciwieństwie do `std::swap`, ta funkcja nie wykorzystuje specjalizacji dla typów,
 *                które można zamienić wydajniej (np. poprzez zamianę samych wskaźników wewnętrznych).
 *  */
template <class T>
inline void wb_swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}

#include <utility>
/**
 * @brief Zamienia wartościami dwa obiekty typu T, gwarantując silną odporność na błędy.
 * @details Funkcja implementuje mechanizm "copy-and-swap". Jeśli proces tworzenia
 *          kopii tymczasowych zawiedzie (rzuci wyjątek), obiekty wejściowe 'a' i 'b' pozostają w stanie nienaruszonym.
 * @note Wymaga, aby typ T posiadał konstruktor kopiujący oraz aby operacje przenoszenia (move) lub destruktor
 *       nie rzucały wyjątków (noexcept). Ponadto wymaga includa "utitility", a wtedy to już można `std::swap` używać :-D ...
 * @tparam T Typ obiektów.
 * @param a Pierwszy obiekt.
 * @param b Drugi obiekt.

 */
template <class T>
void wb_strong_swap(T& a, T& b) {
    // 1. Faza "Commit or Rollback": Tworzymy kopie tymczasowe.
    // Jeśli tutaj rzucony zostanie wyjątek, a i b są nietknięte.
    T temp_a = a;
    T temp_b = b;

    // 2. Faza bezwyjątkowa: Przenosimy gotowe kopie do miejsc docelowych.
    // Używamy std::move, zakładając że przenoszenie dla T jest noexcept.
    a = std::move(temp_b);
    b = std::move(temp_a);
}

/// @}

/* ****************************************************************** */
/*               WB_RTM for SymShell  version 2026                    */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                (Don't change or remove this note)  */
/* ****************************************************************** */
#endif //WB_SWAP_HPP_INCLUDED_

