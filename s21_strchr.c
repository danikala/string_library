#include "s21_string.h"

//	Выполняет поиск первого вхождения символа c (беззнаковый тип) в строке,
// на которую указывает аргумент str.
char *s21_strchr(const char *str, int c) {
  return (char *)s21_memchr((const void *)str, c, s21_strlen(str) + 1);
}