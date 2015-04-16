#ifndef GL_DEBUGDRAWER_HPP
#define GL_DEBUGDRAWER_HPP

#include "../core/opengl.hpp"
#include "../core/common/glm.hpp"
#include "../core/common/debug.hpp"
#include "../core/components/camera_component.hpp"
#include "../core/systems/camera_system.hpp"
#include <bullet/btBulletDynamicsCommon.h>

namespace luck
{

	class gl_debugdrawer : public btIDebugDraw
	{
	private:
		int m_debugMode;
	public:
		gl_debugdrawer() {}
		virtual ~gl_debugdrawer() {}
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& from_color, const btVector3& to_color)
			///@todo: put this in the implementation file
			///@todo: find a faster way to draw debug info
		{
			if (!camera_component::main.isValid()) return;
			glPushMatrix();
			glEnable(GL_DEPTH_TEST);

			glm::mat4 mat_projection = camera_system::calculate_projection(camera_component::main);
			glm::mat4 mat_view = camera_system::calculate_view(camera_component::main);

			glLoadMatrixf(glm::value_ptr(mat_projection * mat_view));

			glBegin(GL_LINES);
			glColor3f(from_color.getX(), from_color.getY(), from_color.getZ());
			glVertex3d(from.getX(), from.getY(), from.getZ());
			glColor3f(to_color.getX(), to_color.getY(), to_color.getZ());
			glVertex3d(to.getX(), to.getY(), to.getZ());
			glEnd();

			glPopMatrix();
		}
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
		{
			drawLine(from, to, color, color);
		}
		virtual void drawSphere(const btVector3&, btScalar, const btVector3&)
		{
			LOG(__FUNCTION__);
		}
		virtual void drawTriangle(const btVector3&, const btVector3&, const btVector3&, const btVector3&, btScalar)
		{
			LOG(__FUNCTION__);
		}
		virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&)
		{
			LOG(__FUNCTION__);
		}
		virtual void reportErrorWarning(const char* warning_string)
		{
			LOG(warning_string);
		}
		virtual void draw3dText(const btVector3&, const char*)
		{
			LOG(__FUNCTION__);
		}
		virtual void setDebugMode(int debugMode)
		{
			m_debugMode = debugMode;
		}
		virtual int getDebugMode() const
		{
			return m_debugMode;
		}
	};

}

#endif