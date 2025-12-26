#include "s21_string.h"

//	Находит первый символ в строке str1, который соответствует любому
// символу, указанному в str2.
char *s21_strpbrk(const char *str1, const char *str2) {
  if (str1 == s21_NULL || str2 == s21_NULL) return s21_NULL;

  for (const char *a = str1; *a; a++) {
    for (const char *b = str2; *b; b++) {
      if (*a == *b) return (char *)a;
    }
  }
  return s21_NULL;
}
