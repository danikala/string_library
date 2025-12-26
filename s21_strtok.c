#include "s21_string.h"

// Разбивает строку str на ряд токенов, разделенных delim
char *s21_strtok(char *str, const char *delim) {
  static char *str_temp = s21_NULL;
  if (str != s21_NULL) str_temp = str;

  if (str_temp == s21_NULL) return s21_NULL;
  while (*str_temp && s21_strchr(delim, *str_temp)) str_temp++;
  if (*str_temp == '\0') return s21_NULL;
  char *token_start = str_temp;
  while (*str_temp && !s21_strchr(delim, *str_temp)) str_temp++;
  if (*str_temp) {
    *str_temp = '\0';
    str_temp++;
  }
  return token_start;
}
