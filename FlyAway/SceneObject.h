#pragma once

#include "Vecmath.h"

namespace fa
{
	class Model;

	class SceneObject
	{
	public:
		SceneObject(Model * model, const Vector3f& position, const Vector4f& rotation = { 1.0f, 0.0f, 0.0f, 1.0f },
			const Vector3f& scale = { 1.0f, 1.0f, 1.0f });
		Matrix4f& GetTransform();
		Model * GetModel();

	private:
		Model * m_Model;
		Vector3f m_Position;
		Vector4f m_Rotation;
		Matrix4f m_Transform;
	};
}