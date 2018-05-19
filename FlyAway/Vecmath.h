#pragma once

#include <array>
#include <ostream>
#include <cmath>
#include "Util.h"

namespace fa {

	template<typename T>
	union Vector4
	{
		T Components[4];
		struct { T X, Y, Z, W; };
		struct { T R, G, B, A; };
		Vector4() : X(0), Y(0), Z(0), W(0) {}
		Vector4(T x, T y , T z, T w) : X(x), Y(y), Z(z), W(w) {}
	};

	template<typename T>
	union Vector3
	{
		struct { T X, Y, Z; };
		struct { T R, G, B; };
		T Components[3];

		Vector3() : X(0), Y(0), Z(0) {}
		Vector3(T x, T y, T z) : X(x), Y(y), Z(z) {}
		Vector3(int color)
		{
			R = ((color >> 16) & 0xff) / 255.0;
			G = ((color >> 8) & 0xff) / 255.0;
			B = (color & 0xff) / 255.0;
		}
		Vector3 Clamp(T maxLength)
		{
			if (Length() > maxLength)
			{
				return Normalized() * maxLength;
			}
			else
			{
				return *this;
			}
		}

		Vector3 Floor()
		{
			return Vector3(std::floorf(X), std::floorf(Y), std::floorf(Z));
		}

		Vector3 Normalized()
		{
			T length = Length();
			return length == (T)0 ? Vector3(0, 0, 0) : (*this) / length;
		}

		T Length()
		{
			return (T)std::sqrt(X*X + Y*Y + Z*Z);
		}

		Vector3 MoveTowards(const Vector3& target, float magnitude, bool& hit)
		{
			Vector3 diff = target - *this;
			if (diff.Length() < magnitude)
			{
				hit = true;
				return target;
			}
			else
			{
				hit = false;
				return *this + diff.Normalized() * magnitude;
			}
		}

		static Vector3 GetColor(int r, int g, int b)
		{
			return{ r / 255.0f, g / 255.0f,  b / 255.0f };
		}

	};
	
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
	{
		os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
		return os;
	}


	template<typename T>
	Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs) 
	{
		return Vector3<T>(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
	}

	template<typename T>
	Vector3<T> operator-(const Vector3<T>& lhs, const Vector3<T>& rhs) 
	{
		return Vector3<T>(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}

	template<typename T>
	Vector3<T> operator*(const Vector3<T>& lhs, const Vector3<T>& rhs) 
	{
		return Vector3<T>(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	template<typename T, typename K>
	Vector3<T> operator*(const Vector3<T>& lhs, K rhs)
	{
		return Vector3<T>(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
	}

	template<typename T>
	Vector3<T> operator/(const Vector3<T>& lhs, const Vector3<T>& rhs) 
	{
		return Vector3<T>(lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z);
	}


	template<typename T, typename K>
	Vector3<T> operator/(const Vector3<T>& lhs, K rhs)
	{
		return Vector3<T>(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs);
	}

	template<typename T>
	T operator^(const Vector3<T>& lhs, const Vector3<T>& rhs) 
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
	}

	template<typename T>
	Vector3<T> Cross(const Vector3<T>& lhs, const Vector3<T>& rhs)
	{
		return {
			lhs.Y * rhs.Z - lhs.Z * rhs.Y,
			lhs.Z * rhs.X - lhs.X * rhs.Z,
			lhs.X * rhs.Y - lhs.Y * rhs.X
		};
	}

	template<typename T>
	union Vector2
	{
		struct { T X, Y; };
		struct { T U, V; };
		Vector2() : X(0), Y(0) {}
		Vector2(T x, T y) : X(x), Y(y) {}

		Vector2 Normalized()
		{
			T length = Length();
			return length > 0 ? Vector3() : *this / length;
		}

		T Length()
		{
			return std::sqrt(X*X + Y*Y);
		}
		
		Vector2 Floor() {
			return Vector2((int)X, (int)Y);
		}
	};

	template<typename T>
	Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T> rhs)
	{
		return{ lhs.X + rhs.X, lhs.Y + rhs.Y };
	}

	template<typename T>
	Vector2<T> operator*(const Vector2<T>& lhs, const Vector2<T> rhs)
	{
		return{ lhs.X * rhs.X, lhs.Y * rhs.Y };
	}

	template<typename T, typename K>
	Vector2<T> operator*(const Vector2<T>& lhs, K rhs)
	{
		return{ lhs.X * rhs, lhs.Y * rhs };
	}

	template<typename T, typename K>
	Vector2<T> operator/(const Vector2<T>& lhs, K rhs)
	{
		return{ lhs.X / rhs, lhs.Y / rhs };
	}

	template<typename T>
	T operator^(const Vector2<T>& lhs, const Vector2<T>& rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y;
	}

	template<typename T>
	struct Matrix4 {
	public:
		std::array<T, 16> Components;

		Matrix4()
		{
			Components = {
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f
			};
		}

		Matrix4(std::array<T, 16>& components)
		{
			Components = components;
		}

		T* Ptr()
		{
			return (T*)Components.data();
		}

		static Matrix4 Identity()
		{
			return Matrix4(std::array<T, 16>{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			});
		}

		static Matrix4 Rotation(const Vector4<T>& quaternion)
		{
			return Rotation(quaternion.W, { quaternion.X, quaternion.Y, quaternion.Z });
		}

		static Matrix4 LookRotation(const Vector3<T>& forward)
		{
			Vector3<T> right = Cross(forward, Vector3<T>{0, 1, 0});
			Vector3<T> up = Cross(right, forward);
			return Matrix4(std::array<T, 16>{
				right.X, right.Y, right.Z, 0,
				up.X, up.Y, up.Z, 0,
				-forward.X, -forward.Y, -forward.Z, 0,
				0,0,0,1
			});
		}

		static Matrix4 Rotation(T theta, const Vector3<T>& axis)
		{
			T ct = std::cos(theta);
			T st = std::sin(theta);
			
			T x = axis.X, y = axis.Y, z = axis.Z;

			return Matrix4(std::array<T, 16> {
				ct + x * x * (1.0f - ct), x * y * (1.0f - ct) - z * st, x * z * (1.0f - ct) + y * st, 0.0f,
				y * x * (1.0f - ct) + z * st, ct + y * y * (1.0f - ct), y * z * (1.0f - ct) - x * st, 0.0f,
				z * x * (1.0f - ct) - y * st, z * y * (1.0f - ct) + x * st, ct + z * z * (1.0f - ct), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			});
		}

		static Matrix4 Translation(const Vector3<T>& t)
		{
			return Matrix4(std::array<T, 16> {
				1.0f, 0.0f, 0.0f, t.X,
				0.0f, 1.0f, 0.0f, t.Y,
				0.0f, 0.0f, 1.0f, t.Z,
				0.0f, 0.0f, 0.0f, 1.0f
			});
		}

		static Matrix4 Scale(const Vector3<T>& s)
		{
			return Matrix4(std::array<T, 16> {
				s.X, 0.0f, 0.0f, 0.0f,
				0.0f, s.Y, 0.0f, 0.0f,
				0.0f, 0.0f, s.Z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			});
		}

	};

	template<typename T>
	Matrix4<T> operator*(const Matrix4<T>& lhs, const Matrix4<T>& rhs) {

		Matrix4<T> result;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					result.Components[i * 4 + j] += lhs.Components[i * 4 + k] * rhs.Components[k * 4 + j];
				}
			}
		}
		
		return result;
	}

	template<typename T>
	struct BoundingBox3
	{
		Vector3<T> Min, Max;
		T Width() const { return Max.X - Min.X; }
		T Height() const { return Max.Y - Min.Y; }
		T Depth() const { return Max.Z - Min.Z; }
		Vector3<T> Center() const { return Util::Lerp(Max, Min, 0.5f); }
	};

	template<typename T>
	struct BoundingBox2
	{
		Vector2<T> Min, Max;
		T Width() const { return Max.X - Min.X; }
		T Height() const { return Max.Y - Min.Y; }
		Vector2<T> Center() const { return Util::Lerp(Max, Min, 0.5f); }
	};

	template<typename T>
	struct Vertex
	{
		Vector3<T> Position;
		Vector3<T> Normal;
		Vector3<T> DiffuseColor;
	};


	using Vector2f = Vector2<float>;

	using Vector3f = Vector3<float>;

	using Vector4f = Vector4<float>;

	using Matrix4f = Matrix4<float>;

	using BoundingBox3f = BoundingBox3<float>;

	using BoundingBox2f = BoundingBox2<float>;

	using Vertexf = Vertex<float>;
}