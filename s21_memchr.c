#include "s21_string.h"

//	Выполняет поиск первого вхождения символа c (беззнаковый тип) в первых n
// байтах строки, на которую указывает аргумент str.
void *s21_memchr(const void *str, int c, s21_size_t n) {
  s21_size_t success = n;
  const unsigned char *ptr_str = (const unsigned char *)str;
  if (str != s21_NULL) {
    s21_size_t i = 0;
    while (i < n && success == n) {
      if (ptr_str[i] == (unsigned char)c) {
        success = i;
      }
      i++;
    }
  }
  return success == n ? s21_NULL : (void *)(ptr_str + success);
}