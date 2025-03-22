class Quaternion
{
public:
    union
    {
        struct
        {
            float r;
            float i;
            float j;
            float k;
        };

        float data[4];
    };

    /**
     * Default constructor. Initializes the quaternion to the identity
     * quaternion (1, 0, 0, 0).
     */
    Quaternion() : r(1), i(0), j(0), k(0) {}

    /**
     * Explicit constructor. Initializes the quaternion with the given
     * components.
     *
     * @param r The real part of the quaternion.
     * @param i The first complex part of the quaternion.
     * @param j The second complex part of the quaternion.
     * @param k The third complex part of the quaternion.
     */
    Quaternion(float r, float i, float j, float k) : r(r), i(i), j(j), k(k) {}

    /**
     * Normalizes the quaternion to unit length to represent
     * a valid orientation quaternion.
     */
    void normalize()
    {
        float d = r * r + i * i + j * j + k * k;

        // Check for zero-length quaternion, and
        // use the identity quaternion in that case.
        if (d < 1e-6)
        {
            r = 1;
            i = j = k = 0;
            return;
        }

        d = 1.0f / sqrtf(d);

        r *= d;
        i *= d;
        j *= d;
        k *= d;
    }

    /**
     * Multiplies the quaternion by the given quaternion.
     *
     * @param multiplier The quaternion to multiply by.
     */
    void operator*=(const Quaternion &multiplier)
    {
        Quaternion q = *this;

        r = q.r * multiplier.r - q.i * multiplier.i -
            q.j * multiplier.j - q.k * multiplier.k;
        i = q.r * multiplier.i + q.i * multiplier.r +
            q.j * multiplier.k - q.k * multiplier.j;
        j = q.r * multiplier.j + q.j * multiplier.r +
            q.k * multiplier.i - q.i * multiplier.k;
        k = q.r * multiplier.k + q.k * multiplier.r +
            q.i * multiplier.j - q.j * multiplier.i;
    }

    /**
     * Rotates the quaternion by the given vector.
     *
     * @param vector The vector to rotate by.
     */
    void rotateByVector(const Vector3f &vector)
    {
        Quaternion q(0, vector.x, vector.y, vector.z);
        *this *= q;
    }

    /**
     * Adds the given vector to the quaternion, scaled by the
     * given amount.
     *
     * @param vector The vector to add.
     * @param scale The amount of the vector to add.
     */
    void addScaledVector(const Vector3f &vector, float scale)
    {
        Quaternion q(0, vector.x * scale, vector.y * scale, vector.z * scale);

        q *= *this;

        r += q.r * 0.5f;
        i += q.i * 0.5f;
        j += q.j * 0.5f;
        k += q.k * 0.5f;
    }
};