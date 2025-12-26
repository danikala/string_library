#include "s21_string.h"

// 	Сравнивает первые n байтов str1 и str2.
int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  if (str1 == s21_NULL || str2 == s21_NULL)
    return (str1 == s21_NULL) ? (str2 == s21_NULL ? 0 : -1) : 1;
  unsigned char *s1 = (unsigned char *)str1;
  unsigned char *s2 = (unsigned char *)str2;
  for (s21_size_t i = 0; i < n; i++) {
    if (*s1 != *s2) return *s1 - *s2;
    s1++;
    s2++;
  }
  return 0;
}
