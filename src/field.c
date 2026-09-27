#include "field.h"
#include <math.h>

float FieldHeight(float x, float z, float ballX, float ballZ)
{
    const float dx = x - ballX, dz = z - ballZ;
    /* Visual Gaussian wells: no singularity, forces or relativistic physics. */
    return -4.2f*expf(-(x*x + z*z)/9.0f)
           -1.15f*expf(-(dx*dx + dz*dz)/2.0f);
}
