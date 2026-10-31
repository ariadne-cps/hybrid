// Regression: the public header must compile without an umbrella header.
#include "hybrid/hybrid_scalings.hpp"

int main() {
    Ariadne::SimpleHybridScalings scalings;
    (void)scalings;
    return 0;
}
