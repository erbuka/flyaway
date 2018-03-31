#pragma once

#include <stack>
#include "Vecmath.h"
#include "Util.h"

namespace fa 
{
	template<typename T>
	class MatrixStack4
	{
	public:
		MatrixStack4()
		{

		}

		~MatrixStack4()
		{

		}

		Matrix4<T> Current()
		{
			return m_Stack[m_CurrentPosition];
		}

		void Translate(Vector3<T>& v)
		{
			Transalte(v.X, v.Y, v.Z);
		}

		void Translate(T x, T y, T z)
		{
			auto translationMatrix = Matrix4<T>::Identity();
				
			translationMatrix.Components[3] = x;
			translationMatrix.Components[7] = y;
			translationMatrix.Components[11] = z;

			Multiply(translationMatrix);

		}

		void Perspective(float fovY, float aspect, float zNear, float zFar) {

			Matrix4<T> perspectiveMatrix;

			float f = 1.0f / tan(fovY / 2.0f);
			float diff = zNear - zFar;

			perspectiveMatrix.Components[0] = f / aspect;
			perspectiveMatrix.Components[1] = 0.0f;
			perspectiveMatrix.Components[2] = 0.0f;
			perspectiveMatrix.Components[3] = 0.0f;

			perspectiveMatrix.Components[4] = 0.0f;
			perspectiveMatrix.Components[5] = f;
			perspectiveMatrix.Components[6] = 0.0f;
			perspectiveMatrix.Components[7] = 0.0f;

			perspectiveMatrix.Components[8] = 0.0f;
			perspectiveMatrix.Components[9] = 0.0f;
			perspectiveMatrix.Components[10] = (zFar + zNear) / diff;
			perspectiveMatrix.Components[11] = 2.0f * (zNear * zFar) / diff;

			perspectiveMatrix.Components[12] = 0.0f;
			perspectiveMatrix.Components[13] = 0.0f;
			perspectiveMatrix.Components[14] = -1.0f;
			perspectiveMatrix.Components[15] = 0.0f;

			Multiply(perspectiveMatrix);

		}

		void LoadIdentity()
		{
			m_Stack[m_CurrentPosition] = Matrix4<T>::Identity();
		}

		void Multiply(const Matrix4<T>& matrix)
		{
			m_Stack[m_CurrentPosition] = m_Stack[m_CurrentPosition] * matrix;
		}

		void Push()
		{
			Util::NotZeroFail(m_CurrentPosition >= StackSize - 1);
			m_CurrentPosition++;
			m_Stack[m_CurrentPosition] = m_Stack[m_CurrentPosition - 1];
		}

		void Pop()
		{
			Util::NotZeroFail(m_CurrentPosition == 0);
			m_CurrentPosition--;
		}

	private:
		static constexpr int StackSize = 16;
		int m_CurrentPosition;
		Matrix4<T> m_Stack[StackSize];
	};


	using MatrixStack4f = MatrixStack4<float>;
}