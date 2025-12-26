#ifndef S21_STRING
#define S21_STRING

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int s21_size_t;
#define s21_NULL ((void *)0)

void *s21_memchr(const void *str, int c, s21_size_t n);
int s21_memcmp(const void *str1, const void *str2, s21_size_t n);
void *s21_memcpy(void *dest, const void *src, s21_size_t n);
void *s21_memset(void *str, int c, s21_size_t n);
char *s21_strncat(char *dest, const char *src, s21_size_t n);
char *s21_strchr(const char *str, int c);
int s21_strncmp(const char *str1, const char *str2, s21_size_t n);
char *s21_strncpy(char *dest, const char *src, s21_size_t n);
s21_size_t s21_strcspn(const char *str1, const char *str2);
char *s21_strerror(int errnum);
s21_size_t s21_strlen(const char *str);
char *s21_strpbrk(const char *str1, const char *str2);
char *s21_strrchr(const char *str, int c);
char *s21_strstr(const char *haystack, const char *needle);
char *s21_strtok(char *str, const char *delim);
void *s21_to_upper(const char *str);
void *s21_to_lower(const char *str);
void *s21_insert(const char *src, const char *str, s21_size_t start_index);
void *s21_trim(const char *src, const char *trim_chars);

// s21_ssprintf
// Структура для хранения спецификатора формата
typedef struct {
  int plus_flag, minus_flag, sharp_flag, space_flag,
      zero_flag;  // Флаги (-, +, пробел, #, 0)
  int width;      // Ширина
  int precision;  // Точность
  int length;     // Длина (h, l, L)
  char specifier;  // Спецификатор (d, s, c, f, g, G, e, E, x, X, o, p, %)
} FormatSpecifier;

// Функция ssprintf
int s21_sprintf(char *buffer, const char *format, ...);

void s21_parser(const char **format, va_list args, char **p);
void s21_get_flags_etc(const char **format, FormatSpecifier *spec,
                       va_list args);
void s21_get_width_precision(const char **format, int *num, va_list args);
char *s21_char_spec(va_list args, char *p, FormatSpecifier spec);
void s21_write_fill_char(int num, char **p, char fill);
char *s21_decimal_spec(va_list args, char *p, FormatSpecifier spec);
int s21_int_part_to_str(long long num, char *buffer);
char s21_get_fill_char(FormatSpecifier spec);
char *s21_exponential_spec(va_list args, char *p, FormatSpecifier spec);
long double s21_get_double(FormatSpecifier spec, va_list args);
char *s21_hexadecimal_spec(va_list args, char *p, FormatSpecifier spec);
char *s21_string_spec(va_list args, char *p, FormatSpecifier spec);
char *s21_unsigned_spec(va_list args, char *p, FormatSpecifier spec);
char *s21_octal_spec(va_list args, char *p, FormatSpecifier spec);
char *s21_pointer_spec(va_list args, char *p, FormatSpecifier spec);
double s21_def_rounding(int precision);
int s21_fract_part_to_str(double fract_part, int precision, char *fract_str);
void s21_get_exponential_notation(char *exp_str, int exponent, char e_or_E);
void s21_write_sign(char **p, int is_negative, FormatSpecifier spec);
long s21_get_long(va_list args, FormatSpecifier spec, int unsign);
void s21_write_whole_num(char **p, const char *int_str, int int_len,
                         const char *frac_str, int frac_len);
char *s21_float_spec(va_list args, char *p, FormatSpecifier spec);
char *s21_write_convert_num(char *p, const char *q, int len,
                            FormatSpecifier spec, char sign);
char *s21_convert_num(char *q, unsigned long int num, int base,
                      char start_letter);
char *s21_general_spec(va_list args, char *p, FormatSpecifier spec);

// s21_sscanf
typedef enum {
  LENGTH_NONE,
  LENGTH_H_H,
  LENGTH_H,
  LENGTH_L,
  LENGTH_L_L,
  LENGTH_L_BIG
} l_mod;

int s21_sscanf(const char *str, const char *format, ...);

int s21_get_width(const char **format, int *is_suppressed);
l_mod s21_parse_len(const char **format);
int handle_assignment(char specifier, const char **str, int is_suppressed,
                      va_list argp, int width, l_mod length);
int s21_handle_char_specifier(const char **input_string_ptr, char *dest_ptr,
                              int field_width);
int s21_handle_string_specifier(const char **input_ptr, char *dest_ptr,
                                int field_width);
int s21_handle_int_specifiers(char spec_char, const char **input_ptr,
                              void *dest_ptr, int field_width,
                              l_mod length_modifier);
void s21_suppress_int(const char **input_ptr, int field_width, int base,
                      int is_signed);
int s21_get_int_base(const char *str);
int s21_scan_int(const char **input_ptr, void *dest_ptr, int field_width,
                 l_mod length_modifier, int is_signed, int base);
void s21_clamp_int_value(long long *value, l_mod length_modifier,
                         int is_signed);
int s21_convert_int_pointer(void *dest_ptr, long long value,
                            l_mod length_modifier, int is_signed);
int s21_parse_float(const char **str, int width, l_mod length, void *out_ptr);
int s21_convert_float_pointer(void *destination_ptr, long double source_value,
                              l_mod conversion_length);
int s21_parse_pointer(const char **input, void **ptr, int width);
int s21_handle_n_specifier(const char *start, const char *current,
                           void *dest_ptr, l_mod length_modifier);
int s21_valid_float_prefix_length(const char *str, int provided_width);
int s21_valid_decimal_float_prefix_length(const char *str);
#endif