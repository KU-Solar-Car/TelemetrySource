#ifndef VARIANT_H
#define VARIANT_H

#include <Arduino.h>

// Credit to Barry at https://stackoverflow.com/a/36267249/3882118.
// The following code block is licensed under the CC-BY-SA license: https://creativecommons.org/licenses/by-sa/3.0/
template <typename A>
constexpr size_t variadic_max() { return sizeof(A); }

template <typename A, typename B, typename... Cs>
constexpr size_t variadic_max()
{
    return sizeof(A) > sizeof(B) ? variadic_max<A, Cs...>() : variadic_max<B, Cs...>();
}

template <typename... Ts>
struct Variant {
  char data[variadic_max<Ts...>()];

  template <typename T>
  const T& operator=(const T& other)
  {
    memcpy(data, &other, sizeof(T));
    return *reinterpret_cast<T*>(data);
  }
  
  template <typename T>
  T& as()
  {
    return *reinterpret_cast<T*>(data);
  }
};
#endif
