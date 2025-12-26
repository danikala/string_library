#include "s21_string.h"

// Возвращает новую строку, в которой указанная строка (str) вставлена в
// указанную позицию (start_index) в данной строке (src). В случае какой-либо
// ошибки следует вернуть значение NULL.
void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  if (src == s21_NULL || str == s21_NULL) {
    return s21_NULL;
  }
  s21_size_t src_len = s21_strlen(src);
  s21_size_t str_len = s21_strlen(str);
  char *result = s21_NULL;
  if (src_len >= start_index) {
    result = (char *)malloc((src_len + str_len + 1) * sizeof(char));
    if (result != s21_NULL) {
      s21_strncpy(result, src, start_index);
      s21_strncpy(result + start_index, str, str_len);
      s21_strncpy(result + start_index + str_len, src + start_index,
                  src_len - start_index);
      result[src_len + str_len] = '\0';
    }
  }
  return result;
}
