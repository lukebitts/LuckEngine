#include <core/systems/camera_system.hpp>

#include <core/systems/renderable_system.hpp>
#include <core/components/camera_component.hpp>
#include <core/components/spatial_component.hpp>
#include <core/screen.hpp>

namespace luck
{
	camera_system::camera_system(renderable_system& renderable_system) : Base(anax::ComponentFilter().requires<spatial_component, camera_component>()), m_renderable_system(renderable_system) {}

	void camera_system::render() ///@todo filter what a camera will render based on rendering flags
	{
		auto cameras = getEntities();
		std::sort(cameras.begin(), cameras.end(), [](const luck::entity& a, const luck::entity& b){
			return a.getComponent<luck::camera_component>().depth < b.getComponent<luck::camera_component>().depth;
		});

		if (cameras.size() && !camera_component::main.isValid())
			camera_component::main = cameras[0];

		for(anax::Entity e : cameras)
		{
			auto c = e.getComponent<luck::camera_component>();

			glClearColor(c.clearColor.r, c.clearColor.g, c.clearColor.b, 1.f);
			
			if (c.clearFlags == luck::camera_component::ClearFlags::ALL)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else if (c.clearFlags == luck::camera_component::ClearFlags::COLOR)
			{
				glClear(GL_COLOR_BUFFER_BIT);
			}
			else if (c.clearFlags == luck::camera_component::ClearFlags::DEPTH)
			{
				glClear(GL_DEPTH_BUFFER_BIT);
			}

			m_renderable_system.render(e);
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
		
		if (c_camera.type == camera_component::Type::PERSPECTIVE)
		{
			float width = c_camera.sensor.x;
			float lens = width / (2.f * tan(0.5f * glm::radians(c_camera.fov)));
			auto default_frustum = compute_default_frustum(c_camera.near, c_camera.far, lens, c_camera.sensor.x, c_camera.sensor.y, 0, (float)screen_size.x / (float)screen_size.y);
			return glm::frustum(default_frustum.x1, default_frustum.x2, default_frustum.y1, default_frustum.y2, default_frustum.camnear, default_frustum.camfar);
		}
		else 
		{
			auto ortho_frustum = compute_default_ortho(c_camera.near, c_camera.far, c_camera.fov, (float)screen_size.x / (float)screen_size.y, 0);
			return glm::ortho(ortho_frustum.x1, ortho_frustum.x2, ortho_frustum.y1, ortho_frustum.y2, ortho_frustum.camnear, ortho_frustum.camfar);
		}
	}

	detail::frustum camera_system::compute_default_frustum(
		const float camnear,
		const float camfar,
		const float lens,
		const float sensor_x, const float sensor_y,
		const short sensor_fit,
		const float design_aspect_ratio
		) {
		float halfSize;
		float sizeX;
		float sizeY;

		if (sensor_fit == 0) { //auto
			halfSize = (sensor_x / 2.f) * camnear / lens;

			if (design_aspect_ratio > 1.f) {
				// halfsize defines the width
				sizeX = halfSize;
				sizeY = halfSize / design_aspect_ratio;
			}
			else {
				// halfsize defines the height
				sizeX = halfSize * design_aspect_ratio;
				sizeY = halfSize;
			}
		}
		else if (sensor_fit == 1) { //horizontal
			halfSize = (sensor_x / 2.f) * camnear / lens;
			sizeX = halfSize;
			sizeY = halfSize / design_aspect_ratio;
		}
		else {
			halfSize = (sensor_y / 2.f) * camnear / lens;
			sizeX = halfSize * design_aspect_ratio;
			sizeY = halfSize;
		}

		detail::frustum frustum;

		frustum.x2 = sizeX;
		frustum.x1 = -frustum.x2;
		frustum.y2 = sizeY;
		frustum.y1 = -frustum.y2;
		frustum.camnear = camnear;
		frustum.camfar = camfar;

		return frustum;
	}

	detail::frustum camera_system::compute_default_ortho(
		const float camnear,
		const float camfar,
		const float scale,
		const float design_aspect_ratio,
		const short sensor_fit
		)
	{
		float halfSize = scale*0.5f;
		float sizeX;
		float sizeY;

		if (sensor_fit == 0) { //auto
			if (design_aspect_ratio > 1.f) {
				// halfsize defines the width
				sizeX = halfSize;
				sizeY = halfSize / design_aspect_ratio;
			}
			else {
				// halfsize defines the height
				sizeX = halfSize * design_aspect_ratio;
				sizeY = halfSize;
			}
		}
		else if (sensor_fit == 1) { //horizontal
			sizeX = halfSize;
			sizeY = halfSize / design_aspect_ratio;
		}
		else { //vertical
			sizeX = halfSize * design_aspect_ratio;
			sizeY = halfSize;
		}

		detail::frustum frustum;

		frustum.x2 = sizeX;
		frustum.x1 = -frustum.x2;
		frustum.y2 = sizeY;
		frustum.y1 = -frustum.y2;
		frustum.camnear = camnear;
		frustum.camfar = camfar;

		return frustum;
	}
}
