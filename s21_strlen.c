#include "s21_string.h"

//	Вычисляет длину строки str, не включая завершающий нулевой символ.
s21_size_t s21_strlen(const char *str) {
  if (str == s21_NULL) return 0;
  int length = 0;
  while (str[length] != '\0') length++;
  return length;
}