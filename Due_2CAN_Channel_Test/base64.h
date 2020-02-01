#ifndef BASE64_H
#define BASE64_H

class Base64 {
 public:
  /* encode_base64_length:
   *   Description:
   *     Calculates length of base64 string needed for a given number of binary bytes
   *   Parameters:
   *     input_length - Amount of binary data in bytes
   *   Returns:
   *     Number of base64 characters needed to encode input_length bytes of binary data
   */
  unsigned int encode_base64_length(unsigned int input_length);

  /* base64_to_binary:
   *   Description:
   *     Converts a single byte from a base64 character to the corresponding binary value
   *   Parameters:
   *     c - Base64 character (as ascii code)
   *   Returns:
   *     6-bit binary value
   */
  unsigned char binary_to_base64(unsigned char v);

  /* encode_base64:
   *   Description:
   *     Converts an array of bytes to a base64 null-terminated string
   *   Parameters:
   *     input - Pointer to input data
   *     input_length - Number of bytes to read from input pointer
   *     output - Pointer to output string. Null terminator will be added automatically
   *   Returns:
   *     Length of encoded string in bytes (not including null terminator)
   */
  unsigned int encode_base64(unsigned char input[], unsigned int input_length, unsigned char output[]);
};

#endif
