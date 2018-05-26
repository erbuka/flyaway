#pragma once

#include "Vecmath.h"

namespace fa
{
	struct Light
	{
		Vector3f Color, Direction;
	};

	class Sky
	{
	public:

		struct Time
		{
			int days;
			float dayTime;
		};

		Sky(float dayDuration);
		~Sky();
		void Update(float elapsedTime);
		Vector3f GetTopColor() const;
		Vector3f GetHorizonColor() const;
		Time GetTime();
		Light GetLight() const;
	private:
		Vector3f m_TopColor, m_HorizonColor;
		float m_Time, m_DayDuration;
		Light m_Light;
	};
}