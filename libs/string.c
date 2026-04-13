#include "string.h"
#include "math.h"

int my_strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

void my_strcpy(char *dst, const char *src) {
    int i = 0;
    while (src[i] != '\0') { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

int my_strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

void int_to_str(int n, char *buf, int buf_size) {
    if (buf_size <= 0) return;

    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }

    int negative = (n < 0);
    int val = my_abs(n);
    int i = 0;
    char tmp[12];

    while (val > 0 && i < 11) {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }

    int pos = 0;
    if (negative && pos < buf_size - 1) buf[pos++] = '-';

    int j = i - 1;
    while (j >= 0 && pos < buf_size - 1) buf[pos++] = tmp[j--];
    buf[pos] = '\0';
}

int str_to_int(const char *s) {
    int result = 0;
    int i = 0;
    while (s[i] >= '0' && s[i] <= '9') {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    return result;
}
