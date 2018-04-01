#include "WavefrontModel.h"
#include "Util.h"

fa::WavefrontModel * fa::WavefrontModel::Create(Wavefront & wavefront, std::map<std::string, Vector3f> colorMap)
{
	auto& groups = wavefront.GetGroups();
	auto& vertices = wavefront.GetVertices();
	auto& normals = wavefront.GetNormals();



	WavefrontModel * result = new WavefrontModel();
	BoundingBox3f bounds = {
		Vector3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
		Vector3f(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())
	};
	// Foreach group create a vertex array
	for (auto &g : groups)
	{
		std::vector<Vertexf> vertexData;
		GLuint vao, vbo;

		Vector3f color = colorMap.find(g.first) != colorMap.end() ?
			colorMap[g.first] : Vector3f(1.0f, 1.0f, 1.0f);

		for (auto &face : g.second)
		{
			if (face.size() != 3)
			{
				delete result;
				throw std::runtime_error("Face is not a triangle");
				return nullptr;
			}

			Vertexf vertex;

			for (auto indices : face)
			{
				auto position = vertices[indices[0]];

				bounds.Min.X = std::fminf(bounds.Min.X, position.X);
				bounds.Min.Y = std::fminf(bounds.Min.Y, position.Y);
				bounds.Min.Z = std::fminf(bounds.Min.Z, position.Z);

				bounds.Max.X = std::fmaxf(bounds.Max.X, position.X);
				bounds.Max.Y = std::fmaxf(bounds.Max.Y, position.Y);
				bounds.Max.Z = std::fmaxf(bounds.Max.Z, position.Z);

				vertex.Position = position;
				vertex.DiffuseColor = color;
				vertex.Normal = normals[indices[2]];

				vertexData.push_back(vertex);
			}


		}

		glFastFail(glGenVertexArrays(1, &vao));

		glFastFail(glBindVertexArray(vao));

		glFastFail(glGenBuffers(1, &vbo));
		glFastFail(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		glFastFail(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertexf) * vertexData.size(), (void*)vertexData.data(), GL_STATIC_DRAW));
		
		glFastFail(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexf), (void*)0));
		glFastFail(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexf), (void*)12));
		glFastFail(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexf), (void*)24));
		
		glFastFail(glEnableVertexAttribArray(0));
		glFastFail(glEnableVertexAttribArray(1));
		glFastFail(glEnableVertexAttribArray(2));

		glFastFail(glBindVertexArray(0));

		result->m_VAOGroups.push_back({ vao, vbo, (int)vertexData.size() });

	}

	result->m_BoundingBox = bounds;

	return result;

}

fa::WavefrontModel::~WavefrontModel()
{
	for (auto& vaoGroup : m_VAOGroups)
	{

		glFastFail(glDeleteVertexArrays(1, &vaoGroup.VAO));
		glFastFail(glDeleteBuffers(1, &vaoGroup.VB));
	}

}

void fa::WavefrontModel::Render()
{
	for (auto& vaoGroup : m_VAOGroups)
	{
		glBindVertexArray(vaoGroup.VAO);
		glDrawArrays(GL_TRIANGLES, 0, vaoGroup.VertexCount);
		glBindVertexArray(0);
	}
}

fa::BoundingBox3f fa::WavefrontModel::GetBounds()
{
	return m_BoundingBox;
}
