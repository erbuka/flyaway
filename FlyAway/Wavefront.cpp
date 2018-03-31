#include "Wavefront.h"
#include <sstream>
#include <stdexcept>
#include <stdio.h>
class WFToken
{
public:

	enum Type
	{
		Vertex, 
		Normal,
		TexCoord,
		Face,
		NewLine,
		Slash,
		WhiteSpace,
		Hash,
		String,
		End,
		Undefined
	};

	WFToken() : m_Type(Undefined), m_Value() {}

	WFToken(Type type) : m_Type(type), m_Value() {}

	WFToken(Type type, const std::string& value) : m_Type(type), m_Value(value) {}

	Type GetType()
	{
		return m_Type;
	}

	std::string GetVal()
	{
		return m_Value;
	}

	int GetIntVal() 
	{
		return std::atoi(m_Value.c_str());
	}

	float GetFloatVal()
	{
		return (float)std::atof(m_Value.c_str());
	}

private:
	Type m_Type;
	std::string m_Value;
};


class WFLexer
{
public:
	WFLexer(const std::string& source) :
		m_Stream(source),
		m_Line(1),
		m_IgnoreTokens({ WFToken::WhiteSpace })
	{

	}

	WFToken Next(WFToken::Type type)
	{
		WFToken token = Next();
		if (token.GetType() != type)
		{
			std::stringstream ss;
			ss << "Expected " << (int)type << ", found " << token.GetType();
			RaiseError(ss.str());
		}
		return token;
	}

	WFToken Next(WFToken::Type type, std::string value)
	{
		WFToken token = Next();
		if (token.GetType() != type)
		{
			std::stringstream ss;
			ss << "Expected " << (int)type << ", found " << token.GetType();
			RaiseError(ss.str());
		}

		if (token.GetVal() != value)
		{
			std::stringstream ss;
			ss << "Expected '" << value << "', found '" << token.GetVal() << "'";
			RaiseError(ss.str());
		}

		return token;
	}

	WFToken Next()
	{

		char c = m_Stream.get(), c1;
		
		if (m_Stream.eof())
		{
			return WFToken::End;
		}

		WFToken token;

		switch (c)
		{
		case ' ': token = WFToken::WhiteSpace; break;
		case '/': token = WFToken::Slash; break;
		case '\n': m_Line++; token = WFToken::NewLine; break;
		case 'f': token = WFToken::Face; break;
		case '#': token = WFToken::Hash; break;
		case 'v':
			c1 = m_Stream.peek();

			if (c1 == 'n')
			{
				token = WFToken::Normal;
				m_Stream.get();
			}
			else if (c1 == 't')
			{
				token = WFToken::TexCoord;
				m_Stream.get();
			}
			else if (c1 == ' ')
			{
				token = WFToken::Vertex;
			}
			else if (IsStringChar(c1))
			{
				token = ParseString(c);
			}
			else
			{
				RaiseError("Unkown token");
			}

			break;
		default:
			if (IsStringChar(c))
			{
				token = ParseString(c);
			}
			else
			{
				RaiseError("Unknown token");
			}
			break;
		}

		if (std::find(m_IgnoreTokens.begin(), m_IgnoreTokens.end(), token.GetType()) != m_IgnoreTokens.end())
		{
			return Next();
		}
		else
		{
			return token;
		}

	}

	int GetLine()
	{
		return m_Line;
	}

	void RaiseError(std::string error)
	{
		std::stringstream ss;
		ss << "Line " << GetLine() << ": " << error;
		throw std::runtime_error(ss.str());
	};

private:

	bool IsStringChar(char c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-';
	}

	WFToken ParseString(char first)
	{
		std::stringstream ss;

		ss << first;

		while (1)
		{
			char c = m_Stream.peek();

			if (IsStringChar(c))
			{
				ss << c;
				m_Stream.get();
			}
			else
			{
				break;
			}
		}

		return WFToken(WFToken::String, ss.str());
	}

	std::stringstream m_Stream;
	int m_Line;
	std::vector<WFToken::Type> m_IgnoreTokens;
};

fa::Wavefront fa::Wavefront::Parse(std::string source)
{
	Wavefront result;
	WFLexer lexer(source);
	WFToken token;
	std::string currentGroup = "default";

	auto parseVector3f = [&lexer]()
	{
		Vector3f v;
		v.X = lexer.Next().GetFloatVal();
		v.Y = lexer.Next().GetFloatVal();
		v.Z = lexer.Next().GetFloatVal();
		return v;
	};

	auto parseVector2f = [&lexer]()
	{
		Vector2f v;
		v.X = lexer.Next().GetFloatVal();
		v.Y = lexer.Next().GetFloatVal();
		return v;
	};

	auto skipLine = [&lexer]()
	{
		WFToken token;
		do
		{
			token = lexer.Next();
		} while (token.GetType() != WFToken::NewLine && token.GetType() != WFToken::End);
	};

	auto parseFace = [&lexer, &currentGroup]()
	{
		Face face;
		WFToken token;

		do
		{
			FaceVertex vertex;

			token = lexer.Next();

			if (token.GetType() == WFToken::NewLine || token.GetType() == WFToken::End)
			{
				break;
			}

			vertex[0] = token.GetIntVal() - 1;
			lexer.Next(WFToken::Slash);

			token = lexer.Next();

			if (token.GetType() == WFToken::Slash) // <v>//<n>
			{
				vertex[2] = lexer.Next().GetIntVal() - 1;
			}
			else if (token.GetType() == WFToken::String) // <v>/<t>/<n>
			{
				vertex[1] = token.GetIntVal() - 1;
				lexer.Next(WFToken::Slash);
				vertex[2] = lexer.Next().GetIntVal() - 1;
			}
			else
			{;
				lexer.RaiseError("Unexpected token");
			}

			face.push_back(vertex);

		} while (1);

		return face;
	};

	auto parseStringCommand = [&currentGroup, &lexer, &skipLine](WFToken& token)
	{
		if (token.GetVal() == "g")
		{
			currentGroup = lexer.Next(WFToken::String).GetVal();
		}
		else
		{
			skipLine();
		}
	};

	while (token.GetType() != WFToken::End)
	{
		token = lexer.Next();

		switch (token.GetType())
		{
		case WFToken::Vertex:
			result.m_Vertices.push_back(parseVector3f());
			break;
		case WFToken::Normal:
			result.m_Normals.push_back(parseVector3f());
			break;
		case WFToken::TexCoord:
			result.m_TexCoords.push_back(parseVector2f());
			break;		
		case WFToken::Face:
			result.GetGroup(currentGroup, true).Faces.push_back(parseFace());
			break;
		case WFToken::Hash:
			skipLine();
			break;
		case WFToken::String:
			parseStringCommand(token);
			break;
		case WFToken::End:
			break;
		case WFToken::NewLine:
			break;
		default:
			lexer.RaiseError("Unkown token");
		}
	}
	
	return result;
}

std::string fa::Wavefront::Encode() const
{
	std::stringstream ss;
	char buffer[1024];

	for (auto vertex : m_Vertices)
	{
		sprintf_s(buffer, "v %f %f %f", vertex.X, vertex.Y, vertex.Z);
		ss << buffer << std::endl;
	}

	for (auto normal : m_Normals)
	{
		sprintf_s(buffer, "vn %f %f %f", normal.X, normal.Y, normal.Z);
		ss << buffer << std::endl;
	}

	for (auto texCoord : m_TexCoords)
	{
		sprintf_s(buffer, "vt %f %f", texCoord.X, texCoord.Y);
		ss << buffer << std::endl;
	}

	for (auto group : m_Groups)
	{
		ss << "g " << group.first << std::endl;

		for (auto face : group.second.Faces)
		{
			ss << "f";

			for (auto vertex : face)
			{
				sprintf_s(buffer, " %d/%d/%d", vertex[0] + 1, vertex[1] + 1, vertex[2] + 1);
				ss << buffer;
			}

			ss << std::endl;

		}

	}

	return ss.str();
}

fa::Wavefront::Group & fa::Wavefront::GetGroup(std::string name, bool create)
{
	try
	{
		auto& group = m_Groups.at(name);
		return group;
	}
	catch (std::out_of_range& ex)
	{
		if (create)
		{
			return m_Groups[name];
		}
		else
		{
			throw ex;
		}
	}
}
