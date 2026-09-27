#include "field.h"
#include <math.h>
#include <stdio.h>
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "Failed: %s\n", #condition); return 1; } } while (0)
int main(void)
{
    CHECK(FieldHeight(0, 0, 8, 8) < -4.19f);
    CHECK(FieldHeight(5, 5, 5, 5) < FieldHeight(5, 5, -5, -5) - 1.14f);
    CHECK(fabsf(FieldHeight(0, 0, 0, 0) + 5.35f) < 0.0001f);
    CHECK(fabsf(FieldHeight(10, 10, -5, -5)) < 0.001f);
    for (int i = -100; i <= 100; ++i) {
        float h = FieldHeight(i*0.1f, 0, 0, 0);
        CHECK(isfinite(h) && h <= 0 && h >= -5.351f);
        CHECK(fabsf(h - FieldHeight(-i*0.1f, 0, 0, 0)) < 0.0001f);
    }
    puts("Field invariants passed");
    return 0;
}
