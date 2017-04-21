#pragma once

#include <PhotonMath.h>

namespace Photon {

    Float fresnelDielectric(Float n1, Float n2, Float cosI) {
        //cosI = Math::clamp(cosI, -1, 1);

        // Swap iors?
        if (cosI < 0) 
            std::swap(n1, n2);
        cosI = std::abs(cosI);

        Float sinI = Math::sqrtSafe(1 - cosI * cosI);
        Float sinT = n1 / n2 * sinI;

        // Check for total internal reflection
        if (sinT >= 1) 
            return 1;  // Reflection gets all contribution

        Float cosT = Math::sqrtSafe(1 - sinT * sinT);

        Float Rparl = ((n2 * cosI) - (n1 * cosT)) /
                      ((n2 * cosI) + (n1 * cosT));

        Float Rperp = ((n1 * cosI) - (n2 * cosT)) /
                      ((n1 * cosI) + (n2 * cosT));

        return (Rparl * Rparl + Rperp * Rperp) / 2;
    }



}