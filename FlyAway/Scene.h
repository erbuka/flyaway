#pragma once

#include "MatrixStack.h"

namespace fa 
{

	class Engine;
	class BiomeGenerator;

	struct Vertex
	{
		Vector3f Position;
		Vector3f Normal;
		Vector3f DiffuseColor;
	};

	class Scene
	{
	public:
		Scene(Engine * engine, int width, int depth, float tileSize);
		~Scene();


		void Update(float elapsedTime);
		void Render();
	private:

		void ComputeFaceNormals();

		void UpdateTerrain(int fromVertex, int count);

		MatrixStack4f m_Projection, m_ModelView;
		GLuint m_Vbo;
		int m_Width, m_Depth, m_VerticesCount;
		float m_TileSize;
		Vertex * m_Vertices;
		Engine * m_Engine;
		Vector3f m_CameraPosition;
		BiomeGenerator * m_Biome;
	};
}