typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;

#define SIDE_FRONT 0
#define SIDE_ON 2
#define SIDE_BACK 1
#define SIDE_CROSS -2

#define RANDOM_LONG(a, b) gEngfuncs.pfnRandomLong(a, b)
#define RANDOM_FLOAT(a, b) gEngfuncs.pfnRandomFloat(a, b)

#define GET_SCREEN_PIXEL(h, str) mathlib::GetScreenPixel(h ? ScreenHeight : ScreenWidth, atof(pSchemeData->GetResourceString(str)))


enum
{
	Q_PITCH = 0,	// up / down
	Q_YAW,		// left / right
	Q_ROLL		// fall over
};

struct mplane_s;
#define AngleIVectors mathlib::AngleVectorsTranspose

namespace mathlib {
	const double Q_PI = 3.14159265358979323846;
	constexpr double DOUBLE_PI = 2 * 3.14159265358979323846;
	const float COLINEAR_EPSILON = 0.001f;
	const int MAX_PLAYER_NAME_LENGTH = 128;
	const double RADIAN_PER_DEGREE = 57.29577951308232087684;
	const double DEGREE_PER_RADIAN = 0.017453292519943295769;
	const float PERIGON_ANGLE = 360.0f;
	const double SQRT_TWO = 1.414213562373095048801;
	const double SQRT_THREE = 1.732050807568877293527;
	const float FLAT_ANGLE = 180.0;
	const vec3_t vecZero = { 0, 0, 0 };
	const float METERS_PER_INCH = 0.0254f;
	constexpr int nanmask = 255 << 23;

	bool Q_IS_NAN(float x);
	float Q_DEG2RAD(float a);
	float Q_RAD2DEG(float r);
	template<typename T>
	T clamp(T num, T minn, T maxn);
	float METER2INCH(float x);
	float INCH2METER(float x);
	void VectorSubtract(vec3_t a, vec3_t b, vec3_t c);
	void VectorAdd(vec3_t a, vec3_t b, vec3_t c);
	void VectorAddFloat(vec3_t a, float f);
	void VectorMultipiler(vec3_t a, float f);
	void VectorCopy(vec3_t a, vec3_t b);
	void VectorReverse(vec3_t a);
	void Q_Vector2Copy(vec2_t a, vec2_t b);
	void VectorClear(vec3_t a);
	float DotProduct(vec3_t x, vec3_t y);
	float DotProduct(const vec3_t x, const vec3_t y);
	void ColorCalcuAlpha(int& r, int& g, int& b, int a);
	void Vector2RotateCASA(vec2_t out, float x, float y, float ca, float sa);
	template<typename T>
	T max3(T a, T b, T c);
	template<typename T>
	T min3(T a, T b, T c);

	void CenterPos2OpenGLPos(vec2_t pos, int w, int h);
	size_t GetScreenPixel(int length, double percent);
	void Vector2Rotate(vec2_t out, float x, float y, float rotate);

	void MatrixCopy(float in[4][3], float out[4][3]);
	void VectorMA(const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc);

	vec_t _DotProduct(vec3_t v1, vec3_t v2);
	void _VectorSubtract(vec3_t veca, vec3_t vecb, vec3_t out);
	void _VectorAdd(vec3_t veca, vec3_t vecb, vec3_t out);
	void _VectorCopy(vec3_t in, vec3_t out);

	int VectorCompare(const vec3_t v1, const vec3_t v2);
	float FVectorLength(vec3_t v);
	double VectorLength(vec3_t v);
	void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
	float VectorNormalize(vec3_t v);
	void VectorInverse(vec3_t v);
	void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
	int Q_log2(int val);
	void ClearBounds(vec3_t mins, vec3_t maxs);
	void ProjectPointOnPlane(vec3_t dst, const vec3_t p, const vec3_t normal);
	void PerpendicularVector(vec3_t dst, const vec3_t src);
	vec_t Q_rint(vec_t in);
	void _VectorScale(vec3_t v, vec_t scale, vec3_t out);
	vec_t Length(const vec3_t v);
	void AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs);
	void AngleIMatrix(const vec3_t angles, float matrix[3][4]);
	void AngleQuaternion(const vec3_t angles, vec4_t quaternion);
	void QuaternionMatrix(const vec4_t quaternion, float(*matrix)[4]);
	void QuaternionSlerp(const vec4_t p, vec4_t q, float t, vec4_t qt);
	void SinCos(float radians, float* sine, float* cosine);

	void R_ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3]);
	void R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
	void Matrix4x4_CreateFromEntity(float out[4][4], const vec3_t angles, const vec3_t origin, float scale);

	void FloorDivMod(double numer, double denom, int* quotient, int* rem);
	int GreatestCommonDivisor(int i1, int i2);
	void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);

	void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
	void AngleVectorsTranspose(const vec3_t angles, vec3_t* forward, vec3_t* right, vec3_t* up);
	void AngleMatrix(const vec3_t angles, float(*matrix)[4]);
	void VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out);

	void VectorAngles(const vec3_t forward, vec3_t angles);

	int InvertMatrix(const float* m, float* out);

	void Matrix4x4_ConcatTransforms(float out[4][4], float in1[4][4], float in2[4][4]);

	void VectorRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
	void VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out);

	bool PointInPolygen(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x, int y);
	float anglemod(float a);

	void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
	void RGBToHSV(int r, int g, int b, float& h, float& s, float& v);
	void HSVToRGB(float h, float s, float v, int& r, int& g, int& b);
	float fsqrt(float x);
	int fatoi(const char* src);
};