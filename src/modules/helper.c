#include "../../include/modules/helper.h"

double limit(double amplitude, double limit) {
    if (amplitude >= limit) {
        return limit;
    } else if (amplitude <= -limit) {
        return -limit;
    }

    return amplitude;
}
