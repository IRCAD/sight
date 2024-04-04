/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include <sight/filter/image/config.hpp>

#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/mesh.hpp>

#include <glm/vec3.hpp>

namespace sight::filter::image
{

/**
 * @brief This class implements one static method that call an image type dispatcher to extrude voxels that are
 * inside a mesh.
 *
 * The only way to use this class is to call @ref extrude(const data::image::sptr&, const data::mesh::csptr&),
 * which sets all voxels inside of the mesh to an empty value. To compute this quickly, we loop over two dimensions out
 * of three, for each voxel, we launch a ray on the third dimension and get a list of intersections between this ray,
 * and triangles of the mesh. After that, we iterate over the voxel line on the third dimension and look where it's
 * located relatively to intersections, it allows to know if the voxel is inside or outside of the mesh.
 *
 * @pre The input image must be in 3D.
 * @pre Input meshes must have cells with 3 or 4 points.
 * @warning The image and meshes must be used without transformation matrices in 3D scene, no matrices are used here.
 */
class SIGHT_FILTER_IMAGE_CLASS_API image_extruder final
{
public:

    /**
     * @brief Sets all voxels of the image that are inside the mesh to an empty value.
     * @param _image image to extrude.
     * @param _mesh mesh use to compute the extrusion.
     * @param _transform transform of the image, if any.
     *
     * @warning No data are locked here, it must be done before.
     * @warning No signals are sent here, it must be done after.
     */
    static SIGHT_FILTER_IMAGE_API void extrude(
        const data::image::sptr& _image,
        const data::mesh::csptr& _mesh,
        const data::matrix4::csptr& _transform
    );

    /**
     * @brief Stores parameters of the functor.
     * @warning It's not exported since it's only used by the functor.
     */
    struct parameters
    {
        data::image::sptr m_image;
        data::mesh::csptr m_mesh;
        data::matrix4::csptr m_transform;
    };

    /**
     * @brief Dispatches the algorithm for image types.
     * @warning It's not exported, it's only public since the dispatcher need it.
     */
    template<typename IMAGE_TYPE>
    void operator()(parameters& _param);

private:

    /// Represents a 3D triangle by three points.
    struct triangle
    {
        const glm::vec3 a;
        const glm::vec3 b;
        const glm::vec3 c;
    };
};

} // namespace sight::filter::image
