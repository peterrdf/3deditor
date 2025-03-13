#pragma once

// ************************************************************************************************
#define PI 3.14159265

// ************************************************************************************************
struct _vector3d
{
    double x;
    double y;
    double z;
};

// ************************************************************************************************
struct _matrix
{
    double _11, _12, _13;
    double _21, _22, _23;
    double _31, _32, _33;
    double _41, _42, _43;
};

static void	_transform(const _vector3d* pV, const _matrix* pM, _vector3d* pOut)
{
    _vector3d vecTmp;
    vecTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    vecTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    vecTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

    pOut->x = vecTmp.x;
    pOut->y = vecTmp.y;
    pOut->z = vecTmp.z;
}

// mathematicsGeometryDoublePrecisionDerived.h
// Example: 30 degrees in the XY plane
// => MatrixRotateByEulerAngles(matrix, 0, 0, 2 * Pi * 30. / 360.);
static void	_matrixRotateByEulerAngles(
    _matrix* matrix,
    double	alpha,
    double	beta,
    double	gamma)
{
    //
    //	https://en.wikipedia.org/wiki/Rotation_matrix
    //
    double	cos_alpha = cos(alpha), sin_alpha = sin(alpha),
        cos_beta = cos(beta), sin_beta = sin(beta),
        cos_gamma = cos(gamma), sin_gamma = sin(gamma);

    matrix->_11 = 1. * cos_beta * cos_gamma;
    matrix->_12 = 1. * cos_beta * sin_gamma;
    matrix->_13 = -1. * sin_beta * 1.;

    matrix->_21 = sin_alpha * sin_beta * cos_gamma
        - cos_alpha * 1. * sin_gamma;
    matrix->_22 = sin_alpha * sin_beta * sin_gamma
        + cos_alpha * 1. * cos_gamma;
    matrix->_23 = sin_alpha * cos_beta * 1.;

    matrix->_31 = cos_alpha * sin_beta * cos_gamma
        + sin_alpha * 1. * sin_gamma;
    matrix->_32 = cos_alpha * sin_beta * sin_gamma
        - sin_alpha * 1. * cos_gamma;
    matrix->_33 = cos_alpha * cos_beta * 1.;

    matrix->_41 = 0.;
    matrix->_42 = 0.;
    matrix->_43 = 0.;
}

// ************************************************************************************************
// https://gist.github.com/zooty/f2222e0bc33e6845f068
class _vector3f
{

private:
    double x;
    double y;
    double z;

public:
    _vector3f()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    _vector3f(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }

    double length()
    {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    double length2()
    {
        return pow(length(), 2.);
    }

    _vector3f normalized()
    {
        double mag = length();

        return _vector3f(x / mag, y / mag, z / mag);
    }

    _vector3f neg()
    {
        return _vector3f(-x, -y, -z);
    }

    double dot(_vector3f other)
    {
        return x * other.getX() + y * other.getY() + z * other.getZ();
    }

    _vector3f cross(_vector3f other)
    {
        double x_ = y * other.getZ() - z * other.getY();
        double y_ = z * other.getX() - x * other.getZ();
        double z_ = x * other.getY() - y * other.getX();

        return _vector3f(x_, y_, z_);
    }

    _vector3f operator + (const _vector3f& other)
    {
        return _vector3f(x + other.x, y + other.y, z + other.z);
    }

    _vector3f operator - (const _vector3f& other)
    {
        return _vector3f(x - other.x, y - other.y, z - other.z);
    }

    _vector3f operator * (const _vector3f& other)
    {
        return _vector3f(x * other.x, y * other.y, z * other.z);
    }

    _vector3f operator * (double number)
    {
        return _vector3f(x * number, y * number, z * number);
    }

    _vector3f operator / (const _vector3f& other)
    {
        return _vector3f(x / other.x, y / other.y, z / other.z);
    }

    _vector3f operator / (double number)
    {
        return _vector3f(x / number, y / number, z / number);
    }
};

// ************************************************************************************************
struct _vector3
{
    double x;
    double y;
    double z;
};

// ************************************************************************************************
struct _matrix4x3
{
    double _11, _12, _13;
    double _21, _22, _23;
    double _31, _32, _33;
    double _41, _42, _43;
};

static void	_matrix4x3Transform(const _vector3* pIn, const _matrix4x3* pM, _vector3* pOut)
{
    _vector3 pTmp;
    pTmp.x = pIn->x * pM->_11 + pIn->y * pM->_21 + pIn->z * pM->_31 + pM->_41;
    pTmp.y = pIn->x * pM->_12 + pIn->y * pM->_22 + pIn->z * pM->_32 + pM->_42;
    pTmp.z = pIn->x * pM->_13 + pIn->y * pM->_23 + pIn->z * pM->_33 + pM->_43;

    pOut->x = pTmp.x;
    pOut->y = pTmp.y;
    pOut->z = pTmp.z;
}

static void _matrix4x3Identity(_matrix4x3* pM)
{
    assert(pM != nullptr);

    memset(pM, 0, sizeof(_matrix4x3));

    pM->_11 = pM->_22 = pM->_33 = 1.;
}

static double _matrixDeterminant(_matrix4x3* pM)
{
    double a = pM->_11 * pM->_22;
    double b = pM->_12 * pM->_23;
    double c = pM->_13 * pM->_21;
    double d = pM->_22 * pM->_31;
    double e = pM->_21 * pM->_33;
    double f = pM->_23 * pM->_32;

    double determinant =
        a * pM->_33 +
        b * pM->_31 +
        c * pM->_32 -
        pM->_13 * d -
        pM->_12 * e -
        pM->_11 * f;

    return determinant;
}

static void	_vector3Transform(
    _vector3* pOut,
    const _vector3* pV,
    const _matrix4x3* pM
)
{
    _vector3	pTmp;

    pTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    pTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    pTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

    pOut->x = pTmp.x;
    pOut->y = pTmp.y;
    pOut->z = pTmp.z;
}

static void	_matrix4x3Inverse(
    _matrix4x3* pOut,
    const _matrix4x3* pM
)
{
    _matrix4x3 pTmp;

    double	a, b, c, d, e, f, determinant;
    a = pM->_11 * pM->_22;
    b = pM->_12 * pM->_23;
    c = pM->_13 * pM->_21;
    d = pM->_22 * pM->_31;
    e = pM->_21 * pM->_33;
    f = pM->_23 * pM->_32;

    determinant = a * pM->_33 +
        b * pM->_31 +
        c * pM->_32 -
        pM->_13 * d -
        pM->_12 * e -
        pM->_11 * f;

    if (determinant < -0.0000000000000001 || determinant > 0.0000000000000001) {
        pTmp._11 = (pM->_22 * pM->_33 - f) / determinant;
        pTmp._12 = (pM->_13 * pM->_32 - pM->_12 * pM->_33) / determinant;
        pTmp._13 = (b - pM->_13 * pM->_22) / determinant;
        pTmp._21 = (pM->_23 * pM->_31 - e) / determinant;
        pTmp._22 = (pM->_11 * pM->_33 - pM->_13 * pM->_31) / determinant;
        pTmp._23 = (c - pM->_11 * pM->_23) / determinant;
        pTmp._31 = (pM->_21 * pM->_32 - d) / determinant;
        pTmp._32 = (pM->_12 * pM->_31 - pM->_11 * pM->_32) / determinant;
        pTmp._33 = (a - pM->_12 * pM->_21) / determinant;
        pTmp._41 = 0.;
        pTmp._42 = 0.;
        pTmp._43 = 0.;

        _vector3	vTmp;
        vTmp.x = -pM->_41;
        vTmp.y = -pM->_42;
        vTmp.z = -pM->_43;
        _vector3Transform(&vTmp, &vTmp, &pTmp);

        pOut->_11 = pTmp._11;
        pOut->_12 = pTmp._12;
        pOut->_13 = pTmp._13;

        pOut->_21 = pTmp._21;
        pOut->_22 = pTmp._22;
        pOut->_23 = pTmp._23;

        pOut->_31 = pTmp._31;
        pOut->_32 = pTmp._32;
        pOut->_33 = pTmp._33;

        pOut->_41 = vTmp.x;
        pOut->_42 = vTmp.y;
        pOut->_43 = vTmp.z;
    }
    else {
        _matrix4x3Identity(pOut);
    }
}

static void	_matrix4x3Inverse(
    _matrix4x3* pInOut
)
{
    _matrix4x3Inverse(
        pInOut,
        (const _matrix4x3*)pInOut
    );
}

static void	_matrix4x3Multiply(_matrix4x3* pOut, const _matrix4x3* pM1, const _matrix4x3* pM2)
{
    assert((pOut != nullptr) && (pM1 != nullptr) && (pM2 != nullptr));

    _matrix4x3 pTmp;
    pTmp._11 = pM1->_11 * pM2->_11 + pM1->_12 * pM2->_21 + pM1->_13 * pM2->_31;
    pTmp._12 = pM1->_11 * pM2->_12 + pM1->_12 * pM2->_22 + pM1->_13 * pM2->_32;
    pTmp._13 = pM1->_11 * pM2->_13 + pM1->_12 * pM2->_23 + pM1->_13 * pM2->_33;

    pTmp._21 = pM1->_21 * pM2->_11 + pM1->_22 * pM2->_21 + pM1->_23 * pM2->_31;
    pTmp._22 = pM1->_21 * pM2->_12 + pM1->_22 * pM2->_22 + pM1->_23 * pM2->_32;
    pTmp._23 = pM1->_21 * pM2->_13 + pM1->_22 * pM2->_23 + pM1->_23 * pM2->_33;

    pTmp._31 = pM1->_31 * pM2->_11 + pM1->_32 * pM2->_21 + pM1->_33 * pM2->_31;
    pTmp._32 = pM1->_31 * pM2->_12 + pM1->_32 * pM2->_22 + pM1->_33 * pM2->_32;
    pTmp._33 = pM1->_31 * pM2->_13 + pM1->_32 * pM2->_23 + pM1->_33 * pM2->_33;

    pTmp._41 = pM1->_41 * pM2->_11 + pM1->_42 * pM2->_21 + pM1->_43 * pM2->_31 + pM2->_41;
    pTmp._42 = pM1->_41 * pM2->_12 + pM1->_42 * pM2->_22 + pM1->_43 * pM2->_32 + pM2->_42;
    pTmp._43 = pM1->_41 * pM2->_13 + pM1->_42 * pM2->_23 + pM1->_43 * pM2->_33 + pM2->_43;

    pOut->_11 = pTmp._11;
    pOut->_12 = pTmp._12;
    pOut->_13 = pTmp._13;

    pOut->_21 = pTmp._21;
    pOut->_22 = pTmp._22;
    pOut->_23 = pTmp._23;

    pOut->_31 = pTmp._31;
    pOut->_32 = pTmp._32;
    pOut->_33 = pTmp._33;

    pOut->_41 = pTmp._41;
    pOut->_42 = pTmp._42;
    pOut->_43 = pTmp._43;
}

// ************************************************************************************************
struct _matrix4x4
{
    double _11, _12, _13, _14;
    double _21, _22, _23, _24;
    double _31, _32, _33, _34;
    double _41, _42, _43, _44;
};

static void _matrix4x4Identity(_matrix4x4* pM)
{
    assert(pM != nullptr);

    memset(pM, 0, sizeof(_matrix4x4));

    pM->_11 = pM->_22 = pM->_33 = pM->_44 = 1.;
}

static void	_transform(const _vector3* pV, const _matrix4x4* pM, _vector3* pOut)
{
    _vector3 pTmp;
    pTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    pTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    pTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

    pOut->x = pTmp.x;
    pOut->y = pTmp.y;
    pOut->z = pTmp.z;
}

// ************************************************************************************************
template<class T>
class _buffer
{

private: // Members

    T* m_pData;
    int64_t m_iSize;

public: // Methods

    _buffer()
        : m_pData(nullptr)
        , m_iSize(0)
    {}

    virtual ~_buffer()
    {
        delete[] m_pData;
    }

public: // Properties

    T*& data() { return m_pData; }
    int64_t& size() { return m_iSize; }
};

// ************************************************************************************************
template<class V>
class _vertexBuffer : public _buffer<V>
{

private: // Members

    uint32_t m_iVertexLength;

public: // Methods

    _vertexBuffer(uint32_t iVertexLength)
        : m_iVertexLength(iVertexLength)
    {
        static_assert(
            is_same<V, float>::value ||
            is_same<V, double>::value,
            "V must be float or double type.");
        assert(iVertexLength >= 3);
    }

    virtual ~_vertexBuffer()
    {}

    void copyFrom(_vertexBuffer* pSource)
    {
        assert(pSource != nullptr);
        assert(_buffer<V>::size() == pSource->size());
        assert(getVertexLength() == pSource->getVertexLength());

        memcpy(_buffer<V>::data(), pSource->data(), (uint32_t)_buffer<V>::size() * getVertexLength() * (uint32_t)sizeof(V));
    }

public: // Properties

    uint32_t getVertexLength() { return m_iVertexLength; }
};

// ************************************************************************************************
typedef _vertexBuffer<float> _vertices_f;
typedef _vertexBuffer<double> _vertices_d;

// ************************************************************************************************
template<class I>
class _indexBuffer : public _buffer<I>
{

public: // Methods

    _indexBuffer()
    {
        static_assert(
            is_same<I, int32_t>::value ||
            is_same<I, int64_t>::value,
            "I must be int32_t or int64_t type.");
    }

    virtual ~_indexBuffer()
    {}
};

// ************************************************************************************************
typedef _indexBuffer<int32_t> _indices_i32;
typedef _indexBuffer<int64_t> _indices_i64;