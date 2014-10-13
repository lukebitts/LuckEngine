#include <core/common/aabb.hpp>
#include <core/opengl.hpp>
#include <glm/gtx/component_wise.hpp>

namespace glm
{

	aabb::aabb()
	{
		setNull();
	}

	aabb::aabb(const glm::vec3& center, glm::float_t radius)
	{
		setNull();
		extend(center, radius);
	}

	aabb::aabb(const glm::vec3& p1, const glm::vec3& p2)
	{
		setNull();
		extend(p1);
		extend(p2);
	}

	aabb::aabb(const aabb& aabb)
	{
		setNull();
		extend(aabb);
	}

	aabb::~aabb()
	{
	}

	void aabb::extend(glm::float_t val)
	{
		if(!isNull())
		{
			min -= glm::vec3(val);
			max += glm::vec3(val);
		}
	}

	void aabb::extend(const glm::vec3& p)
	{
		if(!isNull())
		{
			min = glm::min(p, min);
			max = glm::max(p, max);
		}
		else
		{
			min = p;
			max = p;
		}
	}

	void aabb::extend(const glm::vec3& p, glm::float_t radius)
	{
		glm::vec3 r(radius);
		if(!isNull())
		{
			min = glm::min(p - r, min);
			max = glm::max(p + r, max);
		}
		else
		{
			min = p - r;
			max = p + r;
		}
	}

	void aabb::extend(const aabb& aabb)
	{
		if(!aabb.isNull())
		{
			extend(aabb.min);
			extend(aabb.max);
		}
	}

	void aabb::extendDisk(const glm::vec3& c, const glm::vec3& n, glm::float_t r)
	{
		if(glm::length(n) < 1.e-12)
		{
			extend(c);
			return;
		}
		glm::vec3 norm = glm::normalize(n);
		glm::float_t x = sqrt(1 - norm.x) * r;
		glm::float_t y = sqrt(1 - norm.y) * r;
		glm::float_t z = sqrt(1 - norm.z) * r;
		extend(c + glm::vec3(x, y, z));
		extend(c - glm::vec3(x, y, z));
	}

	glm::vec3 aabb::getDiagonal() const
	{
		if(!isNull())
			return max - min;
		else
			return glm::vec3(0);
	}

	glm::float_t aabb::getLongestEdge() const
	{
		return glm::compMax(getDiagonal());
	}

	glm::float_t aabb::getShortestEdge() const
	{
		return glm::compMin(getDiagonal());
	}

	glm::vec3 aabb::getCenter() const
	{
		if(!isNull())
		{
			glm::vec3 d = getDiagonal();
			return min + (d * glm::float_t(0.5));
		}
		else
		{
			return glm::vec3(0.0);
		}
	}

	void aabb::translate(const glm::vec3& v)
	{
		if(!isNull())
		{
			min += v;
			max += v;
		}
	}

	void aabb::scale(const glm::vec3& s, const glm::vec3& o)
	{
		if(!isNull())
		{
			min -= o;
			max -= o;

			min *= s;
			max *= s;

			min += o;
			max += o;
		}
	}

	bool aabb::overlaps(const aabb& bb) const
	{
		if(isNull() || bb.isNull())
			return false;

		if(bb.min.x > max.x || bb.max.x < min.x)
			return false;
		else if(bb.min.y > max.y || bb.max.y < min.y)
			return false;
		else if(bb.min.z > max.z || bb.max.z < min.z)
			return false;

		return true;
	}

	aabb::INTERSECTION_TYPE aabb::intersect(const aabb& b) const
	{
		if(isNull() || b.isNull())
			return OUTSIDE;

		if((max.x < b.min.x) || (min.x > b.max.x) ||
		   (max.y < b.min.y) || (min.y > b.max.y) ||
		   (max.z < b.min.z) || (min.z > b.max.z))
		{
			return OUTSIDE;
		}

		if((min.x <= b.min.x) && (max.x >= b.max.x) &&
		   (min.y <= b.min.y) && (max.y >= b.max.y) &&
		   (min.z <= b.min.z) && (max.z >= b.max.z))
		{
			return INSIDE;
		}

		return INTERSECT;
	}

	bool aabb::isSimilarTo(const aabb& b, glm::float_t diff) const
	{
		if(isNull() || b.isNull()) return false;

		glm::vec3 acceptable_diff = ((getDiagonal() + b.getDiagonal()) / glm::float_t(2.0)) * diff;
		glm::vec3 min_diff(min - b.min);
		min_diff = glm::vec3(fabs(min_diff.x), fabs(min_diff.y), fabs(min_diff.z));
		if(min_diff.x > acceptable_diff.x) return false;
		if(min_diff.y > acceptable_diff.y) return false;
		if(min_diff.z > acceptable_diff.z) return false;
		glm::vec3 max_diff(max - b.max);
		max_diff = glm::vec3(fabs(max_diff.x), fabs(max_diff.y), fabs(max_diff.z));
		if(max_diff.x > acceptable_diff.x) return false;
		if(max_diff.y > acceptable_diff.y) return false;
		if(max_diff.z > acceptable_diff.z) return false;
		return true;
	}

	float aabb::getPerimeter() const
	{
		float wx = getMax().x - getMin().x;
		float wy = getMax().y - getMin().y;
		return 2.0f * (wx + wy);
	}

	void aabb::combine(const aabb& aabb1, const aabb& aabb2)
	{
		min = glm::vec3(glm::min(aabb1.min.x, aabb2.min.x), glm::min(aabb1.min.y, aabb2.min.y), glm::min(aabb1.min.z, aabb2.min.z));
		max = glm::vec3(glm::max(aabb1.max.x, aabb2.max.x), glm::max(aabb1.max.y, aabb2.max.y), glm::max(aabb1.max.z, aabb2.max.z));
	}

	bool aabb::contains(const glm::aabb& aabb) const
	{
		bool result = true;
		result = result && min.x <= aabb.min.x;
		result = result && min.y <= aabb.min.y;
		result = result && aabb.max.x <= max.x;
		result = result && aabb.max.y <= max.y;
		return result;
	}

	std::array<glm::vec3, 8> aabb::vertices() const
	{
		std::array<glm::vec3, 8> vertices;

		vertices[0] = min;
		vertices[1] = glm::vec3(max.x, min.y, min.z);
		vertices[2] = glm::vec3(min.x, max.y, min.z);
		vertices[3] = glm::vec3(min.x, min.y, max.z);
		vertices[4] = glm::vec3(min.x, max.y, max.z);
		vertices[5] = glm::vec3(max.x, min.y, max.z);
		vertices[6] = glm::vec3(max.x, max.y, min.z);
		vertices[7] = max;

		return vertices;
	}

	void aabb::rotate(glm::quat orientation)
	{
		auto v = vertices();

		glm::mat4 mat_model(1.f);
		mat_model = mat_model * glm::toMat4(orientation);

		for (int i = 0; i < 8; ++i)
		{
			v[i] = glm::vec3(mat_model * glm::vec4(v[i], 1.f));
		}

		min = v[0];
		max = v[0];

		for (int i = 0; i < 8; ++i)
		{
			min = glm::min(min, v[i]);
			max = glm::max(max, v[i]);
		}
	}
	
	void aabb::draw() const
	{
		auto mode = GL_LINES;
		auto m_min = min;
		auto m_max = max;
	
		//--- pos z
		glBegin(mode);
		glNormal3f(0,0,1);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_min.y,(GLfloat)m_max.z);
		glNormal3f(0,0,1);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_min.y,(GLfloat)m_max.z);
		glNormal3f(0,0,1);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_max.y,(GLfloat)m_max.z);
		glNormal3f(0,0,1);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_max.y,(GLfloat)m_max.z);
		glEnd();

		//--- pos x
		glBegin(mode);
		glNormal3f(1,0,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_min.y,(GLfloat)m_max.z);
		glNormal3f(1,0,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_min.y,(GLfloat)m_min.z);
		glNormal3f(1,0,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_max.y,(GLfloat)m_min.z);
		glNormal3f(1,0,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_max.y,(GLfloat)m_max.z);
		glEnd();

		//---- pos y
		glBegin(mode);
		glNormal3f(0,1,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_max.y,(GLfloat)m_max.z);
		glNormal3f(0,1,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_max.y,(GLfloat)m_max.z);
		glNormal3f(0,1,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_max.y,(GLfloat)m_min.z);
		glNormal3f(0,1,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_max.y,(GLfloat)m_min.z);
		glEnd();

		//--- neg z
		glBegin(mode);
		glNormal3f(0,0,-1);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_min.y,(GLfloat)m_min.z);
		glNormal3f(0,0,-1);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_max.y,(GLfloat)m_min.z);
		glNormal3f(0,0,-1);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_max.y,(GLfloat)m_min.z);
		glNormal3f(0,0,-1);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_min.y,(GLfloat)m_min.z);
		glEnd();

		//--- neg y
		glBegin(mode);
		glNormal3f(0,-1,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_min.y,(GLfloat)m_min.z);
		glNormal3f(0,-1,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_min.y,(GLfloat)m_min.z);
		glNormal3f(0,-1,0);
		glVertex3f((GLfloat)m_max.x,(GLfloat)m_min.y,(GLfloat)m_max.z);
		glNormal3f(0,-1,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_min.y,(GLfloat)m_max.z);
		glEnd();

		//--- neg x
		glBegin(mode);
		glNormal3f(-1,0,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_min.y,(GLfloat)m_min.z);
		glNormal3f(-1,0,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_min.y,(GLfloat)m_max.z);
		glNormal3f(-1,0,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_max.y,(GLfloat)m_max.z);
		glNormal3f(-1,0,0);
		glVertex3f((GLfloat)m_min.x,(GLfloat)m_max.y,(GLfloat)m_min.z);
		glEnd();
	}
}
