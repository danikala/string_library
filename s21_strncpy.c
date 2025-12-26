#include "s21_string.h"

// Функция копирования до n символов из одной строки в другую
char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  s21_size_t i = 0;
  while (i < n && src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }
  if (i < n) dest[i] = '\0';
  return dest;
}
