#pragma once

#include <array>
#include <ostream>
#include <cmath>
namespace fa {
	template<typename T>
	union Vector3
	{
		struct { T X, Y, Z; };
		struct { T R, G, B; };
		T Components[3];

		Vector3() : X(0), Y(0), Z(0) {}
		Vector3(T x, T y, T z) : X(x), Y(y), Z(z) {}

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
		return Vector3<T>(lhs.X * rhs, lhs.Y * rhs, lhs.X * rhs);
	}

	template<typename T>
	Vector3<T> operator/(const Vector3<T>& lhs, const Vector3<T>& rhs) 
	{
		return Vector3<T>(lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z);
	}


	template<typename T, typename K>
	Vector3<T> operator/(const Vector3<T>& lhs, K rhs)
	{
		return Vector3<T>(lhs.X / rhs, lhs.Y / rhs, lhs.X / rhs);
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
		Vector2(T x, T y) : X(x), Y(y) {}
		
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

	using Vector2f = Vector2<float>;

	using Vector3f = Vector3<float>;

	using Matrix4f = Matrix4<float>;
}