#include <Fresnel.h>

using namespace Photon;

Color Photon::fresnelSchlick(Color refl, Float cosI) {
    Float pCosI = 1 - cosI;
    pCosI *= (pCosI * pCosI) * (pCosI * pCosI);

    return refl + (Color(1) - refl) * pCosI;
}

Float Photon::fresnelDielectric(Float eta, Float cosI) {
    Float cos2T = 1.0 - (eta * eta) * (1.0 - cosI * cosI);
    if (cos2T <= 0)
        return 1; // Total internal reflection

    Float absCosI = std::abs(cosI);
    Float cosT = std::sqrt(cos2T);

    Float Rpl = (absCosI - eta * cosT) /
                (absCosI + eta * cosT);

    Float Rpp = (eta * absCosI - cosT) /
                (eta * absCosI + cosT);

    return (Rpl * Rpl + Rpp * Rpp) / 2.0;
}

Float Photon::fresnelDielectric(Float etaInt, Float etaExt, Float cosI) {
    Float eta = etaExt / etaInt;
    if (cosI < 0) // If we are leaving the surface, swap etas
        eta = 1.0 / eta;

    Float cos2T = 1.0 - (eta * eta) * (1.0 - cosI * cosI);
    if (cos2T <= 0)
        return 1; // Total internal reflection

    Float absCosI = std::abs(cosI);
    Float cosT = std::sqrt(cos2T);

    Float Rpl= (absCosI - eta * cosT) /
               (absCosI + eta * cosT);

    Float Rpp = (eta * absCosI - cosT) /
                (eta * absCosI + cosT);

    return (Rpl * Rpl + Rpp * Rpp) / 2.0;
}

Float Photon::fresnelDielectric(Float etaInt, Float etaExt, Float cosI, Float& cosT) {
    Float eta = etaExt / etaInt;
    if (cosI < 0) // If we are leaving the surface, swap etas
        eta = 1.0 / eta;

    Float cos2T = 1.0 - (eta * eta) * (1.0 - cosI * cosI);
    if (cos2T <= 0) { 
        cosT = 0;
        return 1; // Total internal reflection
    }

    Float absCosI = std::abs(cosI);
    cosT = std::sqrt(cos2T);

    Float Rpl = (absCosI - eta * cosT) /
                (absCosI + eta * cosT);

    Float Rpp = (eta * absCosI - cosT) /
                (eta * absCosI + cosT);

    // If we are entering the surface, 
    // the vector T is on the opposite side of the normal
    if (cosI > 0)
        cosT = -cosT;

    return (Rpl * Rpl + Rpp * Rpp) / 2.0;
}

Color Photon::fresnelConductor(const Color& eta, const Color& k, Float cosI) {
    Color cos2I = Color(cosI * cosI);
    Color sin2I = Color(1) - cos2I;
    Color sin4I = sin2I * sin2I;

    Color temp1 = eta * eta - k * k - sin2I;
    Color a2pb2 = sqrtSafe(temp1 * temp1 + k * k * eta * eta * 4.0);
    Color a = sqrtSafe((a2pb2 + temp1) * 0.5);

    Color term1 = a2pb2 + cos2I;
    Color term2 = 2.0 * a * cos2I;

    Color Rs2 = (term1 - term2) / (term1 + term2);

    Color term3 = a2pb2 * cos2I + sin4I;
    Color term4 = term2 * sin2I;

    Color Rp2 = Rs2 * (term3 - term4) / (term3 + term4);

    return (Rp2 + Rs2) / 2.0;
}