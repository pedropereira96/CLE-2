#include "helpers.h"

/**
 * @brief check if char is a vowel
 * 
 * @param c 
 * @return int 
 */
int isVowel(unsigned int c)
{
  int dictionary_size = 30;
  // a e i o u
  if (c == 0x61 || c == 0x65 || c == 0x69 || c == 0x6F  || c == 0x75) return 1; 
  
  // A E I O U
  if (c == 0x41  || c == 0x45 || c == 0x49  || c == 0x4F || c == 0x55) return 1;

  int accents[30] =  {0xC380, 0xC381, 0xC382, 0xC383, 0xC388, 0xC389, 0xC38A, 0xC38C, 0xC38D, 0xC392, 0xC393, 0xC394, 0xC395, 0xC399, 0xC39A, 0xC3A0, 0xC3A1, 0xC3A2, 0xC3A3, 0xC3A8, 0xC3A9, 0xC3AA, 0xC3AC, 0xC3AD, 0xC3B2, 0xC3B3, 0xC3B4, 0xC3B5, 0xC3B9, 0xC3BA};
  for (int i = 0; i < dictionary_size; i++)
    if (c == accents[i])
      return 1;

  return 0;
}

/**
 * @brief check if char is char is a consonant
 * 
 * @param c 
 * @return int 
 */
int isConsonant(unsigned int c)
{
  int lower_cedilha = 0xC3A7;
  int upper_cedilha = 0xC387;
  return !isVowel(c) && (((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) || c == lower_cedilha || c == upper_cedilha);
}

/**
 * @brief check if any char is a word separator
 * 
 * @param c 
 * @return int 
 */
int isSeparator(unsigned int c)
{
  int dash = 0xE28093;
  int ellipsis = 0xE280A6;
  int doubleQuotationMarkLeft = 0xE2809C;
  int doubleQuotationMarkRight = 0xE2809D;
  return c == '[' || c == ']' || c == '(' || c == ')' || c == '-' || c == '"' || c == 0x20 || c == 0x9 || c == 0xA || c == 0xD || c == '.' || c == '?' || c == ';' || c == ':' || c == ',' || c == '!' || c == dash || c == ellipsis || c == doubleQuotationMarkLeft || c == doubleQuotationMarkRight;
}


/**
 * @brief check if char is a number
 * 
 * @param c 
 * @return int 
 */
int isNumber(unsigned int c)
{
  return c >= 48 && c <= 57;
}


/**
 * @brief Read next char
 * 
 * @param file 
 * @return int 
 */
int read_char(FILE *file)
{
  unsigned char buffer[4] = {0, 0, 0, 0};
  unsigned int c = 0;
  fread(buffer, 1, 1, file);
  c = buffer[0];

  if ((buffer[0] >> 5) == 0b110)
  {
    fread(&buffer[1], 1, 1, file);
    c = (c << 8) | (buffer[1] & 0xff);
  }
  else if ((buffer[0] >> 4) == 0b1110)
  {
    fread(&buffer[1], 1, 2, file);
    c = (c << 8) | (buffer[1] & 0xff);
    c = (c << 8) | (buffer[2] & 0xff);
  }
  else if ((buffer[0] >> 3) == 0b11110)
  {
    fread(&buffer[1], 1, 3, file);
    c = (c << 8) | (buffer[1] & 0xff);
    c = (c << 8) | (buffer[2] & 0xff);
    c = (c << 8) | (buffer[2] & 0xff);
  }

  return c;
}



/**
 * @brief Get the needed bytes object to go back in the file
 * 
 * @param n 
 * @return long 
 */
long get_bytes(unsigned int n)
{
  if (n < 256)
    return 1;
  else if (n < 65536)
    return 2;
  else if (n < 16777216)
    return 3;
  return 4;
}