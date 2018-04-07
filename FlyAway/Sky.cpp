#include "Sky.h"
#include "Util.h"

namespace _Sky
{

	constexpr float Time(int hour, int minute)
	{
		return ((hour >= 6 ? hour - 6 : hour + 18) + minute / 60.0f) / 24.0f;
	};

	fa::Interpolator<fa::Vector3f> LightColor = {
		{ Time(6, 0), fa::Vector3f::GetColor(194,197,234) },
		{ Time(6, 30), fa::Vector3f::GetColor(242,216,252) },

			// 12AM
		{ Time(12, 0), fa::Vector3f::GetColor(242,216,252) },

			// 6 PM
		{ Time(18, 0), fa::Vector3f::GetColor(242,216,252) },

			// 6.30 - 7 PM
		{ Time(18, 30), fa::Vector3f::GetColor(255,194,45) },
		{ Time(19, 0), fa::Vector3f::GetColor(244,118,70) },
		{ Time(19, 30), fa::Vector3f::GetColor(12,51,113) },

			// 12PM
		{ Time(0, 0), fa::Vector3f::GetColor(12,51,113) },

			// 5.30 - 6 AM
		{ Time(5, 30), fa::Vector3f::GetColor(12,51,113) },
		{ 1.0f, fa::Vector3f::GetColor(194,197,234) }

	};

	fa::Interpolator<fa::Vector3f> SkyColor = {
		// 6 - 6.30 AM
		{ Time(6, 0), fa::Vector3f::GetColor(220,194,234) },
		{ Time(6, 30), fa::Vector3f::GetColor(156,229,255) },

		// 12AM
		{ Time(18, 0), fa::Vector3f::GetColor(0,165,255) },

		// 6 PM
		{ Time(18, 0), fa::Vector3f::GetColor(0,165,255) },

		// 6.30 - 7 PM
		{ Time(18, 30), fa::Vector3f::GetColor(244,118,70) },
		{ Time(19, 30), fa::Vector3f::GetColor(13,51,113) },

		// 12PM
		{ Time(0, 0), fa::Vector3f::GetColor(21,14,62) },

		// 5.30 - 6 AM

		{ Time(5, 30), fa::Vector3f::GetColor(21,14,62) },
		{ 1.0f, fa::Vector3f::GetColor(220,194,234) }
	};
}

fa::Sky::Sky(float dayDuration) : m_DayDuration(dayDuration)
{
}

fa::Sky::~Sky()
{
}

fa::Vector3f fa::Sky::GetTopColor() const
{
	return m_TopColor;
}

fa::Vector3f fa::Sky::GetHorizonColor() const
{
	return m_HorizonColor;
}

void fa::Sky::Update(float elapsedTime)
{
	m_Time += elapsedTime;

	// Everything is based on the day time and the total
	int days;
	float dayTime;
	GetTime(days, dayTime);

	// Compute light direction
	float angle = std::sinf(2 * PI<float>() * dayTime);
	//m_Light.Direction = Vector3f(0, std::sinf(angle), -std::cosf(angle));
	m_Light.Direction = Vector3f(0.0, 1.0, 1.0).Normalized();

	// Compute light color
	m_Light.Color = _Sky::LightColor.Sample(dayTime);

	// Compute colors
	m_TopColor = _Sky::SkyColor.Sample(dayTime);
	m_HorizonColor = m_TopColor + Vector3f(0.2f, 0.2f, 0.2f);

}

void fa::Sky::GetTime(int & days, float & dayTime) const
{
	float d = m_Time / m_DayDuration;
	days = (int)d;
	dayTime = (m_Time - days * m_DayDuration) / m_DayDuration;
}

fa::Light fa::Sky::GetLight() const
{
	return m_Light;
}
