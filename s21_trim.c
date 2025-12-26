#include "s21_string.h"

// Возвращает новую строку, в которой удаляются все начальные и конечные
// вхождения набора заданных символов (trim_chars) из данной строки (src). В
// случае какой-либо ошибки следует вернуть значение NULL.
void *s21_trim(const char *src, const char *trim_chars) {
  if (!src || !trim_chars) return s21_NULL;

  const char *start = src;
  const char *end = src + s21_strlen(src) - 1;

  while (start <= end && s21_strchr(trim_chars, *start)) start++;

  while (end >= start && s21_strchr(trim_chars, *end)) end--;

  s21_size_t len = end - start + 1;

  char *result = (char *)malloc(len + 1);
  if (!result) return s21_NULL;

  s21_strncpy(result, start, len);
  result[len] = '\0';
  return result;
}
