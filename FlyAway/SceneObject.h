#pragma once

#include "Vecmath.h"

namespace fa
{
	class Model;

	class SceneObject
	{
	public:
		SceneObject(Model * model, const Vector3f& position);
		Matrix4f& GetTransform();
		Model * GetModel();

	private:
		Model * m_Model;
		Vector3f m_Position;
		Matrix4f m_Transform;
	};
}