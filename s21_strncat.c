#include "s21_string.h"

// Функция добавления строки в конец другой строки с ограничением длины
char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  if (dest == s21_NULL || src == s21_NULL) return s21_NULL;
  char *d = dest + s21_strlen(dest);
  s21_size_t i = 0;
  while (i < n && src[i] != '\0') {
    d[i] = src[i];
    i++;
  }
  d[i] = '\0';
  return dest;
}
