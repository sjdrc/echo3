#ifndef _ECHOPRIMITIVETYPES_H_
#define _ECHOPRIMITIVETYPES_H_

#include <echo/Types.h>
#include <echo/Maths/Vector2.h>
#include <ostream>

namespace Echo
{
	struct TextureUV
	{
		f32 u;
		f32 v;
		TextureUV(f32 u = 0.f, f32 v = 0.f) : u(u), v(v){}
		bool operator==(const TextureUV& rhs) const
		{
			return (u==rhs.u && v==rhs.v);
		}
		bool operator!=(const TextureUV& rhs) const
		{
			return (u!=rhs.u || v!=rhs.v);
		}

		operator Vector2() const
		{
			return Vector2(u,v);
		}

		inline friend std::ostream& operator <<( std::ostream& o, const TextureUV& tuv )
		{
			o << "TextureUV(" << tuv.u << "," << tuv.v << ")";
			return o;
		}
		
		friend std::istream& operator >>(std::istream& i, TextureUV& tuv);
	};
	
	/**
	 * A TextureUVPair is useful for storing a min and max pair of uvs for a quad.
	 * 
	 */
	typedef std::pair<TextureUV,TextureUV> TextureUVPair;
	
	inline std::ostream& operator <<( std::ostream& o, const TextureUVPair& tuvPair )
	{
		o << "TextureUVPair(" << tuvPair.first.u << "," << tuvPair.first.v << "," << tuvPair.second.u << "," << tuvPair.second.v << ")";
		return o;
	}

	std::istream& operator >>(std::istream& i, TextureUVPair& tuvPair);

	struct IndexedTriangle
	{
		IndexedTriangle(u16 a = 0u, u16 b = 0u, u16 c = 0u) : a(a), b(b), c(c){}
		~IndexedTriangle(){}
		u16 a;
		u16 b;
		u16 c;
	};
	
	struct IndexedQuad
	{
		IndexedQuad(u16 a = 0u, u16 b = 0u, u16 c = 0u, u16 d = 0u) : a(a), b(b), c(c), d(d){}
		~IndexedQuad(){}
		u16 a;
		u16 b;
		u16 c;
		u16 d;
	};

	struct RGB
	{
		u8 mR;
		u8 mG;
		u8 mB;
	};

	struct RGBA
	{
		u8 mR;
		u8 mG;
		u8 mB;
		u8 mA;
	};

	struct BGR
	{
		u8 mB;
		u8 mG;
		u8 mR;
	};

	struct BGRA
	{
		u8 mB;
		u8 mG;
		u8 mR;
		u8 mA;
	};
}
#endif
