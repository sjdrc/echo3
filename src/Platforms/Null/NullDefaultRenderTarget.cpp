#include <echo/Platform.h>

namespace Echo
{
	namespace Platform
	{
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen)
		{
			return shared_ptr<RenderTarget>();
		}
	}
}
