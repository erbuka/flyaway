#pragma once

#include "Vecmath.h"
#include "GL/glew.h"
#include <vector>

namespace fa
{

	class SceneObject;

	class Terrain
	{
	public:

		enum EAdjacency : unsigned int
		{
			Left = 0, Right = 1, Up = 2, Down = 3
		};

		Terrain(int divisionsX, int divisionsZ, const BoundingBox3f& bounds);
		~Terrain();

		int GetVerticesX() const;
		int GetVerticesZ() const;

		int GetVerticesCount() const;
		int GetIndicescount() const;

		void SetAdjacenty(EAdjacency adjacency, int index, const Vertexf& vertex);
		Vertexf& GetAdjacency(EAdjacency adjacency, int index);

		void GenerateVertexArray();
		void ComputeNormals();

		GLuint GetVAO();

		Vertexf& operator[](int index);
		Vertexf& At(int index);

		float GetHeightAt(const Vector3f& v) const;

		std::vector<SceneObject*>& GetSceneObjects();

		BoundingBox3f& GetBounds();

	private:
		int m_VerticesCount, m_VerticesX, m_VerticesZ;
		float m_TileWidth, m_TileDepth;
		BoundingBox3f m_Bounds;
		Vertexf * m_Vertices, ** m_Adjacency;
		GLuint m_VB, m_IB, m_VAO;
		std::vector<GLuint> m_Indices;
		std::vector<SceneObject*> m_SceneObjects;
	};
}