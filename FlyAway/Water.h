#pragma once

#include "GL/glew.h"
#include "Vecmath.h"
#include <vector>

namespace fa
{
	class Terrain;

	class Water
	{
	public:
		Water();
		~Water();

		void GenerateVertexArrays();
		void Generate(const Terrain& terrain);
		int GetVerticesCount() const;
		GLuint GetVAO() const;

	private:
		GLuint m_Vao, m_Vb;
		std::vector<Vertexf> m_Vertices;
	};
}