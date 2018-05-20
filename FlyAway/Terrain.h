#pragma once

#include "Vecmath.h"
#include "GL/glew.h"
#include <vector>

namespace fa
{

	class SceneObject;
	class SceneObjectGrid;
	class Terrain;
	class Water;
	
	class SceneObjectGrid
	{
	public:

		static constexpr float CellWidth = 2.0f;
		static constexpr float CellDepth = 2.0f;

		struct CellContent
		{
			BoundingBox3f Bounds;
			SceneObject * Object;
		};

		CellContent& At(int x, int z);

		inline int GetCellsX() { return m_CellsX; }
		inline int GetCellsZ() { return m_CellsZ; }

		SceneObjectGrid& operator=(SceneObjectGrid&& other);

		SceneObjectGrid(Terrain * terrain);
		SceneObjectGrid(SceneObjectGrid&& other);
		SceneObjectGrid();
		~SceneObjectGrid();
	private:
		int m_CellsX, m_CellsZ;
		Terrain * m_Terrain;
		CellContent * m_Data;
	};

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

		void GenerateWater();
		void GenerateVertexArrays();

		GLuint GetTerrainVAO() const;
		std::shared_ptr<Water> GetWater() const;

		Vertexf& operator[](int index) const;
		Vertexf& operator[](int index);
		Vertexf& At(int index);

		float GetHeightAt(const Vector3f& v) const;

		SceneObjectGrid& GetSceneObjects();

		BoundingBox3f& GetBounds();

		void ComputeNormals();


	private:



		int m_VerticesCount, m_VerticesX, m_VerticesZ;
		float m_TileWidth, m_TileDepth;
		BoundingBox3f m_Bounds;
		Vertexf * m_Vertices, ** m_Adjacency;
		GLuint m_VB, m_IB, m_VAO;
		std::vector<GLuint> m_Indices;
		SceneObjectGrid m_SceneObjectGrid;
		std::shared_ptr<Water> m_Water;
	};

	

}