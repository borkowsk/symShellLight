/// @file
/// @brief Uniwersalne makro do zaznaczania być może nieużywanych obiektów.
//  Created by borkowsk on 17.04.26.
/// @date 2026-04-17 (last modification)
#ifndef SYMSHELLLIGHT_MAYBE_UNUSED_H
#define SYMSHELLLIGHT_MAYBE_UNUSED_H

/// Uniwersalne makro do zaznaczania być może nieużywanych obiektów.
/// Działa dla kompilatorów standardu C++17 albo starszych GNUC i clang. Dla pozostałych nie ma efektu.
/// \details Użycie:
/// ```
///  void funkcja(MAYBE_UNUSED int x) { }
/// ```
#if __cplusplus >= 201703L
#define MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

#endif //SYMSHELLLIGHT_MAYBE_UNUSED_H
