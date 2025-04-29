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

#include "fiducials.hpp"

#include "viz/scene3d/helper/manual_object.hpp"
#include "viz/scene3d/ogre.hpp"

#include <core/exceptionmacros.hpp>

#include <OgreMath.h>

namespace sight::viz::scene3d::helper
{

//------------------------------------------------------------------------------

void fiducials::draw_contour_data(
    const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
    Ogre::ManualObject* _object,
    const std::string& _material_name,
    const Ogre::ColourValue& _color,
    bool _visible
)
{
    _object->begin(
        _material_name,
        Ogre::RenderOperation::OT_LINE_LIST,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    for(const auto& q : _fiducial_query)
    {
        // We must have a contour data to draw and there must be at least 2 points available
        if(q.m_contour_data.has_value() && q.m_contour_data->size() > 6
           && q.m_visible.has_value() && q.m_visible.value())
        {
            if(q.m_color.has_value())
            {
                std::array<float, 4> c = q.m_color.value();
                Ogre::ColourValue cv {c[0], c[1], c[2], c[3]};
                _object->colour(cv);
            }
            else
            {
                _object->colour(_color);
            }

            const auto size = q.m_contour_data->size() - 3;
            for(std::size_t i = 0 ; i < size ; i = i + 3)
            {
                _object->position(
                    static_cast<float>(q.m_contour_data->at(i)),
                    static_cast<float>(q.m_contour_data->at(i + 1)),
                    static_cast<float>(q.m_contour_data->at(i + 2))
                );
                _object->position(
                    static_cast<float>(q.m_contour_data->at(i + 3)),
                    static_cast<float>(q.m_contour_data->at(i + 4)),
                    static_cast<float>(q.m_contour_data->at(i + 5))
                );
            }
        }
    }

    _object->end();
    _object->setQueryFlags(0x0);
    _object->setVisible(_visible);
}

//------------------------------------------------------------------------------

void fiducials::draw_graphic_data(
    const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
    Ogre::ManualObject* _object,
    const std::string& _material_name,
    const Ogre::ColourValue& _color,
    std::array<double, 2> _scaling,
    bool _visible
)
{
    _object->begin(
        _material_name,
        Ogre::RenderOperation::OT_LINE_LIST,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    for(const auto& q : _fiducial_query)
    {
        if(q.m_graphic_data.has_value() && q.m_graphic_data->size() > 4
           && q.m_visible.has_value() && q.m_visible.value())
        {
            if(q.m_color.has_value())
            {
                std::array<float, 4> c = q.m_color.value();
                Ogre::ColourValue cv {c[0], c[1], c[2], c[3]};
                _object->colour(cv);
            }
            else
            {
                _object->colour(_color);
            }

            const auto& gd = q.m_graphic_data.value();

            const auto size = gd.size() - 2;
            for(std::size_t i = 0 ; i < size ; i = i + 2)
            {
                _object->position(
                    static_cast<float>(gd.at(i) * _scaling[0]),
                    static_cast<float>(gd.at(i + 1) * _scaling[1]),
                    0.F
                );
                _object->position(
                    static_cast<float>(gd.at(i + 2) * _scaling[0]),
                    static_cast<float>(gd.at(i + 3) * _scaling[1]),
                    0.F
                );
            }
        }
    }

    _object->end();
    _object->setQueryFlags(0x0);
    _object->setVisible(_visible);
}

//-----------------------------------------------------------------------------

void fiducials::draw_contour_control_points(
    const std::vector<sight::data::fiducials_series::query_result>& _fiducial_query,
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::ColourValue& _color,
    float _radius,
    unsigned int _sample,
    bool _visible
)
{
    _object->begin(_material, Ogre::RenderOperation::OT_TRIANGLE_LIST, sight::viz::scene3d::RESOURCE_GROUP);

    Ogre::uint32 index = 0;
    for(const auto& q : _fiducial_query)
    {
        if(q.m_contour_data.has_value() && q.m_visible.has_value() && q.m_visible.value())
        {
            const auto size = q.m_contour_data.value().size();
            for(std::size_t i = 0 ; i < size ; i = i + 3)
            {
                Ogre::Vector3 v {static_cast<float>(q.m_contour_data->at(i)),
                                 static_cast<float>(q.m_contour_data->at(i + 1)),
                                 static_cast<float>(q.m_contour_data->at(i + 2))
                };
                index = sight::viz::scene3d::helper::manual_object::draw_sphere(
                    _object,
                    v,
                    _color,
                    _radius,
                    _sample,
                    index
                );
            }
        }
    }

    _object->end();
    _object->setVisible(_visible);
}

//------------------------------------------------------------------------------

void fiducials::draw_graphic_control_points(
    const std::vector<sight::data::fiducials_series::query_result>
    & _fiducial_query,
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::ColourValue& _color,
    std::array<double, 2> _scaling,
    float _radius,
    unsigned int _sample,
    bool _visible
)
{
    _object->begin(_material, Ogre::RenderOperation::OT_TRIANGLE_LIST, sight::viz::scene3d::RESOURCE_GROUP);

    Ogre::uint32 index = 0;
    for(const auto& q : _fiducial_query)
    {
        if(q.m_graphic_data.has_value() && q.m_visible.has_value() && q.m_visible.value())
        {
            const auto& gd  = q.m_graphic_data.value();
            const auto size = gd.size();
            for(std::size_t i = 0 ; i < size ; i = i + 2)
            {
                Ogre::Vector3 v {
                    static_cast<float>(gd.at(i) * _scaling[0]),
                    static_cast<float>(gd.at(i + 1) * _scaling[1]),
                    0.F
                };

                index = sight::viz::scene3d::helper::manual_object::draw_sphere(
                    _object,
                    v,
                    _color,
                    _radius,
                    _sample,
                    index
                );
            }
        }
    }

    _object->end();
    _object->setVisible(_visible);
}

} // namespace sight::viz::scene3d::helper
