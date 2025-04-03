#include <echo/Util/Texture.h>
#include <echo/Graphics/Texture.h>
#include <boost/gil/channel.hpp>
#include <boost/mp11/mpl.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/metafunctions.hpp>

namespace boost { namespace gil {
		struct luminance_t {};
		typedef mp11::mp_list<luminance_t,alpha_t> luminance_alpha_t;
		typedef layout<luminance_alpha_t> luminance_alpha_layout_t;
		BOOST_GIL_DEFINE_ALL_TYPEDEFS(8,uint8_t,luminance_alpha)
		BOOST_GIL_DEFINE_ALL_TYPEDEFS(32f,uint32_t,luminance_alpha)
}}

namespace Echo
{
	namespace Utils
	{
		template<typename PixelType,typename ViewType, typename Sampler>
		void ResizeWithGIL(const Texture& texture, Texture& destTexture, Sampler sampler)
		{
			using namespace boost::gil;
			ViewType sourceData = interleaved_view(texture.GetWidth(),texture.GetHeight(),(PixelType*)texture.GetBuffer().get(),texture.GetWidth()*texture.GetBytesPerPixel());
			ViewType destView = interleaved_view(destTexture.GetWidth(),destTexture.GetHeight(),(PixelType*)destTexture.GetBuffer().get(),destTexture.GetBytesPerPixel()*destTexture.GetWidth());
			resize_view(sourceData,destView, sampler);
		}

		template<typename SourcePixelType,typename SourceViewType,typename DestinationPixelType,typename DestinationViewType>
		void CopyWithGIL(const Texture& texture, Texture& destTexture)
		{
			using namespace boost::gil;
			SourceViewType sourceData = interleaved_view(texture.GetWidth(),texture.GetHeight(),(SourcePixelType*)texture.GetBuffer().get(),texture.GetWidth()*texture.GetBytesPerPixel());
			DestinationViewType destView = interleaved_view(destTexture.GetWidth(),destTexture.GetHeight(),(DestinationPixelType*)destTexture.GetBuffer().get(),destTexture.GetBytesPerPixel()*destTexture.GetWidth());
			copy_pixels(sourceData,destView);
		}
		
		template<typename PixelType,typename ViewType>
		bool CopySmallerTextureIntoLargerWithGIL(const Texture& texture, Texture& destTexture, bool centre)
		{
			if( texture.GetWidth()>destTexture.GetWidth() ||
				texture.GetHeight()>destTexture.GetHeight())
			{
				return false;
			}
			Size x=0;
			Size y=0;
			if(centre)
			{
				x = (destTexture.GetWidth() - texture.GetWidth()) / 2;
				y = (destTexture.GetHeight() - texture.GetHeight()) / 2;
			}
			using namespace boost::gil;
			ViewType sourceData = interleaved_view(texture.GetWidth(),texture.GetHeight(),(PixelType*)texture.GetBuffer().get(),texture.GetWidth()*texture.GetBytesPerPixel());
			ViewType destView = subimage_view(
									interleaved_view(destTexture.GetWidth(),destTexture.GetHeight(),(PixelType*)destTexture.GetBuffer().get(),destTexture.GetBytesPerPixel()*destTexture.GetWidth()),
									x,y,texture.GetWidth(),texture.GetHeight());
			copy_pixels(sourceData,destView);
			return true;
		}
		
	#define DEFINE_GIL_PACKED_PIXEL_16BIT_TYPES(channelFormat,layout,distribution) \
		typedef boost::gil::packed_pixel_type<uint16_t, channelFormat, boost::gil::layout##_layout_t>::type layout##distribution##_pixel_t; \
		typedef layout##distribution##_pixel_t* layout##distribution##_ptr_t; \
		typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<layout##distribution##_ptr_t> > layout##distribution##_loc_t; \
		typedef boost::gil::image_view<layout##distribution##_loc_t> layout##distribution##_view_t;

		using namespace boost::gil;
		typedef boost::mp11::mp_list_c<unsigned,5,6,5> FORMAT565;

		DEFINE_GIL_PACKED_PIXEL_16BIT_TYPES(FORMAT565,rgb,565);
		typedef boost::mp11::mp_list_c<unsigned,5,5,5,1> FORMAT5551;
		DEFINE_GIL_PACKED_PIXEL_16BIT_TYPES(FORMAT5551,rgba,5551);
		typedef boost::mp11::mp_list_c<unsigned,4,4,4,4> FORMAT4444;
		DEFINE_GIL_PACKED_PIXEL_16BIT_TYPES(FORMAT4444,rgba,4444);
		
		template< typename Sampler >
		shared_ptr<Texture> CreateResizedTexture(const Texture& texture, size_t newWidth, size_t newHeight, Sampler sampler)
		{
			shared_ptr<Texture> newTexture(new Texture(newWidth,newHeight,texture.GetFormat()));

			switch(texture.GetFormat())
			{
				case Texture::Formats::R8G8B8A8:
				case Texture::Formats::R8G8B8X8:
				{
					ResizeWithGIL<rgba8_pixel_t,rgba8_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::R8G8B8:
				{
					ResizeWithGIL<rgb8_pixel_t,rgb8_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::R5G6B5:
				{
					//ResizeWithGIL<rgb565_pixel_t,rgb565_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::R5G5B5A1:
				{
					//ResizeWithGIL<rgba5551_pixel_t,rgba5551_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::R4G4B4A4:
				{
					//ResizeWithGIL<rgba4444_pixel_t,rgba4444_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::B8G8R8A8:
				{
					ResizeWithGIL<bgra8_pixel_t,bgra8_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::B8G8R8:
				{
					ResizeWithGIL<bgr8_pixel_t,bgr8_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::RGB_F32:
				{
					ResizeWithGIL<rgb32f_pixel_t,rgb32f_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::RGBA_F32:
				{
					ResizeWithGIL<rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::LUMINANCE8_ALPHA8:
				{
					ResizeWithGIL<luminance_alpha8_pixel_t,luminance_alpha8_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::LUMINANCE8:
				case Texture::Formats::GREYSCALE8:
				{
					ResizeWithGIL<gray8_pixel_t,gray8_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::DEPTH16:
				case Texture::Formats::GREYSCALE16:
				{
					ResizeWithGIL<gray16_pixel_t,gray16_view_t>(texture,*newTexture,sampler);
				}break;
				case Texture::Formats::UNKNOWN:
				default:
					newTexture.reset();
					break;
			}
			return newTexture;
		}
		
		shared_ptr<Texture> CreateResizedTexture(const Texture& texture, size_t newWidth, size_t newHeight, TextureUnit::TextureFilter filter)
		{
			switch(filter)
			{
				case TextureUnit::TextureFilters::NEAREST:
					return CreateResizedTexture(texture, newWidth, newHeight, nearest_neighbor_sampler());
				break;
				case TextureUnit::TextureFilters::LINEAR:
					return CreateResizedTexture(texture, newWidth, newHeight, bilinear_sampler());
				break;
				default:
					ECHO_LOG_ERROR("Unsupported filter method " << filter << " Supported samplers are: " << TextureUnit::TextureFilters::NEAREST << "," << TextureUnit::TextureFilters::LINEAR);
				break;
			}
			return nullptr;
		}
		
		shared_ptr<Texture> CreateFloatTexture(const Texture& texture)
		{
			Texture::Format format = Texture::Formats::HasAlpha(texture.GetFormat()) ? Texture::Formats::RGBA_F32 : Texture::Formats::RGB_F32;
			shared_ptr<Texture> newTexture(new Texture(texture.GetWidth(),texture.GetHeight(),format));
			switch(texture.GetFormat())
			{
				case Texture::Formats::R8G8B8A8:
				case Texture::Formats::R8G8B8X8:
				{
					CopyWithGIL<rgba8_pixel_t,rgba8_view_t,rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::R8G8B8:
				{
					CopyWithGIL<rgb8_pixel_t,rgb8_view_t,rgb32f_pixel_t,rgb32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::R5G6B5:
				{
					//CopyWithGIL<rgb565_pixel_t,rgb565_view_t,rgb32f_pixel_t,rgb32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::R5G5B5A1:
				{
					//CopyWithGIL<rgba5551_pixel_t,rgba5551_view_t,rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::R4G4B4A4:
				{
					//CopyWithGIL<rgba4444_pixel_t,rgba4444_view_t,rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::B8G8R8A8:
				{
					CopyWithGIL<bgra8_pixel_t,bgra8_view_t,rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::B8G8R8:
				{
					CopyWithGIL<bgr8_pixel_t,bgr8_view_t,rgb32f_pixel_t,rgb32f_view_t>(texture,*newTexture);
				}break;				
				case Texture::Formats::LUMINANCE8_ALPHA8:
				{
					CopyWithGIL<luminance_alpha8_pixel_t,luminance_alpha8_view_t,luminance_alpha32f_pixel_t,luminance_alpha32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::LUMINANCE8:
				{
					CopyWithGIL<gray8_pixel_t,gray8_view_t,gray32f_pixel_t,gray32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::LUMINANCE32_ALPHA32:
				{
					CopyWithGIL<luminance_alpha32f_pixel_t,luminance_alpha32f_view_t,luminance_alpha32f_pixel_t,luminance_alpha32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::LUMINANCE32:
				{
					CopyWithGIL<gray32_pixel_t,gray32_view_t,gray32f_pixel_t,gray32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::RGBA_F32:
				{
					CopyWithGIL<rgba32f_pixel_t,rgba32f_view_t,rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::RGB_F32:
				{
					CopyWithGIL<rgb32f_pixel_t,rgb32f_view_t,rgb32f_pixel_t,rgb32f_view_t>(texture,*newTexture);
				}
				case Texture::Formats::GREYSCALE8:
				{
					CopyWithGIL<gray8_pixel_t,gray8_view_t,gray32f_pixel_t,gray32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::DEPTH16:
				case Texture::Formats::GREYSCALE16:
				{
					CopyWithGIL<gray16_pixel_t,gray16_view_t,gray32f_pixel_t,gray32f_view_t>(texture,*newTexture);
				}break;
				case Texture::Formats::UNKNOWN:
				default:
					newTexture.reset();
					break;
			}
			return newTexture;
		}

		shared_ptr<Texture> CreatePaddedTexture(const Texture& texture, Size newWidth, Size newHeight, bool centre)
		{
			if(texture.GetWidth()>=newWidth && texture.GetHeight()>=newHeight)
			{
				return nullptr;
			}
			
			// We can create a new texture.
			shared_ptr<Texture> newTexture(new Texture(newWidth,newHeight,texture.GetFormat()));
			
			ZeroTexture(*newTexture);
			bool copyResult=false;
			switch(texture.GetFormat())
			{
				case Texture::Formats::R8G8B8A8:
				case Texture::Formats::R8G8B8X8:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<rgba8_pixel_t,rgba8_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::R8G8B8:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<rgb8_pixel_t,rgb8_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::R5G6B5:
				{
					//copyResult = CopySmallerTextureIntoLargerWithGIL<rgb565_pixel_t,rgb565_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::R5G5B5A1:
				{
					//copyResult = CopySmallerTextureIntoLargerWithGIL<rgba5551_pixel_t,rgba5551_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::R4G4B4A4:
				{
					//copyResult = CopySmallerTextureIntoLargerWithGIL<rgba4444_pixel_t,rgba4444_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::B8G8R8A8:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<bgra8_pixel_t,bgra8_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::B8G8R8:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<bgr8_pixel_t,bgr8_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::LUMINANCE8_ALPHA8:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<luminance_alpha8_pixel_t,luminance_alpha8_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::LUMINANCE8:
				case Texture::Formats::GREYSCALE8:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<gray8_pixel_t,gray8_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::DEPTH16:
				case Texture::Formats::GREYSCALE16:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<gray16_pixel_t,gray16_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::LUMINANCE32_ALPHA32:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<luminance_alpha32f_pixel_t,luminance_alpha32f_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::LUMINANCE32:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<gray32_pixel_t,gray32_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::RGBA_F32:
				{
					copyResult = CopySmallerTextureIntoLargerWithGIL<rgba32f_pixel_t,rgba32f_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::RGB_F32:
				{				
					copyResult = CopySmallerTextureIntoLargerWithGIL<rgb32f_pixel_t,rgb32f_view_t>(texture,*newTexture,centre);
				}break;
				case Texture::Formats::UNKNOWN:
				default:
					break;
			}
			if(!copyResult)
			{
				newTexture.reset();
			}
			return newTexture;
		}
		
		bool ZeroTexture(Texture& texture)
		{
			// Fill with zeros.
			u8* buffer=texture.GetBuffer().get();
			if(!buffer)
			{
				return false;
			}
			Size bufferSize=texture.GetDataSize();
			for(Size i=0;i<bufferSize;++i)
			{
				buffer[i]=0;
			}
			return true;
		}
	}
}
