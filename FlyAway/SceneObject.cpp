#include "SceneObject.h"

fa::SceneObject::SceneObject(Model * model, const Vector3f & position) :
	m_Model(model),
	m_Position(position)
{
	m_Transform = Matrix4f::Translation(position);
}

fa::Matrix4f & fa::SceneObject::GetTransform()
{
	return m_Transform;
}

fa::Model * fa::SceneObject::GetModel()
{
	return m_Model;
}
