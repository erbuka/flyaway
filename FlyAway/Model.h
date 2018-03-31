#pragma once

#include "Vecmath.h"

namespace fa
{
	class Model
	{
	public:
		virtual void Render() = 0;
		virtual BoundingBox3f GetBounds() = 0;
	};
}