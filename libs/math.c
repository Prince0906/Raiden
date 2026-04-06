#include "math.h"

int my_abs(int x) {
    return (x < 0) ? -x : x;
}

int my_min(int a, int b) {
    return (a < b) ? a : b;
}

int my_max(int a, int b) {
    return (a > b) ? a : b;
}

/* Clamp x to [lo, hi]. Uses my_min/my_max — no raw comparisons scattered. */
int my_clamp(int x, int lo, int hi) {
    return my_max(lo, my_min(x, hi));
}

/* Returns 1 if lo <= x <= hi, else 0. Used in collision detection (V2+). */
int in_bounds(int x, int lo, int hi) {
    return (x >= lo && x <= hi) ? 1 : 0;
}
