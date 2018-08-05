#pragma once
#include <math.h>
#include <limits>
#include <DirectXMath.h>

namespace Lobelia {
	/**
	*@brief floatの等値判定
	*@param[in] 左辺値
	*@param[in] 右辺値
	*@param[in] 許容値 デフォルトではものすごく小さい値が設定される
	*/
	__forceinline bool FloatEqual(float x, float y, float threshold = FLT_EPSILON) noexcept { return  (fabsf(x - y) <= threshold); };

	struct Vector2 {
	public:
		union {
			struct { float x, y; };
			float v[2];
		};
		/**
		*@brief 型がintのVector2<br>
		*値を取る為のみ使用なので、メンバ関数は提供しない
		*/
		struct IVector2 {
			union {
				struct { int x, y; };
				int v[2];
			};
			__forceinline IVector2(int x, int y) :x(x), y(y) {}
		};
		__forceinline Vector2(float x, float y) noexcept : x(x), y(y) {}
		__forceinline Vector2(IVector2 v) noexcept : Vector2(static_cast<float>(v.x), static_cast<float>(v.y)) {}
		__forceinline Vector2()noexcept : Vector2(0.0f, 0.0f) {}
		/**
		*@brief 値をint型で取得する関数
		*/
		__forceinline IVector2 Get()const  noexcept { return IVector2(static_cast<int>(x), static_cast<int>(y)); }
		/**
		*@brief int型のIVector2からの直接Vector2に代入
		*/
		__forceinline void Set(IVector2 v)noexcept {
			for (int i = 0; i < 2; i++) {
				this->v[i] = static_cast<float>(v.v[i]);
			}
		}
		/**@brief 距離(平方根あり)*/
		__forceinline float Length()const { return sqrtf(x*x + y * y); }
		/**@brief 距離(平方根なし)*/
		__forceinline float LengthSq()const { return (x*x + y * y); }
		/**@brief 正規化*/
		__forceinline void Normalize() {
			float l = Length();
			if (l != 0.0f) { x /= l;	y /= l; }
		}
		__forceinline bool IsEqual(const Vector2& v) { return (FloatEqual(x, v.x) && FloatEqual(y, v.y)); }
		/**@brief 内積*/
		__forceinline static float Dot(const Vector2& v1, const Vector2& v2) { return (v1.x*v2.x + v1.y*v2.y); }
		__forceinline static float Cross(const Vector2& v1, const Vector2& v2) { return v1.x*v2.y - v1.y*v2.x; }
		__forceinline Vector2 operator +(const Vector2& v) { return Vector2(x + v.x, y + v.y); }
		__forceinline Vector2 operator -(const Vector2& v) { return Vector2(x - v.x, y - v.y); }
		__forceinline Vector2 operator *(const Vector2& v) { return Vector2(x * v.x, y * v.y); }
		__forceinline Vector2 operator /(const Vector2& v) { return Vector2(x / v.x, y / v.y); }
		__forceinline Vector2 operator +(float scala) { return Vector2(x + scala, y + scala); }
		__forceinline Vector2 operator -(float scala) { return Vector2(x - scala, y - scala); }
		__forceinline Vector2 operator *(float scala) { return Vector2(x * scala, y * scala); }
		__forceinline Vector2 operator /(float scala) { return Vector2(x / scala, y / scala); }
		__forceinline Vector2 operator =(float scala) { x = scala; y = scala; return *this; }
		__forceinline Vector2& operator +=(const Vector2& v) { x += v.x;	y += v.y; return *this; }
		__forceinline Vector2& operator -=(const Vector2& v) { x -= v.x;	y -= v.y; return *this; }
		__forceinline Vector2& operator *=(const Vector2& v) { x *= v.x;	y *= v.y; return *this; }
		__forceinline Vector2& operator /=(const Vector2& v) { x /= v.x;	y /= v.y; return *this; }
		__forceinline Vector2& operator +=(float scala) { x += scala;	y += scala; return *this; }
		__forceinline Vector2& operator -=(float scala) { x -= scala;		y -= scala; return *this; }
		__forceinline Vector2& operator *=(float scala) { x *= scala;	y *= scala; return *this; }
		__forceinline Vector2& operator /=(float scala) { x /= scala;	y /= scala; return *this; }
		__forceinline bool operator ==(const Vector2& v) { return IsEqual(v); }
		__forceinline bool operator !=(const Vector2& v) { return !IsEqual(v); }
	};
	__forceinline Vector2 operator +(const Vector2& v0, const Vector2& v1) { return Vector2(v0.x + v1.x, v0.y + v1.y); }
	__forceinline Vector2 operator -(const Vector2& v0, const Vector2& v1) { return Vector2(v0.x - v1.x, v0.y - v1.y); }
	__forceinline Vector2 operator *(const Vector2& v0, const Vector2& v1) { return Vector2(v0.x * v1.x, v0.y * v1.y); }
	__forceinline Vector2 operator /(const Vector2& v0, const Vector2& v1) { return Vector2(v0.x / v1.x, v0.y / v1.y); }
	__forceinline Vector2 operator +(const Vector2& v, float scala) { return Vector2(v.x + scala, v.y + scala); }
	__forceinline Vector2 operator -(const Vector2& v, float scala) { return Vector2(v.x - scala, v.y - scala); }
	__forceinline Vector2 operator *(const Vector2& v, float scala) { return Vector2(v.x * scala, v.y * scala); }
	__forceinline Vector2 operator /(const Vector2& v, float scala) { return Vector2(v.x / scala, v.y / scala); }
	__forceinline Vector2 operator *(float scala, const Vector2& v) { return Vector2(v.x * scala, v.y * scala); }
	__forceinline Vector2 operator -(float scala, const Vector2& v) { return Vector2(scala - v.x, scala - v.y); }
	__forceinline Vector2 operator -(const Vector2& v) { return Vector2(-v.x, -v.y); }

	struct Vector3 {
	public:
		union {
			struct { float x, y, z; };
			float v[3];
			Vector2 xy;
		};
		/**
		*@brief 型がintのVector3<br>
		*値を取る為のみ使用なので、メンバ関数は提供しない
		*/
		struct IVector3 {
			union {
				struct { int x, y, z; };
				int v[3];
			};
			__forceinline IVector3(int x, int y, int z) :x(x), y(y), z(z) {}
		};
		__forceinline Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
		__forceinline Vector3()noexcept : Vector3(0.0f, 0.0f, 0.0f) {}
		/**
		*@brief 値をint型で取得する関数
		*/
		__forceinline IVector3 Get()const  noexcept { return IVector3(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)); }
		/**
		*@brief int型のIVector3からの直接Vector3に代入
		*/
		__forceinline void Set(IVector3 v)noexcept {
			for (int i = 0; i < 3; i++) {
				this->v[i] = static_cast<float>(v.v[i]);
			}
		}
		/**@brief 距離(平方根あり)*/
		__forceinline float Length()const { return sqrtf(x*x + y * y + z * z); }
		/**@brief 距離(平方根なし)*/
		__forceinline float LengthSq()const { return (x*x + y * y + z * z); }
		/**@brief 正規化*/
		__forceinline void Normalize() {
			float l = Length();
			if (l != 0.0f) { x /= l;	y /= l; z /= l; }
		}
		__forceinline bool IsEqual(const Vector3& v) { return (FloatEqual(x, v.x) && FloatEqual(y, v.y) && FloatEqual(z, v.z)); }
		/**@brief 内積*/
		__forceinline static float Dot(const Vector3& v1, const Vector3& v2) { return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); }
		/**@brief 外積*/
		__forceinline static Vector3 Cross(const Vector3& v1, const Vector3& v2) {
			Vector3 ret = {};
			ret.x = v1.y*v2.z - v1.z*v2.y;
			ret.y = v1.z*v2.x - v1.x*v2.z;
			ret.z = v1.x*v2.y - v1.y*v2.x;
			return ret;
		}
		__forceinline Vector3 operator +(const Vector3& v) { return Vector3(x + v.x, y + v.y, z + v.z); }
		__forceinline Vector3 operator -(const Vector3& v) { return Vector3(x - v.x, y - v.y, z - v.z); }
		__forceinline Vector3 operator *(const Vector3& v) { return Vector3(x * v.x, y * v.y, z * v.z); }
		__forceinline Vector3 operator /(const Vector3& v) { return Vector3(x / v.x, y / v.y, z / v.z); }
		__forceinline Vector3 operator +(float scala) { return Vector3(x + scala, y + scala, z + scala); }
		__forceinline Vector3 operator -(float scala) { return Vector3(x - scala, y - scala, z - scala); }
		__forceinline Vector3 operator *(float scala) { return Vector3(x * scala, y * scala, z * scala); }
		__forceinline Vector3 operator /(float scala) { return Vector3(x / scala, y / scala, z / scala); }
		__forceinline Vector3 operator =(float scala) { x = scala; y = scala; z = scala; return *this; }
		__forceinline Vector3& operator +=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
		__forceinline Vector3& operator -=(const Vector3& v) { x -= v.x;	y -= v.y;	z -= v.z; return *this; }
		__forceinline Vector3& operator *=(const Vector3& v) { x *= v.x;	y *= v.y;	z *= v.z; return *this; }
		__forceinline Vector3& operator /=(const Vector3& v) { x /= v.x;	y /= v.y;	z /= v.z; return *this; }
		__forceinline Vector3& operator +=(float scala) { x += scala;	y += scala;	z += scala; return *this; }
		__forceinline Vector3& operator -=(float scala) { x -= scala;	y -= scala;	z -= scala; return *this; }
		__forceinline Vector3& operator *=(float scala) { x *= scala;	y *= scala;	z *= scala; return *this; }
		__forceinline Vector3& operator /=(float scala) { x /= scala;	y /= scala;	z /= scala; return *this; }
		__forceinline bool operator ==(const Vector3& v) { return IsEqual(v); }
		__forceinline bool operator !=(const Vector3& v) { return !IsEqual(v); }
	};
	__forceinline Vector3 operator *(float scala, const Vector3& v) { return Vector3(v.x * scala, v.y * scala, v.z * scala); }
	__forceinline Vector3 operator *(const Vector3& v, float scala) { return Vector3(v.x * scala, v.y * scala, v.z * scala); }
	__forceinline Vector3 operator -(float scala, const Vector3& v) { return Vector3(scala - v.x, scala - v.y, scala - v.z); }
	__forceinline Vector3 operator -(const Vector3& v0, const Vector3& v1) { return Vector3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z); }
	__forceinline Vector3 operator +(const Vector3& v0, const Vector3& v1) { return Vector3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z); }
	__forceinline Vector3 operator -(const Vector3& v) { return Vector3(-v.x, -v.y, -v.z); }
	__forceinline bool operator ==(const Vector3& v0, const Vector3& v1) { return (v0.x == v1.x&&v0.y == v1.y&&v0.z == v1.z); }

	struct Vector4 {
	public:
		union {
			union {
				struct { float x, y, z, w; };
				float v[4];
				Vector3 xyz;
				Vector2 xy;
			};
		};
		/**
		*@brief 型がintのVector4<br>
		*値を取る為のみ使用なので、メンバ関数は提供しない
		*/
		struct IVector4 {
			union {
				struct { int x, y, z, w; };
				int v[4];
			};
			__forceinline IVector4(int x, int y, int z, int w) :x(x), y(y), z(z), w(w) {}
		};
		__forceinline Vector4(float x, float y, float z, float w) noexcept :x(x), y(y), z(z), w(w) {}
		__forceinline Vector4()noexcept : Vector4(0.0f, 0.0f, 0.0f, 0.0f) {}
		/**
		*@brief 値をint型で取得する関数
		*/
		__forceinline IVector4 Get()const  noexcept { return IVector4(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z), static_cast<int>(w)); }
		/**
		*@brief int型のIVector4からの直接Vector4に代入
		*/
		__forceinline void Set(IVector4 v)noexcept {
			for (int i = 0; i < 4; i++) {
				this->v[i] = static_cast<float>(v.v[i]);
			}
		}
		/**@brief 距離(平方根あり)*/
		__forceinline float Length()const { return sqrtf(x*x + y * y + z * z + w * w); }
		/**@brief 距離(平方根なし)*/
		__forceinline float LengthSq()const { return (x*x + y * y + z * z + w * w); }
		/**@brief 正規化*/
		__forceinline void Normalize() {
			float l = Length();
			if (l != 0.0f) { x /= l;	y /= l; z /= l; }
		}
		__forceinline bool IsEqual(const Vector4& v) { return (FloatEqual(x, v.x) && FloatEqual(y, v.y) && FloatEqual(z, v.z) && FloatEqual(w, v.w)); }
		/**@brief 内積*/
		__forceinline static float Dot(const Vector4& v1, const Vector4& v2) { return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w); }
		__forceinline Vector4 operator +(const Vector4& v) { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
		__forceinline Vector4 operator -(const Vector4& v) { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
		__forceinline Vector4 operator *(const Vector4& v) { return Vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
		__forceinline Vector4 operator /(const Vector4& v) { return Vector4(x / v.x, y / v.y, z / v.z, w / v.w); }
		__forceinline Vector4 operator +(float scala) { return Vector4(x + scala, y + scala, z + scala, w + scala); }
		__forceinline Vector4 operator -(float scala) { return Vector4(x - scala, y - scala, z - scala, w - scala); }
		__forceinline Vector4 operator *(float scala) { return Vector4(x * scala, y * scala, z * scala, w * scala); }
		__forceinline Vector4 operator /(float scala) { return Vector4(x / scala, y / scala, z / scala, w / scala); }
		__forceinline Vector4 operator =(float scala) { x = scala; y = scala; z = scala; w = scala; return *this; }
		__forceinline Vector4& operator +=(const Vector4& v) { x += v.x;	y += v.y;	z += v.z; w += v.w; return *this; }
		__forceinline Vector4& operator -=(const Vector4& v) { x -= v.x;	y -= v.y;	z -= v.z; w -= v.w; return *this; }
		__forceinline Vector4& operator *=(const Vector4& v) { x *= v.x;	y *= v.y;	z *= v.z; w *= v.w; return *this; }
		__forceinline Vector4& operator /=(const Vector4& v) { x /= v.x;	y /= v.y;	z /= v.z; w /= v.w; return *this; }
		__forceinline Vector4& operator +=(float scala) { x += scala;	y += scala;	z += scala; w += scala; return *this; }
		__forceinline Vector4& operator -=(float scala) { x -= scala;	y -= scala;	z -= scala; w -= scala; return *this; }
		__forceinline Vector4& operator *=(float scala) { x *= scala;	y *= scala;	z *= scala; w *= scala; return *this; }
		__forceinline Vector4& operator /=(float scala) { x /= scala;	y /= scala;	z /= scala; w /= scala; return *this; }
		__forceinline bool operator ==(const Vector4& v) { return IsEqual(v); }
		__forceinline bool operator !=(const Vector4& v) { return !IsEqual(v); }
	};
	__forceinline Vector4 operator *(float scala, const Vector4& v) { return Vector4(v.x * scala, v.y * scala, v.z * scala, v.w * scala); }
	__forceinline Vector4 operator -(float scala, const Vector4& v) { return Vector4(scala - v.x, scala - v.y, scala - v.z, scala - v.w); }
	__forceinline Vector4 operator -(const Vector4& v) { return Vector4(-v.x, -v.y, -v.z, -v.w); }

}