#pragma once

#include "Model.h"
#include "Wavefront.h"
#include "GL/glew.h"

namespace fa
{
	class WavefrontModel : public Model
	{
	public:
		static WavefrontModel * Create(Wavefront& wavefront, std::map<std::string, Vector3f> colorMap = {});
		
		~WavefrontModel();

		virtual void Render() override;

		virtual BoundingBox3f GetBounds();


	private:

		struct VAOGroup
		{
			GLuint VAO, VB;
			int VertexCount;
		};

		std::vector<VAOGroup> m_VAOGroups;
		BoundingBox3f m_BoundingBox;

	};
}