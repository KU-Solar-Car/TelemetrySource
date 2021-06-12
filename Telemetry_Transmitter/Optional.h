#ifndef OPTIONAL_H
#define OPTIONAL_H

template <typename T>
struct Optional
{
  T value;
  bool present;
};

#endif
