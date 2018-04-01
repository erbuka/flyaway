#pragma once

#include "Vecmath.h"

namespace fa
{
	class Model;

	class SceneObject
	{
	public:
		SceneObject(Model * model, const Vector3f& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f } );
		Matrix4f& GetTransform();
		Model * GetModel();

	private:
		Model * m_Model;
		Vector3f m_Position;
		Matrix4f m_Transform;
	};
}