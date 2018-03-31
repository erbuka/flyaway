#pragma once

#include "Vecmath.h"
#include <string>
#include <vector>
#include <map>

namespace fa
{

	class Wavefront
	{
	public:
		typedef std::array<int, 3> FaceVertex;
		typedef std::vector<FaceVertex> Face;

		struct Group
		{
			std::string Name;
			std::vector<Face> Faces;
		};

		static Wavefront Parse(std::string source);

		Group& GetGroup(std::string name, bool create = false);

		std::map<std::string, Group>& GetGroups() { return m_Groups; }
		std::vector<Vector3f>& GetVertices() { return m_Vertices; }
		std::vector<Vector3f>& GetNormals() { return m_Normals; }
		std::vector<Vector2f>& GetTexCoords() { return m_TexCoords; }


	private:
		std::vector<Vector3f> m_Vertices, m_Normals;
		std::vector<Vector2f> m_TexCoords;
		std::map<std::string, Group> m_Groups;
	};
}