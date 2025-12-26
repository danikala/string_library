#include "s21_string.h"

// Функция копирования n символов из одной строки в другую
void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
  if (dest == s21_NULL || src == s21_NULL) return s21_NULL;

  unsigned char *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;
  for (s21_size_t i = 0; i < n; i++) d[i] = s[i];

  return dest;
}
