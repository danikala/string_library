#include "s21_string.h"

// ищет первое вхождение подстроки needle в строке haystack.
// Завершающий символ `\0' не сравнивается.
char *s21_strstr(const char *haystack, const char *needle) {
  if (haystack == s21_NULL || needle == s21_NULL) return s21_NULL;
  char *res = s21_NULL;

  for (s21_size_t i = 0; res == s21_NULL && haystack[i] != '\0'; i++) {
    s21_size_t j = 0;
    while (haystack[i + j] == needle[j] && needle[j] != '\0') j++;

    if (needle[j] == '\0') res = (char *)&haystack[i];
  }
  return res;
}
