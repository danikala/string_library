#include "s21_string.h"

int s21_sscanf(const char *str, const char *format, ...) {
  if (str == NULL || format == NULL) return EOF;

  va_list argp;
  va_start(argp, format);

  int assigned_count = 0;
  int scan_finished = 0;
  const char *input_ptr = str;

  while (*format && !scan_finished) {
    if (isspace((unsigned char)*format)) {
      while (isspace((unsigned char)*input_ptr)) input_ptr++;
      format++;
    } else if (*format == '%') {
      format++;

      int is_suppressed = 0;
      int field_width = s21_get_width(&format, &is_suppressed);
      l_mod length_modifier = s21_parse_len(&format);

      if (*format == 'n') {
        s21_handle_n_specifier(str, input_ptr,
                               is_suppressed ? 0 : va_arg(argp, void *),
                               length_modifier);
        format++;
      } else if (*format == '%') {
        if (*input_ptr == '%')
          input_ptr++;
        else
          scan_finished = 1;
        format++;
      } else {
        int result = handle_assignment(*format, &input_ptr, is_suppressed, argp,
                                       field_width, length_modifier);
        format++;

        if (result == EOF && assigned_count == 0) {
          assigned_count = EOF;
          scan_finished = 1;
        } else if (result == EOF)
          scan_finished = 1;
        else if (result > 0 && !is_suppressed)
          assigned_count++;
        else if (result == 0 && !is_suppressed)
          scan_finished = 1;
      }
    } else {
      if (*input_ptr && *input_ptr == *format) {
        input_ptr++;
        format++;
      } else
        scan_finished = 1;
    }
  }
  va_end(argp);
  return assigned_count;
}

int s21_get_width(const char **format, int *is_suppressed) {
  int width = -1;
  *is_suppressed = 0;

  if (**format == '*') {
    *is_suppressed = 1;
    (*format)++;
  }

  if (isdigit(**format)) {
    char *endptr;
    long temp_width = strtol(*format, &endptr, 10);

    if (endptr == *format) temp_width = -1;

    width = (int)temp_width;
    *format = endptr;
  }

  return width;
}

l_mod s21_parse_len(const char **format) {
  l_mod length = LENGTH_NONE;

  if (**format == 'h') {
    (*format)++;
    length = (**format == 'h') ? ((*format)++, LENGTH_H_H) : LENGTH_H;
  } else if (**format == 'l') {
    (*format)++;
    length = (**format == 'l') ? ((*format)++, LENGTH_L_L) : LENGTH_L;
  } else if (**format == 'L') {
    length = LENGTH_L_BIG;
    (*format)++;
  }

  return length;
}

int handle_assignment(char specifier, const char **input_string_ptr,
                      int is_suppressed, va_list argp, int field_width,
                      l_mod length_modifier) {
  int result = 0;

  if (isspace(**input_string_ptr) && (specifier != 'c' && specifier != 's'))
    while (isspace(**input_string_ptr)) (*input_string_ptr)++;

  if (**input_string_ptr) {
    void *arg = NULL;
    if (!is_suppressed && specifier != 'p') arg = va_arg(argp, void *);

    switch (specifier) {
      case 'c':
        result = s21_handle_char_specifier(input_string_ptr, (char *)arg,
                                           field_width);
        break;
      case 's':
        result = s21_handle_string_specifier(input_string_ptr, (char *)arg,
                                             field_width);
        break;
      case 'd':
      case 'u':
      case 'o':
      case 'x':
      case 'X':
      case 'i': {
        l_mod adjusted_length_modifier = length_modifier;
        if (length_modifier == LENGTH_L_BIG)
          adjusted_length_modifier = LENGTH_L_L;
        result =
            s21_handle_int_specifiers(specifier, input_string_ptr, arg,
                                      field_width, adjusted_length_modifier);
        break;
      }
      case 'f':
      case 'F':
      case 'e':
      case 'E':
      case 'g':
      case 'G':
        result = s21_parse_float(input_string_ptr, field_width, length_modifier,
                                 arg);
        break;
      case 'p':
        result = s21_parse_pointer(input_string_ptr,
                                   (void **)va_arg(argp, void *), field_width);
        break;
      default:
        result = 0;
        break;
    }
  } else {
    result = EOF;
  }

  return result;
}

int s21_handle_char_specifier(const char **input_string_ptr, char *dest_ptr,
                              int field_width) {
  int chars_assigned = 0;

  if (!dest_ptr) {
    if (field_width == -1) {
      if (**input_string_ptr) (*input_string_ptr)++;
    } else
      while (field_width > 0 && **input_string_ptr) {
        (*input_string_ptr)++;
        field_width--;
      }
  } else {
    if (field_width < 0) {
      if (**input_string_ptr) {
        *dest_ptr = **input_string_ptr;
        (*input_string_ptr)++;
        chars_assigned = 1;
      }
    } else if (**input_string_ptr) {
      *dest_ptr = **input_string_ptr;
      (*input_string_ptr)++;
      chars_assigned = 1;

      for (int i = 1; i < field_width && **input_string_ptr; ++i) {
        (*input_string_ptr)++;
      }
    }
  }
  return chars_assigned;
}

int s21_handle_string_specifier(const char **input_ptr, char *dest_ptr,
                                int field_width) {
  while (isspace(**input_ptr)) (*input_ptr)++;
  int result = 0;

  if (!dest_ptr) {
    if (field_width == -1)
      while (!isspace(**input_ptr) && **input_ptr) (*input_ptr)++;
    else {
      while (!isspace(**input_ptr) && **input_ptr && field_width > 0) {
        (*input_ptr)++;
        field_width--;
      }
    }
  } else {
    int i = 0;
    while ((field_width < 0 || i < field_width) && !isspace(**input_ptr) &&
           **input_ptr) {
      *dest_ptr++ = **input_ptr;
      (*input_ptr)++;
      i++;
    }

    *dest_ptr = '\0';
    result = 1;
  }

  return result;
}

int s21_handle_int_specifiers(char spec_char, const char **input_ptr,
                              void *dest_ptr, int field_width,
                              l_mod length_modifier) {
  int result = 0;

  int base = (spec_char == 'x' || spec_char == 'X') ? 16
             : (spec_char == 'o')                   ? 8
             : (spec_char == 'i')                   ? 0
                                                    : 10;
  int is_signed = (spec_char == 'd' || spec_char == 'i') ? 1 : 0;

  if (dest_ptr)
    result = s21_scan_int(input_ptr, dest_ptr, field_width, length_modifier,
                          is_signed, base);
  else
    s21_suppress_int(input_ptr, field_width, base, is_signed);
  return result;
}

void s21_suppress_int(const char **input_ptr, int field_width, int base,
                      int is_signed) {
  if (base == 0) base = s21_get_int_base(*input_ptr);

  if (is_signed && (**input_ptr == '+' || **input_ptr == '-')) {
    (*input_ptr)++;
    if (field_width > 0) field_width--;
  }

  if (base == 16 && **input_ptr == '0' &&
      ((*(*input_ptr + 1) == 'x' || *(*input_ptr + 1) == 'X'))) {
    *input_ptr += 2;
    if (field_width > 0) field_width -= 2;
  }

  while ((field_width != 0) &&
         ((base == 16 && isxdigit(**input_ptr)) ||
          (base == 10 && isdigit(**input_ptr)) ||
          (base == 8 && **input_ptr >= '0' && **input_ptr <= '7'))) {
    (*input_ptr)++;
    if (field_width > 0) field_width--;
  }
}

int s21_scan_int(const char **input_ptr, void *dest_ptr, int field_width,
                 l_mod length_modifier, int is_signed, int base) {
  int result = 0;
  const char *start_ptr = *input_ptr;
  char *end_ptr;
  char *temp_str = s21_NULL;

  if (field_width > 0) {
    temp_str = (char *)malloc((size_t)(field_width + 1) * sizeof(char));
    if (!temp_str) return 0;

    size_t len = 0;
    while (**input_ptr && len < (size_t)field_width) {
      temp_str[len++] = **input_ptr;
      (*input_ptr)++;
    }
    temp_str[len] = '\0';

    *input_ptr = start_ptr;
    start_ptr = temp_str;
  }

  long long value = 0;

  if (is_signed)
    value = strtol(start_ptr, &end_ptr, base);
  else
    value = strtoul(start_ptr, &end_ptr, base);

  if (temp_str) free(temp_str);

  if (end_ptr == start_ptr)
    result = 0;
  else {
    *input_ptr += (end_ptr - start_ptr);

    s21_clamp_int_value(&value, length_modifier, is_signed);
    result =
        s21_convert_int_pointer(dest_ptr, value, length_modifier, is_signed);
  }

  return result;
}

int s21_get_int_base(const char *str) {
  int base = 10;

  if (*str == '-' || *str == '+') str++;

  if (*str == '0') {
    str++;
    if (*str)
      base = (*str == 'x' || *str == 'X')   ? 16
             : (*str >= '0' && *str <= '7') ? 8
                                            : 10;
  }

  return base;
}

void s21_clamp_int_value(long long *value, l_mod length_modifier,
                         int is_signed) {
  long long min_val = 0;
  unsigned long long max_val = 0;

  switch (length_modifier) {
    case LENGTH_H_H:
      min_val = is_signed ? SCHAR_MIN : 0;
      max_val = is_signed ? SCHAR_MAX : UCHAR_MAX;
      break;
    case LENGTH_H:
      min_val = is_signed ? SHRT_MIN : 0;
      max_val = is_signed ? SHRT_MAX : USHRT_MAX;
      break;
    case LENGTH_NONE:
      min_val = is_signed ? INT_MIN : 0;
      max_val = is_signed ? INT_MAX : UINT_MAX;
      break;
    case LENGTH_L:
      min_val = is_signed ? LONG_MIN : 0;
      max_val = is_signed ? LONG_MAX : ULONG_MAX;
      break;
    case LENGTH_L_L:
      min_val = is_signed ? LLONG_MIN : 0;
      max_val = is_signed ? LLONG_MAX : ULLONG_MAX;
      break;
    default:
      return;
  }

  if (*value < min_val) {
    if (length_modifier < LENGTH_L) {
      long long range = (long long)(max_val - min_val + 1);
      *value = min_val + (*value - min_val) % range;
      *value += range;
    } else if (is_signed)
      *value = min_val;
    else {
      unsigned long long unsigned_value = (unsigned long long)(*value);
      unsigned_value = max_val + 1 + *value;
      *value = (long long)unsigned_value;
    }
  } else if (*value > 0) {
    if ((unsigned long long)*value > max_val) {
      if (length_modifier < LENGTH_L) {
        long long range = (long long)(max_val - min_val + 1);
        *value = min_val + (*value - min_val) % range;
      } else
        *value = (long long)max_val;
    }
  }
}

int s21_convert_int_pointer(void *dest_ptr, long long value,
                            l_mod length_modifier, int is_signed) {
  switch (length_modifier) {
    case LENGTH_H_H:
      if (is_signed)
        *((signed char *)dest_ptr) = (signed char)value;
      else
        *((unsigned char *)dest_ptr) = (unsigned char)value;
      break;
    case LENGTH_H:
      if (is_signed)
        *((short *)dest_ptr) = (short)value;
      else
        *((unsigned short *)dest_ptr) = (unsigned short)value;
      break;
    case LENGTH_L:
      if (is_signed)
        *((long *)dest_ptr) = (long)value;
      else
        *((unsigned long *)dest_ptr) = (unsigned long)value;
      break;
    case LENGTH_L_L:
      if (is_signed)
        *((long long *)dest_ptr) = (long long)value;
      else
        *((unsigned long long *)dest_ptr) = (unsigned long long)value;
      break;
    default:
      if (is_signed)
        *((int *)dest_ptr) = (int)value;
      else
        *((unsigned int *)dest_ptr) = (unsigned int)value;
      break;
  }

  return 1;
}

int s21_parse_float(const char **str, int width, l_mod length, void *out_ptr) {
  int result = 0;
  char *temp_str = s21_NULL;
  char *end_ptr = s21_NULL;

  int effective_width = (width > 0) ? width : (int)s21_strlen(*str);
  int provided_width = (width > 0) ? width : 0;

  temp_str = (char *)malloc((effective_width + 1) * sizeof(char));
  if (temp_str) {
    int i = 0;
    const char *original_ptr = *str;

    while (**str && i < effective_width) {
      temp_str[i++] = **str;
      (*str)++;
    }
    temp_str[i] = '\0';
    *str = original_ptr;

    int valid_length = s21_valid_float_prefix_length(temp_str, provided_width);
    if (valid_length > 0) {
      temp_str[valid_length] = '\0';

      long double value = strtold(temp_str, &end_ptr);
      if (end_ptr != temp_str) {
        *str += valid_length;
        if (out_ptr) result = s21_convert_float_pointer(out_ptr, value, length);
      }
    }
    free(temp_str);
  }

  return result;
}

int s21_convert_float_pointer(void *destination_ptr, long double source_value,
                              l_mod conversion_length) {
  if (destination_ptr == s21_NULL) return 0;

  switch (conversion_length) {
    case LENGTH_L:
      *((double *)destination_ptr) = (double)source_value;
      break;
    case LENGTH_L_BIG:
      *((long double *)destination_ptr) = source_value;
      break;
    default:
      *((float *)destination_ptr) = (float)source_value;
      break;
  }

  return 1;
}

int s21_parse_pointer(const char **input, void **ptr, int width) {
  int result = 0;
  size_t len = s21_strlen(*input);
  int w = (width > 0) ? width : (int)len;
  if ((size_t)w > len) w = (int)len;

  char *buf = (char *)malloc(w + 1);
  if (buf) {
    s21_strncpy(buf, *input, w);
    buf[w] = '\0';
    char *p = buf;

    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
      p += 2;
      if (!(*p && isxdigit((unsigned char)*p))) {
        *input += (int)(p - buf);
        if (ptr) *ptr = s21_NULL;
        result = 1;
      } else {
        s21_size_t val = 0;
        int count = 0;
        while (*p) {
          char c = *p;
          int d;
          if (c >= '0' && c <= '9')
            d = c - '0';
          else if (c >= 'a' && c <= 'f')
            d = c - 'a' + 10;
          else if (c >= 'A' && c <= 'F')
            d = c - 'A' + 10;
          else
            break;
          val = val * 16 + d;
          count++;
          p++;
        }
        if (count) {
          *input += (int)(p - buf);
          if (ptr) *ptr = (void *)val;
          result = 1;
        }
      }
    }
    free(buf);
  }

  return result;
}

int s21_handle_n_specifier(const char *start, const char *current,
                           void *dest_ptr, l_mod length_modifier) {
  int result = 0;

  if (dest_ptr) {
    long long processed_chars_count =
        (long long)(s21_strlen(start) - s21_strlen(current));
    result = s21_convert_int_pointer(dest_ptr, processed_chars_count,
                                     length_modifier, 1);
  }

  return result;
}

int s21_valid_hex_float_prefix_length(const char *str) {
  int index = 0;

  if (str[index] == '+' || str[index] == '-') index++;

  if (str[index] == '0' && (str[index + 1] == 'x' || str[index + 1] == 'X')) {
    index += 2;

    int has_hex_digits = 0;

    while (isxdigit((unsigned char)str[index])) {
      has_hex_digits = 1;
      index++;
    }
    if (str[index] == '.') {
      index++;
      int after_dot = 0;
      while (isxdigit((unsigned char)str[index])) {
        after_dot = 1;
        index++;
      }
      has_hex_digits = has_hex_digits || after_dot;
    }
    if (!has_hex_digits) return 0;

    if (str[index] == 'p' || str[index] == 'P') {
      int exponent_start = index;
      index++;

      if (str[index] == '+' || str[index] == '-') index++;

      int exp_digits = 0;
      while (isdigit((unsigned char)str[index])) {
        exp_digits = 1;
        index++;
      }
      if (!exp_digits) index = exponent_start;
    }
  }
  return index;
}

int s21_valid_decimal_float_prefix_length(const char *str) {
  int index = 0;

  if (str[index] == '+' || str[index] == '-') index++;

  int digits_before_decimal = index;
  while (isdigit((unsigned char)str[index])) index++;

  if (str[index] == '.') {
    index++;
    while (isdigit((unsigned char)str[index])) index++;
  }
  if (index == digits_before_decimal) return 0;
  if (str[index] == 'e' || str[index] == 'E') {
    int exponent_start = index;
    index++;
    if (str[index] == '+' || str[index] == '-') index++;

    int exponent_digits = 0;
    while (isdigit((unsigned char)str[index])) {
      exponent_digits = 1;
      index++;
    }
    if (!exponent_digits) index = exponent_start;
  }

  return index;
}

int s21_valid_float_prefix_length(const char *input_str, int provided_width) {
  int prefix_length = 0;
  int i = 0;
  int input_len = s21_strlen(input_str);

  if (input_str[i] == '+' || input_str[i] == '-') i++;

  if (input_len >= i + 3 && (input_str[i] == 'N' || input_str[i] == 'n') &&
      (input_str[i + 1] == 'A' || input_str[i + 1] == 'a') &&
      (input_str[i + 2] == 'N' || input_str[i + 2] == 'n')) {
    prefix_length = i + 3;
  } else if (input_len >= i + 3 &&
             (input_str[i] == 'I' || input_str[i] == 'i') &&
             (input_str[i + 1] == 'N' || input_str[i + 1] == 'n') &&
             (input_str[i + 2] == 'F' || input_str[i + 2] == 'f')) {
    if (input_len >= i + 8 &&
        (input_str[i + 3] == 'I' || input_str[i + 3] == 'i') &&
        (input_str[i + 4] == 'N' || input_str[i + 4] == 'n') &&
        (input_str[i + 5] == 'I' || input_str[i + 5] == 'i') &&
        (input_str[i + 6] == 'T' || input_str[i + 6] == 't') &&
        (input_str[i + 7] == 'Y' || input_str[i + 7] == 'y')) {
      prefix_length = i + 8;
    } else {
      prefix_length = i + 3;
    }
  } else if (input_str[0] == '0' &&
             (input_str[1] == 'X' || input_str[1] == 'x')) {
    int min_len = 2;
    if (input_len == min_len) {
      prefix_length = (provided_width > 0)
                          ? s21_valid_decimal_float_prefix_length(input_str)
                          : 0;
    } else {
      int hex_len = s21_valid_hex_float_prefix_length(input_str);
      prefix_length = (hex_len > 0)
                          ? hex_len
                          : s21_valid_decimal_float_prefix_length(input_str);
    }
  } else {
    prefix_length = s21_valid_decimal_float_prefix_length(input_str);
  }

  return prefix_length;
}