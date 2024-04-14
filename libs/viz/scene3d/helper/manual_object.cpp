/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "viz/scene3d/helper/manual_object.hpp"

#include "viz/scene3d/ogre.hpp"

#include <core/exceptionmacros.hpp>

#include <OgreMath.h>

namespace sight::viz::scene3d::helper
{

//-----------------------------------------------------------------------------

void manual_object::create_cylinder(
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::ColourValue& _color,
    float _radius,
    float _length,
    unsigned int _sample
)
{
    // Data to create cylinder in x axis
    const Ogre::Radian delta(Ogre::Degree(360.F / static_cast<float>(_sample)));
    const Ogre::Quaternion rot(delta, Ogre::Vector3::UNIT_X);
    Ogre::Vector3 normal = Ogre::Quaternion(delta / 2.F, Ogre::Vector3::UNIT_X) * Ogre::Vector3::UNIT_Y;

    Ogre::Vector3 p0(0.F, _radius, 0.F);
    Ogre::Vector3 p1(_length, _radius, 0.F);
    Ogre::Vector3 p2 = rot * p0;
    Ogre::Vector3 p3 = rot * p1;

    // Create cylinder
    _object->begin(_material, Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
    _object->colour(_color);
    // Initial points of the basis
    _object->position(Ogre::Vector3::ZERO);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);
    _object->position(Ogre::Vector3(_length, 0.F, 0.F));
    _object->normal(Ogre::Vector3::UNIT_X);
    for(unsigned int i = 0 ; i < _sample ; ++i)
    {
        // Cylinder
        _object->position(p0);
        _object->normal(normal);

        _object->position(p2);
        _object->normal(normal);

        _object->position(p1);
        _object->normal(normal);

        _object->position(p3);
        _object->normal(normal);

        // Basis
        _object->position(p0);
        _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);

        _object->position(p2);
        _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);

        _object->position(p1);
        _object->normal(Ogre::Vector3(_length, 0.F, 0.F));

        _object->position(p3);
        _object->normal(Ogre::Vector3(_length, 0.F, 0.F));

        const unsigned int offset = 2 + (i * 8);
        // Cylinder
        _object->triangle(offset + 1, offset + 3, offset);
        _object->triangle(offset + 3, offset + 2, offset);
        // Basis
        _object->triangle(0, offset + 4, offset + 5);
        _object->triangle(1, offset + 6, offset + 7);

        p0 = p2;
        p1 = p3;
        p2 = rot * p2;
        p3 = rot * p3;

        normal = rot * normal;
    }

    _object->end();

    _object->setBoundingBox(
        Ogre::AxisAlignedBox(
            Ogre::Vector3(0.F, -_radius, -_radius),
            Ogre::Vector3(_length, _radius, _radius)
        )
    );
}

//-----------------------------------------------------------------------------

void manual_object::create_cone(
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::ColourValue& _color,
    float _radius,
    float _length,
    unsigned int _sample
)
{
    // Data to create cone in x axis
    const Ogre::Radian delta(Ogre::Degree(360.F / static_cast<float>(_sample)));
    const Ogre::Quaternion rot(delta, Ogre::Vector3::UNIT_X);
    Ogre::Vector3 normal = Ogre::Quaternion(delta / 2.F, Ogre::Vector3::UNIT_X) * Ogre::Vector3::UNIT_Y;

    Ogre::Vector3 p0(0.F, _radius, 0.F);
    Ogre::Vector3 p1(_length, 0, 0.F);
    Ogre::Vector3 p2 = rot * p0;

    // Compute real normal
    const Ogre::Radian angle(std::atan(_radius / _length));
    normal = Ogre::Quaternion(angle, Ogre::Vector3::UNIT_Z) * normal;

    // Create cone
    _object->begin(_material, Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
    _object->colour(_color);
    // Initial point of the basis
    _object->position(Ogre::Vector3::ZERO);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);
    for(unsigned int i = 0 ; i < _sample ; ++i)
    {
        // Cone
        _object->position(p0);
        _object->normal(normal);

        _object->position(p2);
        _object->normal(normal);

        _object->position(p1);
        _object->normal(normal);

        // Basis
        _object->position(p0);
        _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);

        _object->position(p2);
        _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);

        const unsigned int offset = 1 + (i * 5);
        // Cone
        _object->triangle(offset, offset + 1, offset + 2);
        // basis
        _object->triangle(0, offset + 3, offset + 4);

        p0 = p2;
        p2 = rot * p2;

        normal = rot * normal;
    }

    _object->end();

    _object->setBoundingBox(
        Ogre::AxisAlignedBox(
            Ogre::Vector3(0.F, -_radius, -_radius),
            Ogre::Vector3(_length, _radius, _radius)
        )
    );
}

//------------------------------------------------------------------------------

void manual_object::create_cube(
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::ColourValue& _color,
    float _length
)
{
    float length = _length / 2.0F;

    _object->begin(_material, Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
    _object->colour(_color);

    _object->position(-length, -length, length);
    _object->normal(Ogre::Vector3::UNIT_Z);
    _object->position(-length, length, length);
    _object->normal(Ogre::Vector3::UNIT_Z);
    _object->position(length, length, length);
    _object->normal(Ogre::Vector3::UNIT_Z);
    _object->position(length, -length, length);
    _object->normal(Ogre::Vector3::UNIT_Z);

    _object->triangle(0, 1, 2);
    _object->triangle(2, 3, 0);

    _object->position(-length, -length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Z);
    _object->position(-length, length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Z);
    _object->position(length, length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Z);
    _object->position(length, -length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Z);

    _object->triangle(4, 5, 6);
    _object->triangle(6, 7, 4);

    _object->position(-length, -length, length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);
    _object->position(-length, length, length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);
    _object->position(-length, length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);
    _object->position(-length, -length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_X);

    _object->triangle(8, 9, 10);
    _object->triangle(10, 11, 8);

    _object->position(length, -length, length);
    _object->normal(Ogre::Vector3::UNIT_X);
    _object->position(length, length, length);
    _object->normal(Ogre::Vector3::UNIT_X);
    _object->position(length, length, -length);
    _object->normal(Ogre::Vector3::UNIT_X);
    _object->position(length, -length, -length);
    _object->normal(Ogre::Vector3::UNIT_X);

    _object->triangle(12, 13, 14);
    _object->triangle(14, 15, 12);

    _object->position(-length, -length, length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Y);
    _object->position(length, -length, length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Y);
    _object->position(length, -length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Y);
    _object->position(-length, -length, -length);
    _object->normal(Ogre::Vector3::NEGATIVE_UNIT_Y);

    _object->triangle(16, 17, 18);
    _object->triangle(18, 19, 16);

    _object->position(-length, length, length);
    _object->normal(Ogre::Vector3::UNIT_Y);
    _object->position(length, length, length);
    _object->normal(Ogre::Vector3::UNIT_Y);
    _object->position(length, length, -length);
    _object->normal(Ogre::Vector3::UNIT_Y);
    _object->position(-length, length, -length);
    _object->normal(Ogre::Vector3::UNIT_Y);

    _object->triangle(20, 21, 22);
    _object->triangle(22, 23, 20);

    _object->end();

    _object->setBoundingBox(
        Ogre::AxisAlignedBox(
            {-length, -length, -length},
            {length, length, length
            })
    );
}

//------------------------------------------------------------------------------

void manual_object::create_sphere(
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::ColourValue& _color,
    float _radius,
    unsigned int _sample
)
{
    _object->begin(_material, Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
    _object->colour(_color);

    const float delta_ring = (static_cast<float>(Ogre::Math::PI) / static_cast<float>(_sample));
    const float delta_seg  = (2 * static_cast<float>(Ogre::Math::PI) / static_cast<float>(_sample));
    Ogre::uint32 index     = 0;

    for(unsigned ring = 0 ; ring <= _sample ; ++ring)
    {
        const float r0 = _radius * std::sin(static_cast<float>(ring) * delta_ring);
        const float y0 = _radius * std::cos(static_cast<float>(ring) * delta_ring);

        for(unsigned seg = 0 ; seg <= _sample ; ++seg)
        {
            const float x0 = r0 * std::sin(static_cast<float>(seg) * delta_seg);
            const float z0 = r0 * std::cos(static_cast<float>(seg) * delta_seg);

            _object->position(x0, y0, z0);
            _object->normal(Ogre::Vector3(x0, y0, z0).normalisedCopy());

            if(ring != _sample)
            {
                _object->index(index + _sample + 1);
                _object->index(index);
                _object->index(index + _sample);
                _object->index(index + _sample + 1);
                _object->index(index + 1);
                _object->index(index);
                ++index;
            }
        }
    }

    _object->end();

    _object->setBoundingBox(Ogre::AxisAlignedBox({-_radius, -_radius, -_radius}, {_radius, _radius, _radius}));
}

//------------------------------------------------------------------------------

void manual_object::create_frustum(
    Ogre::ManualObject* _object,
    const std::string& _material,
    const Ogre::Frustum& _frustum
)
{
    _object->begin(_material, Ogre::RenderOperation::OT_LINE_LIST, RESOURCE_GROUP);

    const Ogre::Frustum::Corners& corners = _frustum.getWorldSpaceCorners();

    std::ranges::for_each(corners, [_object](const auto& _corner){_object->position(_corner);});

    // see AxisAlignedBox::getAllCorners
    const std::vector<Ogre::uint32> idx = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 6, 1, 5, 2, 4, 3, 7};

    std::ranges::for_each(idx, [_object](const auto _i){_object->index(_i);});

    _object->end();

    Ogre::Vector3 min(std::numeric_limits<Ogre::Real>::max());
    Ogre::Vector3 max(std::numeric_limits<Ogre::Real>::lowest());

    for(size_t i = 0 ; i < 7 ; ++i)
    {
        std::tie(min.x, max.x) = std::minmax(corners[i].x, corners[i + 1].x);
        std::tie(min.y, max.y) = std::minmax(corners[i].y, corners[i + 1].y);
        std::tie(min.z, max.z) = std::minmax(corners[i].z, corners[i + 1].z);
    }

    _object->setBoundingBox(Ogre::AxisAlignedBox(min, max));
}

//------------------------------------------------------------------------------

void manual_object::draw_dashed_line(
    Ogre::ManualObject* _object,
    const Ogre::Vector3& _p1,
    const Ogre::Vector3& _p2,
    const float _dash_length,
    const float _dash_spacing,
    std::optional<Ogre::ColourValue> _color
)
{
    SIGHT_ASSERT("Invalid argument", _object != nullptr);

    const auto vec    = _p2 - _p1;
    const auto length = vec.length();

    if(length <= (_dash_length + _dash_spacing))
    {
        // No need to draw a dashed line
        return;
    }

    const auto num_dashes  = length / (_dash_length + _dash_spacing);
    const auto inc_dash    = vec * _dash_length / length;
    const auto inc_spacing = vec * _dash_spacing / length;
    const auto inc         = vec / num_dashes;
    Ogre::Vector3 cur      = _p1 + inc_spacing;
    for(std::size_t i = 0 ; i < static_cast<std::size_t>(num_dashes) ; ++i)
    {
        _object->position(cur);
        if(_color.has_value())
        {
            _object->colour(_color.value());
        }

        _object->position(cur + inc_dash);
        if(_color.has_value())
        {
            _object->colour(_color.value());
        }

        cur += inc;
    }
}

} // namespace sight::viz::scene3d::helper
