#include "s21_string.h"

int s21_sprintf(char *str, const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *p = str;
  while (*format) {
    if (*format == '%') {
      format++;
      s21_parser(&format, args, &p);
    } else {
      *p++ = *format;
    }
    format++;
  }
  *p = '\0';
  va_end(args);
  return p - str;
}

void s21_parser(const char **format, va_list args, char **p) {
  FormatSpecifier spec = {0};
  spec.width = -1;
  spec.precision = -1;
  s21_get_flags_etc(format, &spec, args);
  switch (**format) {
    case 'c':
      *p = s21_char_spec(args, *p, spec);
      break;
    case 'd':
    case 'i':
      *p = s21_decimal_spec(args, *p, spec);
      break;
    case 'f':
      *p = s21_float_spec(args, *p, spec);
      break;
    case 'e':
    case 'E':
      *p = s21_exponential_spec(args, *p, spec);
      break;
    case 's':
      *p = s21_string_spec(args, *p, spec);
      break;
    case 'x':
    case 'X':
      *p = s21_hexadecimal_spec(args, *p, spec);
      break;
    case 'G':
    case 'g':
      *p = s21_general_spec(args, *p, spec);
      break;
    case 'o':
      *p = s21_octal_spec(args, *p, spec);
      break;
    case 'p':
      *p = s21_pointer_spec(args, *p, spec);
      break;
    case 'u':
      *p = s21_unsigned_spec(args, *p, spec);
      break;
    case '%':
      *(*p)++ = '%';
      break;
    default:
      *(*p)++ = **format;
  }
}

char *s21_char_spec(va_list args, char *p, FormatSpecifier spec) {
  char c = (char)va_arg(args, int);
  if (spec.width > 1) {
    int spaces = spec.width - 1;
    if (spec.minus_flag) {
      *p++ = c;
      s21_write_fill_char(spaces, &p, ' ');
    } else {
      s21_write_fill_char(spaces, &p, ' ');
      *p++ = c;
    }
  } else {
    *p++ = c;
  }
  return p;
}

char *s21_general_spec(va_list args, char *p, FormatSpecifier spec) {
  va_list args_copy;
  va_copy(args_copy, args);
  double value = va_arg(args_copy, double);
  va_end(args_copy);
  if (fabs(value) < 1e-4 ||
      fabs(value) >= pow(10.0, spec.precision == -1 ? 6 : spec.precision)) {
    return s21_exponential_spec(args, p, spec);
  } else {
    return s21_float_spec(args, p, spec);
  }
}

char *s21_decimal_spec(va_list args, char *p, FormatSpecifier spec) {
  char buffer[64] = {0};
  int is_negative = 0;
  if (spec.precision >= 0) spec.zero_flag = 0;
  long num = s21_get_long(args, spec, 0);
  if (num < 0) {
    is_negative = 1;
    num *= -1;
  }
  char *buffer_end = buffer + sizeof(buffer) - 1;
  char *buffer_start = buffer_end;
  buffer_start = s21_convert_num(buffer_start, num, 10, '0');
  int len = buffer_end - buffer_start;
  int sign =
      is_negative ? '-' : (spec.plus_flag ? '+' : (spec.space_flag ? ' ' : 0));
  if (spec.zero_flag) {
    if (sign) {
      *p++ = sign;
      len++;
    }
    sign = 0;
  }
  return s21_write_convert_num(p, buffer_start, len + (sign > 0), spec, sign);
}

char *s21_pointer_spec(va_list args, char *p, FormatSpecifier spec) {
  s21_size_t num = va_arg(args, s21_size_t);
  char buffer[32] = {'\0'};
  char *buffer_end = buffer + sizeof(buffer) - 1;
  char *buffer_start = buffer_end;
  buffer_start = s21_convert_num(buffer_start, num, 16, 'a');
  int len = buffer_end - buffer_start + (spec.sharp_flag ? 2 : 0);
  return s21_write_convert_num(p, buffer_start, len, spec, 0);
}

char *s21_unsigned_spec(va_list args, char *p, FormatSpecifier spec) {
  unsigned long num = s21_get_long(args, spec, 1);
  char buffer[32] = {'\0'};
  char *buffer_end = buffer + sizeof(buffer) - 1;
  char *buffer_start = buffer_end;
  buffer_start = s21_convert_num(buffer_start, num, 10, '0');
  int len = buffer_end - buffer_start;
  return s21_write_convert_num(p, buffer_start, len, spec, 0);
}

char *s21_hexadecimal_spec(va_list args, char *p, FormatSpecifier spec) {
  unsigned long num = s21_get_long(args, spec, 1);
  char buffer[32] = {'\0'};
  char *buffer_end = buffer + sizeof(buffer) - 1;
  char *buffer_start = buffer_end;
  buffer_start =
      s21_convert_num(buffer_start, num, 16, spec.specifier == 'x' ? 'a' : 'A');
  int len = buffer_end - buffer_start;
  return s21_write_convert_num(p, buffer_start, len, spec, 0);
}

char *s21_octal_spec(va_list args, char *p, FormatSpecifier spec) {
  unsigned long num = s21_get_long(args, spec, 1);
  char buffer[64] = {'\0'};
  char *buffer_end = buffer + sizeof(buffer) - 1;
  char *buffer_start = buffer_end;
  buffer_start = s21_convert_num(buffer_start, num, 8, '0');
  int len = buffer_end - buffer_start;
  return s21_write_convert_num(p, buffer_start, len, spec, 0);
}

char *s21_string_spec(va_list args, char *p, FormatSpecifier spec) {
  const char *s = va_arg(args, const char *);
  if (s == s21_NULL) s = "(null)";

  int len = s21_strlen(s);
  int precision =
      (spec.precision >= 0 && spec.precision < len) ? spec.precision : len;
  int width =
      (spec.width > 0 && spec.width > precision) ? spec.width : precision;
  if (!spec.minus_flag) {
    s21_write_fill_char(width - precision, &p, ' ');
  }
  s21_strncpy(p, s, precision);
  p += precision;
  if (spec.minus_flag) {
    s21_write_fill_char(width - precision, &p, ' ');
  }
  return p;
}

char *s21_float_spec(va_list args, char *p, FormatSpecifier spec) {
  char fill_char = s21_get_fill_char(spec);
  long double num = s21_get_double(spec, args);
  int precision = spec.precision == -1 ? 6 : spec.precision;
  double rounding = s21_def_rounding(precision);
  int sign = num < 0 ? -1 : 1;
  num = sign * num + rounding;
  long long int_part = 0;
  long double fract_part = 0;
  int_part = (long long)num;
  fract_part = num - int_part;
  char int_str[64] = {'\0'};
  int int_len = s21_int_part_to_str(int_part, int_str);
  char fract_str[64] = {'\0'};
  int fract_len;
  if (spec.specifier == 'g' || spec.specifier == 'G') {
    int count = 0;
    int number = int_part;
    while (number > 0) {
      number /= 10;
      count++;
    }
    precision = 6 - count;
    long double multiplier = pow(10.0, precision);
    long double fract_part_round =
        round(fract_part * pow(10.0, precision)) / multiplier;
    fract_len = s21_fract_part_to_str(fract_part_round, precision, fract_str);
  } else {
    fract_len = s21_fract_part_to_str(fract_part, precision, fract_str);
  }
  if (precision == 0 && spec.sharp_flag) {
    s21_strncat(int_str, ".", 1);
    int_len++;
  }
  int total_len = int_len + fract_len;
  if (sign < 0 || spec.plus_flag || spec.space_flag) total_len++;
  int padding_len = spec.width - total_len;
  if (!spec.minus_flag) {
    if (spec.zero_flag) s21_write_sign(&p, sign, spec);
    if (spec.width > total_len) s21_write_fill_char(padding_len, &p, fill_char);
  }
  if (!spec.zero_flag) s21_write_sign(&p, sign, spec);

  s21_write_whole_num(&p, int_str, int_len, fract_str, fract_len);
  if (spec.minus_flag) s21_write_fill_char(padding_len, &p, fill_char);

  *p = '\0';
  return p;
}

char *s21_exponential_spec(va_list args, char *p, FormatSpecifier spec) {
  long double num = s21_get_double(spec, args);
  char e_or_E = spec.specifier;
  if (e_or_E == 'g' || e_or_E == 'G') {
    e_or_E = (char)(e_or_E - 2);
  }
  int precision = spec.precision == -1 ? 6 : spec.precision;
  double rounding = s21_def_rounding(precision);
  int sign = num < 0 ? -1 : 1;
  num *= sign;
  char fill_char = s21_get_fill_char(spec);
  int exponent = 0;
  if (num < 1) {
    while (num < 1 && num != 0) {
      exponent--;
      num *= 10;
    }
  } else {
    while (num >= 10) {
      exponent++;
      num /= 10;
    }
  }
  num += rounding;
  long long int_part = (long long)num;
  double fract_part = num - int_part;
  char int_str[3] = {'\0'};
  int int_len = (precision == 0 && spec.sharp_flag) ? 2 : 1;
  int_str[0] = '0' + (int)(int_part % 10);
  if (precision == 0 && spec.sharp_flag) {
    int_str[1] = '.';
  }
  char frac_str[100] = {'\0'};
  int frac_len = s21_fract_part_to_str(fract_part, precision, frac_str);
  int exp_len = exponent < 100 ? 4 : 5;
  char exp_str[6] = {'\0'};
  s21_get_exponential_notation(exp_str, exponent, e_or_E);
  int total_len = int_len + frac_len + exp_len;
  if (sign < 0 || spec.plus_flag || spec.space_flag) total_len++;

  int padding_len = spec.width - total_len;
  int left_align = spec.minus_flag;
  if (!left_align) {
    if (spec.zero_flag) s21_write_sign(&p, sign, spec);

    if (spec.width > total_len) s21_write_fill_char(padding_len, &p, fill_char);
  }
  if (!spec.zero_flag) s21_write_sign(&p, sign, spec);

  s21_write_whole_num(&p, int_str, int_len, frac_str, frac_len);
  s21_memcpy(p, exp_str, exp_len);
  p += exp_len;
  if (left_align == 1) s21_write_fill_char(padding_len, &p, fill_char);
  *p = '\0';
  return p;
}

void s21_get_flags_etc(const char **format, FormatSpecifier *spec,
                       va_list args) {
  int read_flags = 1;
  while (read_flags) {
    switch (**format) {
      case '-':
        spec->minus_flag = 1;
        break;
      case '+':
        spec->plus_flag = 1;
        break;
      case ' ':
        spec->space_flag = 1;
        break;
      case '#':
        spec->sharp_flag = 1;
        break;
      case '0':
        spec->zero_flag = 1;
        break;
      default:
        read_flags = 0;
        break;
    }
    if (read_flags) (*format)++;
  }
  if (spec->minus_flag) spec->zero_flag = 0;

  s21_get_width_precision(format, &spec->width, args);
  if (**format == '.') {
    (*format)++;
    s21_get_width_precision(format, &spec->precision, args);
    if (spec->precision < 0) spec->precision = 0;
  }
  if (**format == 'h' || **format == 'l' || **format == 'L') {
    spec->length = **format;
    (*format)++;
  }
  spec->specifier = **format;
}

void s21_get_width_precision(const char **format, int *num, va_list args) {
  if (**format == '*') {
    *num = va_arg(args, int);
    (*format)++;
  } else if (isdigit(**format)) {
    *num = 0;
    while (isdigit(**format)) {
      *num = *num * 10 + (**format - '0');
      (*format)++;
    }
  }
}

void s21_write_fill_char(int num, char **p, char fill) {
  for (int i = 0; i < num; i++) *(*p)++ = fill;
}

char s21_get_fill_char(FormatSpecifier spec) {
  return (spec.zero_flag) ? '0' : ' ';
}

int s21_int_part_to_str(long long num, char *buffer) {
  const char *digits = "0123456789";
  char temp[32];
  char *ptr = temp + sizeof(temp) - 1;
  *ptr = '\0';
  int len = 0;
  do {
    *(--ptr) = digits[num % 10];
    num /= 10;
    len++;
  } while (num);
  s21_strncpy(buffer, ptr, len);
  return len;
}

long double s21_get_double(FormatSpecifier spec, va_list args) {
  long double num = 0;
  if (spec.length == 'L')
    num = va_arg(args, long double);
  else
    num = va_arg(args, double);

  return num;
}

double s21_def_rounding(int precision) { return 0.5 * pow(10, -precision); }

int s21_fract_part_to_str(double fract_part, int precision, char *frac_str) {
  int len = 0;
  if (precision > 0) {
    frac_str[len++] = '.';
    for (int i = 0; i < precision; i++) {
      fract_part *= 10.0;
      int digit = (int)fract_part;
      frac_str[len++] = digit + '0';
      fract_part -= digit;
    }
  }
  return len;
}

void s21_get_exponential_notation(char *exp_str, int exponent, char e_or_E) {
  exp_str[0] = e_or_E;
  if (exponent < 0) {
    exp_str[1] = '-';
    exponent *= (-1);
  } else {
    exp_str[1] = '+';
  }
  if (exponent <= 9) {
    exp_str[2] = '0';
    exp_str[3] = '0' + exponent;
  } else if (exponent <= 99) {
    exp_str[2] = '0' + (exponent / 10);
    exp_str[3] = '0' + (exponent % 10);
  } else if (exponent <= 308) {
    exp_str[2] = '0' + (exponent / 100);
    exp_str[3] = '0' + ((exponent / 10) % 10);
    exp_str[4] = '0' + (exponent % 10);
  }
}

void s21_write_sign(char **p, int is_negative, FormatSpecifier spec) {
  if (is_negative < 0 || spec.plus_flag || spec.space_flag)
    *(*p)++ = is_negative < 0 ? '-' : spec.plus_flag ? '+' : ' ';
}

void s21_write_whole_num(char **p, const char *int_str, int int_len,
                         const char *frac_str, int frac_len) {
  s21_memcpy(*p, int_str, int_len);
  *p += int_len;
  if (frac_len) {
    s21_memcpy(*p, frac_str, frac_len);
    *p += frac_len;
  }
}

long s21_get_long(va_list args, FormatSpecifier spec, int unsign) {
  if (unsign) {
    unsigned long num = 0;
    if (spec.length == 'l') {
      num = va_arg(args, unsigned long);
    } else if (spec.length == 'h') {
      num = (unsigned short int)va_arg(args, unsigned int);
    } else {
      num = va_arg(args, unsigned int);
    }
    return (long)num;
  } else {
    long num = 0;

    if (spec.length == 'l') {
      num = va_arg(args, long);
    } else if (spec.length == 'h') {
      num = (short int)va_arg(args, int);
    } else {
      num = va_arg(args, int);
    }
    return num;
  }
}
char *s21_write_convert_num(char *p, const char *q, int len,
                            FormatSpecifier spec, char sign) {
  if ((spec.sharp_flag && (spec.specifier == 'x' || spec.specifier == 'X')) ||
      spec.specifier == 'p') {
    char temp[3] = {'\0'};
    temp[0] = '0';
    temp[1] = spec.specifier == 'p' ? 'x' : spec.specifier;
    s21_strncat(p, temp, 2);
    p += 2;
  }
  if (spec.sharp_flag && spec.specifier == 'o') *p++ = '0';

  int len_adjust = sign != 0;
  int num_fill_zero =
      spec.precision > len - len_adjust ? spec.precision - len + len_adjust : 0;
  int num_fill_char =
      spec.width > len + num_fill_zero ? spec.width - (len + num_fill_zero) : 0;
  char fill_char = spec.zero_flag ? '0' : ' ';
  if (spec.minus_flag) {
    if (sign) *p++ = sign;

    s21_write_fill_char(num_fill_zero, &p, '0');
    while (*q) *p++ = *q++;

    s21_write_fill_char(num_fill_char, &p, fill_char);
  } else {
    s21_write_fill_char(num_fill_char, &p, fill_char);
    if (sign) *p++ = sign;

    s21_write_fill_char(num_fill_zero, &p, '0');
    while (*q) *p++ = *q++;
  }
  return p;
}

char *s21_convert_num(char *q, unsigned long int num, int base,
                      char start_letter) {
  do {
    unsigned int digit = num % base;
    *--q = digit < 10 ? '0' + digit : start_letter + digit - 10;
    num /= base;
  } while (num);
  return q;
}