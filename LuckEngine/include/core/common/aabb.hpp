#ifndef AABB_HPP
#define AABB_HPP

#include "glm.hpp"

namespace glm
{

	/// Standalone axis aligned bounding box implemented built on top of GLM.
	class aabb ///@todo change the methods names to follow glm's
	{
		public:
			/// Builds a null aabb.
			aabb();

			/// Builds an aabb that encompasses a sphere.
			/// \param[in]  center Center of the sphere.
			/// \param[in]  radius Radius of the sphere.
			aabb(const glm::vec3& center, glm::float_t radius);

			/// Builds an aabb that contains the two points.
			aabb(const glm::vec3& p1, const glm::vec3& p2);

			aabb(const aabb& aabb);
			~aabb();

			/// Set the aabb as NULL (not set).
			void setNull()
			{
				min = glm::vec3(1.0);
				max = glm::vec3(-1.0);
			}

			/// Returns true if aabb is NULL (not set).
			bool isNull() const
			{
				return min.x > max.x || min.y > max.y || min.z > max.z;
			}

			/// Extend the bounding box on all sides by \p val.
			void extend(glm::float_t val);

			/// Expand the aabb to include point \p p.
			void extend(const glm::vec3& p);

			/// Expand the aabb to include a sphere centered at \p center and of radius \p
			/// radius.
			/// \param[in]  center Center of sphere.
			/// \param[in]  radius Radius of sphere.
			void extend(const glm::vec3& center, glm::float_t radius);

			/// Expand the aabb to encompass the given \p aabb.
			void extend(const aabb& aabb);

			/// Expand the aabb to include a disk centered at \p center, with normal \p
			/// normal, and radius \p radius.
			/// \xxx Untested -- This function is not represented in our unit tests.
			void extendDisk(const glm::vec3& center, const glm::vec3& normal,
			                glm::float_t radius);

			/// Translates aabb by vector \p v.
			void translate(const glm::vec3& v);

			/// Scale the aabb by \p scale, centered around \p origin.
			/// \param[in]  scale  3D vector specifying scale along each axis.
			/// \param[in]  origin Origin of scaling operation. Most useful origin would
			///                    be the center of the aabb.
			void scale(const glm::vec3& scale, const glm::vec3& origin);

			/// Retrieves the center of the aabb.
			glm::vec3 getCenter() const;

			/// Retrieves the diagonal vector (computed as max - min).
			/// If the aabb is NULL, then a vector of all zeros is returned.
			glm::vec3 getDiagonal() const;

			/// Retrieves the longest edge.
			/// If the aabb is NULL, then 0 is returned.
			glm::float_t getLongestEdge() const;

			/// Retrieves the shortest edge.
			/// If the aabb is NULL, then 0 is returned.
			glm::float_t getShortestEdge() const;

			/// Retrieves the aabb's minimum point.
			glm::vec3 getMin() const
			{
				return min;
			}

			/// Retrieves the aabb's maximum point.
			glm::vec3 getMax() const
			{
				return max;
			}

			/// Returns true if aabbs share a face overlap.
			/// \xxx Untested -- This function is not represented in our unit tests.
			bool overlaps(const aabb& bb) const;

			/// Type returned from call to intersect.
			enum INTERSECTION_TYPE { INSIDE, INTERSECT, OUTSIDE };
			/// Returns one of the intersection types. If either of the aabbs are invalid,
			/// then OUTSIDE is returned.
			INTERSECTION_TYPE intersect(const aabb& bb) const;

			/// Function from SCIRun. Here is a summary of SCIRun's description:
			/// Returns true if the two aabb's are similar. If diff is 1.0, the two
			/// bboxes have to have about 50% overlap each for x,y,z. If diff is 0.0,
			/// they have to have 100% overlap.
			/// If either of the two aabbs is NULL, then false is returned.
			/// \xxx Untested -- This function is not represented in our unit tests.
			bool isSimilarTo(const aabb& b, glm::float_t diff = 0.5) const;

			/// Lucas:
			float getPerimeter() const;
			void combine(const aabb& aabb1, const aabb& aabb2);
			bool contains(const glm::aabb& aabb) const;
			void draw() const;

			glm::vec3 min;   ///< Minimum point.
			glm::vec3 max;   ///< Maximum point.
	};
}

#endif //AABB_HPP
