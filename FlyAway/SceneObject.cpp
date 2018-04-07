#include "SceneObject.h"

fa::SceneObject::SceneObject(Model * model, const Vector3f & position, const Vector4f & rotation, const Vector3f& scale) :
	m_Model(model),
	m_Position(position),
	m_Rotation(rotation)
{
	m_Transform = Matrix4f::Translation(position) * Matrix4f::Rotation(rotation) * Matrix4f::Scale(scale);
}

fa::Matrix4f & fa::SceneObject::GetTransform()
{
	return m_Transform;
}

fa::Model * fa::SceneObject::GetModel()
{
	return m_Model;
}
