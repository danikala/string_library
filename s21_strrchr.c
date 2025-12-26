#include "s21_string.h"

//	Выполняет поиск последнего вхождения символа c (беззнаковый тип) в
// строке, на которую указывает аргумент str.
char *s21_strrchr(const char *str, int c) {
  char *res = s21_NULL;
  if (str != s21_NULL) {
    for (int i = s21_strlen(str); res == s21_NULL && i >= 0; i--)
      if (str[i] == c) res = (char *)&str[i];
  }
  return res;
}