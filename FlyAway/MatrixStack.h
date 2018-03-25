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
			m_Stack.push(Matrix4<T>());
		}

		~MatrixStack4()
		{
			m_Stack.empty();
		}

		Matrix4<T>& Current()
		{
			return m_Stack.top();
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

			m_Stack.top() = m_Stack.top() * translationMatrix;

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

			m_Stack.top() = m_Stack.top() * perspectiveMatrix;
		}

		void LoadIdentity()
		{
			m_Stack.top() = Matrix4<T>::Identity();
		}

		void Push()
		{
			m_Stack.push(m_Stack.top());
		}

		void Pop()
		{
			Util::ZeroFail(m_Stack.size());
			m_Stack.pop();
		}

	private:
		std::stack<Matrix4<T>> m_Stack;
	};


	using MatrixStack4f = MatrixStack4<float>;
}