namespace Photon {
    namespace Math {

        /* ----------------------------------------------------------
            Vector3 member functions 
        ---------------------------------------------------------*/
        template<typename T>
        inline Vector3<T>::Vector3(const Normal3<T>& normal)
            : x(normal.x), y(normal.y), z(normal.z) { }

        template<typename T>
        template<typename U>
        Vector3<T>::Vector3(const Vector3<U>& vec) {
            x = T(vec.x);
            y = T(vec.y);
            z = T(vec.z);
        }

        template<typename T>
        Vector3<T> Vector3<T>::operator+(const Vector3<T>& vec) const {
            return Vector3(x + vec.x, y + vec.y, z + vec.z);
        }

        template<typename T>
        Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        template<typename T>
        Vector3<T> Vector3<T>::operator-(const Vector3<T>& vec) const {
            return Vector3(x - vec.x, y - vec.y, z - vec.z);
        }

        template<typename T>
        Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        template<typename T>
        Vector3<T> Vector3<T>::operator*(const Vector3<T>& vec) const {
            return Vector3(x * vec.x, y * vec.y, z * vec.z);
        }

        template<typename T>
        Vector3<T>& Vector3<T>::operator*=(const Vector3<T>& vec) {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            return *this;
        }

        template<typename T>
        Vector3<T> Vector3<T>::operator*(T scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        template<typename T>
        Vector3<T>& Vector3<T>::operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        template<typename T>
        Vector3<T> Vector3<T>::operator/(T scalar) const {
            T recip = 1 / scalar;
            return Vector3(x * recip, y * recip, z * recip);
        }

        template<typename T>
        Vector3<T>& Vector3<T>::operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            z *= recip;
            return *this;
        }

        template<typename T>
        Vector3<T> Vector3<T>::operator-() const {
            return Vector3<T>(-x, -y, -z);
        }

        template<typename T>
        Vector3<T> Vector3<T>::recip() const {
            return Vector3<T>(1.0 / x, 1.0 / y, 1.0 / z);
        }

        template<typename T>
        Float Vector3<T>::lengthSqr() const {
            return x * x + y * y + z * z;
        }

        template<typename T>
        Float Vector3<T>::length() const {
            return std::sqrt(lengthSqr());
        }

        template<typename T>
        void Vector3<T>::normalize() {
            *this /= length();
        }

        template<typename T>
        T Vector3<T>::min() const {
            return std::min(x, std::min(y, z));
        }

        template<typename T>
        T Vector3<T>::max() const {
            return std::max(x, std::max(y, z));
        }

        template<typename T>
        uint32 Vector3<T>::maxDim() const {
            if (x > y) {
                if (x > z)
                    return 0;
                else
                    return 2;
            } else {
                if (y > z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        uint32 Vector3<T>::minDim() const {
            if (x < y) {
                if (x < z)
                    return 0;
                else
                    return 2;
            } else {
                if (y < z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        T Vector3<T>::operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        template<typename T>
        T& Vector3<T>::operator[](uint32 idx) {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        template<typename T>
        bool Vector3<T>::operator==(Vector3<T> vec) const {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        template<typename T>
        Float Vector3<T>::cube() const {
            return x * y * z;
        }

        template<typename T>
        bool Vector3<T>::isZero() const {
            return x == 0 && y == 0 && z == 0;
        }


        /* ----------------------------------------------------------
            Vector3 functions
        ---------------------------------------------------------*/
        template<typename T>
        inline Vector3<T> operator*(T scalar, const Vector3<T>& vec) {
            return vec * scalar;
        }

        template<typename T>
        inline Vector3<T> operator/(T scalar, const Vector3<T>& vec) {
            // Error, not allowed
            return Vector3<T>();
        }

        template<typename T>
        inline Vector3<T> abs(const Vector3<T>& vec) {
            return Vector3<T>(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
        }

        template<typename T>
        inline T dot(const Vector3<T>& vec1, const Vector3<T>& vec2) {
            return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
        }

        template<typename T>
        inline T absDot(const Vector3<T>& vec1, const Vector3<T>& vec2) {
            return std::abs(dot(vec1, vec2));
        }

        template<typename T>
        inline Vector3<T> cross(const Vector3<T>& vec1, const Vector3<T>& vec2) {
            Float vec1x = vec1.x, vec1y = vec1.y, vec1z = vec1.z;
            Float vec2x = vec2.x, vec2y = vec2.y, vec2z = vec2.z;

            return Vector3<T>((vec1y * vec2z) - (vec1z * vec2y),
                              (vec1z * vec2x) - (vec1x * vec2z),
                              (vec1x * vec2y) - (vec1y * vec2x));
        }

        template<typename T>
        inline Vector3<T> normalize(const Vector3<T>& vec) {
            return vec / vec.length();
        }

        template<typename T>
        inline Vector3<T> sign(const Vector3<T>& vec) {
            return Vector3<T>(sign(vec.x), sign(vec.y), sign(vec.z));
        }

        template<typename T>
        inline Vector3<T> min(const Vector3<T>& vec1, const Vector3<T>& vec2) {
            return Vector3<T>(std::min(vec1.x, vec2.x),
                              std::min(vec1.y, vec2.y),
                              std::min(vec1.z, vec2.z));
        }

        template<typename T>
        inline Vector3<T> max(const Vector3<T>& vec1, const Vector3<T>& vec2) {
            return Vector3<T>(std::max(vec1.x, vec2.x),
                              std::max(vec1.y, vec2.y),
                              std::max(vec1.z, vec2.z));
        }

        template<typename T>
        inline T min(const Vector3<T>& vec) {
            return std::min(vec.x, std::min(vec.y, vec.z));
        }

        template<typename T>
        inline T max(const Vector3<T>& vec) {
            return std::max(vec.x, std::max(vec.y, vec.z));
        }

        template<typename T>
        inline uint32 maxDim(const Vector3<T>& vec) {
            if (vec.x > vec.y) {
                if (vec.x > vec.z)
                    return 0;
                else
                    return 2;
            } else {
                if (vec.y > vec.z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        inline uint32 minDim(const Vector3<T>& vec) {
            if (vec.x < vec.y) {
                if (vec.x < vec.z)
                    return 0;
                else
                    return 2;
            } else {
                if (vec.y < vec.z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        inline void basisFromVector(const Vector3<T>& vec1, Vector3<T>* vec2, Vector3<T>* vec3) {
            // [Duff et. al, 2017] "Building an Orthonormal Basis, Revisited"
            const Float sign = std::copysign(1.0, vec1.z);
            const Float a = -1.0 / (sign + vec1.z);
            const Float b = vec1.x * vec1.y * a;

            *vec2 = Vec3(1.0 + sign * vec1.x * vec1.x * a, sign * b, -sign * vec1.x);
            *vec3 = Vec3(b, sign + vec1.y * vec1.y * a, -vec1.y);

            /*if (std::abs(vec1.x) > std::abs(vec1.y))
                *vec2 = Vector3<T>(-vec1.z, 0, vec1.x) / std::sqrt(vec1.x * vec1.x + vec1.z * vec1.z);
            else
                *vec2 = Vector3<T>(0, vec1.z, -vec1.y) / std::sqrt(vec1.y * vec1.y + vec1.z * vec1.z);
            *vec3 = cross(vec1, *vec2);*/
        }

        template<typename T>
        inline Vector3<T> clamp(Vector3<T> val, T low, T high) {
            Vector3<T> ret;
            for (int32 i = 0; i < 3; i++)
                ret[i] = clamp<T, T, T>(val[i], low, high);
            return ret;
        }

        template<typename T>
        inline Vector3<T> clamp(Vector3<T> val, Vector3<T> low, Vector3<T> high) {
            Vector3<T> ret;
            for (int32 i = 0; i < 3; i++)
                ret[i] = clamp<T, T, T>(val[i], low[i], high[i]);
            return ret;
        }

        template<typename T>
        inline Vector3<T> pow(Vector3<T> val, Float exp) {
            Vector3<T> ret;
            for (int32 i = 0; i < 3; i++)
                ret[i] = std::pow(val[i], (Float)exp);
            return ret;
        }

        template<typename T>
        inline Vector3<T> floor(Vector3<T> vec) {
            return Vector3<T>(std::floor(vec.x), std::floor(vec.y), std::floor(vec.z));
        }

        template<typename T>
        inline Vector3<T> ceil(Vector3<T> vec) {
            return Vector3<T>(std::ceil(vec.x), std::ceil(vec.y), std::ceil(vec.z));
        }


        /* ----------------------------------------------------------
            Vector2 member functions
        ---------------------------------------------------------*/
        template<typename T>
        template<typename U>
        Vector2<T>::Vector2(const Vector2<U>& vec) {
            x = T(vec.x);
            y = T(vec.y);
        }

        template<typename T>
        Vector2<T> Vector2<T>::operator+(const Vector2<T>& vec) const {
            return Vector2(x + vec.x, y + vec.y);
        }

        template<typename T>
        Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& vec) {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        template<typename T>
        Vector2<T> Vector2<T>::operator-(const Vector2<T>& vec) const {
            return Vector2(x - vec.x, y - vec.y);
        }

        template<typename T>
        Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            return *this;
        }

        template<typename T>
        Vector2<T> Vector2<T>::operator*(const Vector2<T>& vec) const {
            return Vector2<T>(x * vec.x, y * vec.y);
        }

        template<typename T>
        Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& vec) {
            x *= vec.x;
            y *= vec.y;
            return *this;
        }

        template<typename T>
        Vector2<T> Vector2<T>::operator*(T scalar) const {
            return Vector2(x * scalar, y * scalar);
        }

        template<typename T>
        Vector2<T>& Vector2<T>::operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        template<typename T>
        Vector2<T> Vector2<T>::operator/(T scalar) const {
            T recip = 1 / scalar;
            return Vector2(x * recip, y * recip);
        }

        template<typename T>
        Vector2<T>& Vector2<T>::operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            return *this;
        }

        template<typename T>
        Vector2<T> Vector2<T>::operator-() const {
            return Vector2<T>(-x, -y);
        }

        template<typename T>
        Float Vector2<T>::lengthSqr() const {
            return x * x + y * y;
        }

        template<typename T>
        Float Vector2<T>::length() const {
            return std::sqrt(lengthSqr());
        }

        template<typename T>
        void Vector2<T>::normalize() {
            *this /= length();
        }

        template<typename T>
        T Vector2<T>::min() const {
            return std::min(x, y);
        }

        template<typename T>
        T Vector2<T>::max() const {
            return std::max(x, y);
        }

        template<typename T>
        uint32 Vector2<T>::maxDim() const {
            if (x > y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        uint32 Vector2<T>::minDim() const {
            if (x < y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        T Vector2<T>::operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            return y;
        }

        template<typename T>
        T& Vector2<T>::operator[](uint32 idx) {
            if (idx == 0)
                return x;

            return y;
        }

        template<typename T>
        bool Vector2<T>::operator==(Vector2<T> vec) const {
            return x == vec.x && y == vec.y;
        }



        /* ----------------------------------------------------------
            Vector2 functions
        ---------------------------------------------------------*/
        template<typename T>
        inline Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
            return vec * scalar;
        }

        template<typename T>
        inline Vector2<T> operator/(T scalar, const Vector2<T>& vec) {
            // Error, not allowed
            return Vector2<T>();
        }

        template<typename T>
        inline Vector2<T> abs(const Vector2<T>& vec) {
            return Vector2<T>(std::abs(vec.x), std::abs(vec.y));
        }

        template<typename T>
        inline T dot(const Vector2<T>& vec1, const Vector2<T>& vec2) {
            return vec1.x * vec2.x + vec1.y * vec2.y;
        }

        template<typename T>
        inline T absDot(const Vector2<T>& vec1, const Vector2<T>& vec2) {
            return std::abs(dot(vec1, vec2));
        }

        template<typename T>
        inline Vector2<T> normalize(const Vector2<T>& vec) {
            return vec / vec.length();
        }

        template<typename T>
        inline Vector2<T> min(const Vector2<T>& vec1, const Vector2<T>& vec2) {
            return Vector2<T>(std::min(vec1.x, vec2.x),
                              std::min(vec1.y, vec2.y));
        }

        template<typename T>
        inline Vector2<T> max(const Vector2<T>& vec1, const Vector2<T>& vec2) {
            return Vector2<T>(std::max(vec1.x, vec2.x),
                              std::max(vec1.y, vec2.y));
        }

        template<typename T>
        inline T min(const Vector2<T>& vec) {
            return std::min(vec.x, vec.y);
        }

        template<typename T>
        inline T max(const Vector2<T>& vec) {
            return std::max(vec.x, vec.y);
        }

        template<typename T>
        inline uint32 maxDim(const Vector2<T>& vec) {
            if (vec.x > vec.y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        inline uint32 minDim(const Vector2<T>& vec) {
            if (vec.x < vec.y)
                return 0;
            else
                return 1;
        }



        /* ----------------------------------------------------------
            Point3T member functions
        ---------------------------------------------------------*/
        template<typename T>
        template<typename U>
        Point3T<T>::Point3T(const Point3T<U>& pt) {
            x = T(pt.x);
            y = T(pt.y);
            z = T(pt.z);
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator+(const Point3T<T>& pt) const {
            return Point3T<T>(x + pt.x, y + pt.y, z + pt.z);
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator+(const Vector3<T>& vec) const {
            return Point3T<T>(x + vec.x, y + vec.y, z + vec.z);
        }

        template<typename T>
        Point3T<T>& Point3T<T>::operator+=(const Vector3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator-(const Vector3<T>& vec) const {
            return Point3T<T>(x - vec.x, y - vec.y, z - vec.z);
        }

        template<typename T>
        Vector3<T> Point3T<T>::operator-(const Point3T<T>& pt) const {
            return Vector3<T>(x - pt.x, y - pt.y, z - pt.z);
        }

        template<typename T>
        Point3T<T>& Point3T<T>::operator-=(const Vector3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator*(const Point3T<T>& pt) const {
            return Point3T(x * pt.x, y * pt.y, z * pt.z);
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator*(T scalar) const {
            return Point3T(x * scalar, y * scalar, z * scalar);
        }

        template<typename T>
        Point3T<T>& Point3T<T>::operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator/(T scalar) const {
            T recip = 1 / scalar;
            return Point3T(x * recip, y * recip, z * recip);
        }

        template<typename T>
        Point3T<T>& Point3T<T>::operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            z *= recip;
            return *this;
        }

        template<typename T>
        Point3T<T> Point3T<T>::operator-() const {
            return Point3T<T>(-x, -y, -z);
        }

        template<typename T>
        Vector3<T> Point3T<T>::posVec() const {
            return Vector3<T>(x, y, z);
        }

        template<typename T>
        T Point3T<T>::min() const {
            return std::min(x, std::min(y, z));
        }

        template<typename T>
        T Point3T<T>::max() const {
            return std::max(x, std::max(y, z));
        }

        template<typename T>
        uint32 Point3T<T>::maxDim() const {
            if (x > y) {
                if (x > z)
                    return 0;
                else
                    return 2;
            } else {
                if (y > z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        uint32 Point3T<T>::minDim() const {
            if (x < y) {
                if (x < z)
                    return 0;
                else
                    return 2;
            } else {
                if (y < z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        T Point3T<T>::operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        template<typename T>
        T& Point3T<T>::operator[](uint32 idx) {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        template<typename T>
        bool Point3T<T>::operator==(Point3T<T> pt) const {
            return x == pt.x && y == pt.y && z == pt.z;
        }

        template<typename T>
        bool Point3T<T>::infinity() const {
            return (std::isinf(x) || std::isinf(y) || std::isinf(z));
        }


        /* ----------------------------------------------------------
            Point3T functions
        ---------------------------------------------------------*/
        template<typename T>
        inline Point3T<T> operator*(T scalar, const Point3T<T>& pt) {
            return pt * scalar;
        }

        template<typename T>
        inline Point3T<T> operator/(T scalar, const Point3T<T>& pt) {
            // Error, not allowed
            return Point3T();
        }

        template<typename T>
        inline Point3T<T> abs(const Point3T<T>& pt) {
            return Point3T<T>(std::abs(pt.x), std::abs(pt.y), std::abs(pt.z));
        }

        template<typename T>
        inline Point3T<T> min(const Point3T<T>& pt1, const Point3T<T>& pt2) {
            return Point3T<T>(std::min(pt1.x, pt2.x),
                              std::min(pt1.y, pt2.y),
                              std::min(pt1.z, pt2.z));
        }

        template<typename T>
        inline Point3T<T> max(const Point3T<T>& pt1, const Point3T<T>& pt2) {
            return Point3T<T>(std::max(pt1.x, pt2.x),
                              std::max(pt1.y, pt2.y),
                              std::max(pt1.z, pt2.z));
        }

        template<typename T>
        inline T min(const Point3T<T>& pt) {
            return std::min(pt.x, std::min(pt.y, pt.z));
        }

        template<typename T>
        inline T max(const Point3T<T>& pt) {
            return std::max(pt.x, std::max(pt.y, pt.z));
        }

        template<typename T>
        inline uint32 maxDim(const Point3T<T>& pt) {
            if (pt.x > pt.y) {
                if (pt.x > pt.z)
                    return 0;
                else
                    return 2;
            } else {
                if (pt.y > pt.z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        inline uint32 minDim(const Point3T<T>& pt) {
            if (pt.x < pt.y) {
                if (pt.x < pt.z)
                    return 0;
                else
                    return 2;
            } else {
                if (pt.y < pt.z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        inline Float dist(const Point3T<T>& pt1, const Point3T<T>& pt2) {
            return (pt1 - pt2).length();
        }

        template<typename T>
        inline Float distSqr(const Point3T<T>& pt1, const Point3T<T>& pt2) {
            return (pt1 - pt2).lengthSqr();
        }

        template<typename T>
        Point3T<T> lerp(Float t, const Point3T<T>& p1, const Point3T<T>& p2) {
            return (1 - t) * p1 + t * p2;
        }




        /* ----------------------------------------------------------
            Point2T member functions
        ---------------------------------------------------------*/
        template<typename T>
        Point2T<T> Point2T<T>::operator+(const Point2T<T>& pt) const {
            return Point2T<T>(x + pt.x, y + pt.y);
        }

        template<typename T>
        Point2T<T> Point2T<T>::operator+(const Vector2<T>& vec) const {
            return Point2T<T>(x + vec.x, y + vec.y);
        }

        template<typename T>
        Point2T<T>& Point2T<T>::operator+=(const Vector2<T>& vec) {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        template<typename T>
        Point2T<T> Point2T<T>::operator-(const Vector2<T>& vec) const {
            return Point2T<T>(x - vec.x, y - vec.y);
        }

        template<typename T>
        Vector2<T> Point2T<T>::operator-(const Point2T<T>& pt) const {
            return Vector2<T>(x - pt.x, y - pt.y);
        }

        template<typename T>
        Point2T<T>& Point2T<T>::operator-=(const Vector2<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            return *this;
        }

        template<typename T>
        Point2T<T> Point2T<T>::operator*(T scalar) const {
            return Point2T(x * scalar, y * scalar);
        }

        template<typename T>
        Point2T<T>& Point2T<T>::operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        template<typename T>
        Point2T<T> Point2T<T>::operator/(T scalar) const {
            T recip = 1 / scalar;
            return Vector2(x * recip, y * recip);
        }

        template<typename T>
        Point2T<T>& Point2T<T>::operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            return *this;
        }

        template<typename T>
        Point2T<T> Point2T<T>::operator-() const {
            return Point2T<T>(-x, -y);
        }

        template<typename T>
        Vector2<T> Point2T<T>::posVec() const {
            return Vector2<T>(x, y);
        }

        template<typename T>
        T Point2T<T>::min() const {
            return std::min(x, y);
        }

        template<typename T>
        T Point2T<T>::max() const {
            return std::max(x, y);
        }

        template<typename T>
        uint32 Point2T<T>::maxDim() const {
            if (x > y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        uint32 Point2T<T>::minDim() const {
            if (x < y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        T Point2T<T>::operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            return y;
        }

        template<typename T>
        T& Point2T<T>::operator[](uint32 idx) {
            if (idx == 0)
                return x;

            return y;
        }

        template<typename T>
        bool Point2T<T>::infinity() const {
            return (std::isinf(x) || std::isinf(y));
        }

        /* ----------------------------------------------------------
            Point2T functions
        ---------------------------------------------------------*/
        template<typename T>
        inline Point2T<T> operator*(T scalar, const Point2T<T>& pt) {
            return pt * scalar;
        }

        template<typename T>
        inline Point2T<T> operator/(T scalar, const Point2T<T>& pt) {
            // Error, not allowed
            return Point2T();
        }

        template<typename T>
        inline Point2T<T> abs(const Point2T<T>& pt) {
            return Point2T<T>(std::abs(pt.x), std::abs(pt.y));
        }

        template<typename T>
        inline Point2T<T> min(const Point2T<T>& pt1, const Point2T<T>& pt2) {
            return Point2T<T>(std::min(pt1.x, pt2.x),
                              std::min(pt1.y, pt2.y));
        }

        template<typename T>
        inline Point2T<T> max(const Point2T<T>& pt1, const Point2T<T>& pt2) {
            return Point2T<T>(std::max(pt1.x, pt2.x),
                              std::max(pt1.y, pt2.y));
        }

        template<typename T>
        inline T min(const Point2T<T>& pt) {
            return std::min(pt.x, pt.y);
        }

        template<typename T>
        inline T max(const Point2T<T>& pt) {
            return std::max(pt.x, pt.y);
        }

        template<typename T>
        inline uint32 maxDim(const Point2T<T>& pt) {
            if (pt.x > pt.y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        inline uint32 minDim(const Point2T<T>& pt) {
            if (pt.x < pt.y)
                return 0;
            else
                return 1;
        }

        template<typename T>
        inline Float dist(const Point2T<T>& pt1, const Point2T<T>& pt2) {
            return (pt1 - pt2).length();
        }

        template<typename T>
        inline Float distSqr(const Point2T<T>& pt1, const Point2T<T>& pt2) {
            return (pt1 - pt2).lengthSqr();
        }

        template<typename T>
        Point2T<T> lerp(Float t, const Point2T<T>& pt1, const Point2T<T>& pt2) {
            return (1 - t) * pt1 + t * pt2;
        }


        /* ----------------------------------------------------------
            Normal3 member functions
        ---------------------------------------------------------*/
        template<typename T>
        Normal3<T> Normal3<T>::operator+(const Normal3<T>& n) const {
            return Normal3<T>(x + n.x, y + n.y, z + n.z);
        }

        template<typename T>
        Normal3<T> Normal3<T>::operator+(const Vector3<T>& vec) const {
            return Vector3<T>(x + vec.x, y + vec.y, z + vec.z);
        }

        template<typename T>
        Normal3<T>& Normal3<T>::operator+=(const Normal3<T>& n) {
            x += n.x;
            y += n.y;
            z += n.z;
            return *this;
        }

        template<typename T>
        Normal3<T>& Normal3<T>::operator+=(const Vector3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        template<typename T>
        Normal3<T> Normal3<T>::operator-(const Normal3<T>& n) const {
            return Normal3(x - n.x, y - n.y, z - n.z);
        }

        template<typename T>
        Normal3<T> Normal3<T>::operator-(const Vector3<T>& vec) const {
            return Normal3(x - vec.x, y - vec.y, z - vec.z);
        }

        template<typename T>
        Normal3<T>& Normal3<T>::operator-=(const Normal3<T>& n) {
            x -= n.x;
            y -= n.y;
            z -= n.z;
            return *this;
        }

        template<typename T>
        Normal3<T>& Normal3<T>::operator-=(const Vector3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        template<typename T>
        Normal3<T> Normal3<T>::operator*(T scalar) const {
            return Normal3(x * scalar, y * scalar, z * scalar);
        }

        template<typename T>
        Normal3<T>& Normal3<T>::operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        template<typename T>
        Normal3<T> Normal3<T>::operator/(T scalar) const {
            T recip = 1 / scalar;
            return Normal3<T>(x * recip, y * recip, z * recip);
        }

        template<typename T>
        Normal3<T>& Normal3<T>::operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            z *= recip;
            return *this;
        }

        template<typename T>
        Normal3<T> Normal3<T>::operator-() const {
            return Normal3<T>(-x, -y, -z);
        }

        template<typename T>
        bool Normal3<T>::operator==(Normal3<T> vec) const {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        template<typename T>
        Float Normal3<T>::lengthSqr() const {
            return x * x + y * y + z * z;
        }

        template<typename T>
        Float Normal3<T>::length() const {
            return std::sqrt(lengthSqr());
        }

        template<typename T>
        void Normal3<T>::normalize() {
            *this /= length();
        }

        template<typename T>
        T Normal3<T>::min() const {
            return std::min(x, std::min(y, z));
        }

        template<typename T>
        T Normal3<T>::max() const {
            return std::max(x, std::max(y, z));
        }

        template<typename T>
        uint32 Normal3<T>::maxDim() const {
            if (x > y) {
                if (x > z)
                    return 0;
                else
                    return 2;
            } else {
                if (y > z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        uint32 Normal3<T>::minDim() const {
            if (x < y) {
                if (x < z)
                    return 0;
                else
                    return 2;
            } else {
                if (y < z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        T Normal3<T>::operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        template<typename T>
        T& Normal3<T>::operator[](uint32 idx) {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        /* ----------------------------------------------------------
            Normal3 functions
        ---------------------------------------------------------*/
        template<typename T>
        inline Normal3<T> operator*(T scalar, const Normal3<T>& n) {
            return n * scalar;
        }

        template<typename T>
        inline Normal3<T> operator/(T scalar, const Normal3<T>& n) {
            // Error, not allowed
            return Normal3<T>();
        }

        template<typename T>
        inline Normal3<T> abs(const Normal3<T>& n) {
            return Normal3<T>(std::abs(n.x), std::abs(n.y), std::abs(n.z));
        }

        template<typename T>
        inline T dot(const Normal3<T>& n1, const Normal3<T>& n2) {
            return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
        }

        template<typename T>
        inline T dot(const Normal3<T>& n, const Vector3<T>& vec) {
            return n.x * vec.x + n.y * vec.y + n.z * vec.z;
        }

        template<typename T>
        inline T dot(const Vector3<T>& vec, const Normal3<T>& n) {
            return vec.x * n.x + vec.y * n.y + vec.z * n.z;
        }

        template<typename T>
        inline T absDot(const Normal3<T>& n1, const Normal3<T>& n2) {
            return std::abs(dot(n1, n2));
        }

        template<typename T>
        inline T absDot(const Normal3<T>& n, const Vector3<T>& vec) {
            return std::abs(dot(vec, n));
        }

        template<typename T>
        inline Normal3<T> normalize(const Normal3<T>& n) {
            return n / n.length();
        }

        template<typename T>
        inline Normal3<T> min(const Normal3<T>& n1, const Normal3<T>& n2) {
            return Normal3<T>(std::min(n1.x, n2.x),
                              std::min(n1.y, n2.y),
                              std::min(n1.z, n2.z));
        }

        template<typename T>
        inline Normal3<T> max(const Normal3<T>& n1, const Normal3<T>& n2) {
            return Normal3<T>(std::max(n1.x, n2.x),
                              std::max(n1.y, n2.y),
                              std::max(n1.z, n2.z));
        }

        template<typename T>
        inline T min(const Normal3<T>& n) {
            return std::min(n.x, std::min(n.y, n.z));
        }

        template<typename T>
        inline T max(const Normal3<T>& n) {
            return std::max(n.x, std::max(n.y, n.z));
        }

        template<typename T>
        inline uint32 maxDim(const Normal3<T>& n) {
            if (n.x > n.y) {
                if (n.x > n.z)
                    return 0;
                else
                    return 2;
            } else {
                if (n.y > n.z)
                    return 1;
                else
                    return 2;
            }
        }

        template<typename T>
        inline uint32 minDim(const Normal3<T>& n) {
            if (n.x < n.y) {
                if (n.x < n.z)
                    return 0;
                else
                    return 2;
            } else {
                if (n.y < n.z)
                    return 1;
                else
                    return 2;
            }
        }
    }
}