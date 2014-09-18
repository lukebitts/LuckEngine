#include <core/systems/camera_system.hpp>

#include <core/systems/renderable_system.hpp>
#include <core/components/camera_component.hpp>
#include <core/components/spatial_component.hpp>
#include <core/screen.hpp>

namespace luck
{
	camera_system::camera_system(renderable_system& renderable_system) : Base(anax::ComponentFilter().requires<spatial_component, camera_component>()), _renderable_system(renderable_system) {}

	void camera_system::render() ///@todo filter what a camera will render based on rendering flags
	{
		auto cameras = getEntities();

		if(cameras.size())
			camera_component::main = cameras[0];

	for(anax::Entity e : cameras)
		{
			_renderable_system.render(e);
		}
	}

	anax::Entity camera_component::main {};

	glm::mat4 camera_system::calculate_view(anax::Entity e)
	{
		auto& c_spatial = e.getComponent<spatial_component>();

		auto camera_roll_direction = c_spatial.rotation * glm::vec3(0, 0, -1);
		auto camera_pitch_direction = c_spatial.rotation * glm::vec3(-1, 0, 0);
		return glm::lookAt(c_spatial.position, c_spatial.position + camera_roll_direction, glm::cross(camera_roll_direction, camera_pitch_direction));
	}

	glm::mat4 camera_system::calculate_projection(anax::Entity e)
	{
		auto c_camera = e.getComponent<camera_component>();
		glm::vec2 screen_size = luck::screen::size();
		if(c_camera.type == camera_component::Type::PERSPECTIVE)
			return glm::perspective(c_camera.fov, (float)screen_size.x / (float)screen_size.y, c_camera.near, c_camera.far);

		return glm::ortho(0.f, (float)screen_size.x, (float)screen_size.y, 0.f, -1.f, 1.f);
	}
}
