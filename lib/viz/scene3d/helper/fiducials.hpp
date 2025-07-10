/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <data/fiducials_series.hpp>
#include <data/image.hpp>

#include <Ogre.h>

namespace sight::viz::scene3d::helper
{

class fiducials
{
public:

    /**
     * @brief Draws contour data from fiducials.
     *
     * @param _fiducial_query query results for fiducials.
     * @param _object line objects (plain lines).
     * @param _material_name line material name.
     * @param _color line color.
     * @param _visible current visibility.
     */
    SIGHT_VIZ_SCENE3D_API static void draw_contour_data(
        const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
        Ogre::ManualObject* _object,
        const std::string& _material_name,
        const Ogre::ColourValue& _color,
        bool _visible = true
    );

    /**
     * @brief Draws graphic data from fiducials.
     *
     * @param _fiducial_query query results for fiducials.
     * @param _object line objects (plain lines).
     * @param _material_name visible line material name.
     * @param _color line color.
     * @param _scaling scaling factors applied to the graphic data values (e.g. image spacing).
     * @param _visible current visibility.
     */
    SIGHT_VIZ_SCENE3D_API static void draw_graphic_data(
        const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
        Ogre::ManualObject* _line_object,
        const std::string& _material_name,
        const Ogre::ColourValue& _color,
        std::array<double, 2> _scaling,
        bool _visible = true
    );

    /**
     * @brief Draws control points from the contour data from the fiducials.
     *
     * @param _fiducial_query query results for fiducials.
     * @param _object control point object (spheres).
     * @param _material_name material name to use.
     * @param _color line color.
     * @param _radius radius of the control points.
     * @param _sample number of samples using to discretize polar coordinates.
     * @param _visible current visibility.
     */
    SIGHT_VIZ_SCENE3D_API static void draw_contour_control_points(
        const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
        Ogre::ManualObject* _object,
        const std::string& _material_name,
        const Ogre::ColourValue& _color,
        float _radius,
        unsigned int _sample,
        bool _visible = true
    );

    /**
     * @brief Draws control points of the graphic data from the fiducials.
     *
     * @param _fiducial_query query results for fiducials.
     * @param _object control point object (spheres).
     * @param _material material name to use.
     * @param _color line color.
     * @param _scaling scaling factors applied to the graphic data values (e.g. image spacing).
     * @param _radius radius of the control points.
     * @param _sample number of samples using to discretize polar coordinates.
     * @param _visible current visibility.
     */
    SIGHT_VIZ_SCENE3D_API static void draw_graphic_control_points(
        const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
        Ogre::ManualObject* _object,
        const std::string& _material,
        const Ogre::ColourValue& _color,
        std::array<double, 2> _scaling,
        float _radius,
        unsigned int _sample,
        bool _visible = true
    );
};

} // namespace sight::viz::scene3d::helper
