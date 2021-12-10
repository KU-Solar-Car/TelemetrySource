#ifndef DEBUG

#define DEBUG_BUILD // Remove when using with DriverHUD

// Debug print macro. Do while loop ensures a semicolon is used.
#ifdef DEBUG_BUILD
  #define DEBUG(x) do { Serial.println(x); } while (0)
#else
  #define DEBUG(x) do {} while (0)
#endif

#endif