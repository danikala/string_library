#include "s21_string.h"

// Возвращает копию строки (str), преобразованной в верхний регистр
void *s21_to_lower(const char *str) {
  if (str == s21_NULL) return s21_NULL;
  s21_size_t len = s21_strlen(str);
  s21_size_t i = 0;
  char *result = malloc((len + 1) * sizeof(char));
  if (result == s21_NULL) return s21_NULL;
  while (i <= len) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      result[i] = str[i] + 32;
    } else {
      result[i] = str[i];
    }
    i++;
  }
  return result;
}
