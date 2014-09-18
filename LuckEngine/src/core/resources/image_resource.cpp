#include <core/resources/resource.hpp>

#include <core/common/debug.hpp>

#include <tools/image_converter.hpp>

namespace luck
{
	bool image_resource::load(image_resource* r, std::string path)
	{
		auto image = tools::image::load(path);
		if(image.data == nullptr)
			return false;

		r->size.x = (float)image.width;
		r->size.y = (float)image.height;
		r->pixels.assign(&image.data[0],&image.data[image.width*image.height*4]);
		
		return true;
	}
}
