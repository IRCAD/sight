/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/ManualObject.hpp"

namespace fwRenderOgre
{

namespace helper
{

//-----------------------------------------------------------------------------

void ManualObject::createCylinder(::Ogre::ManualObject* _object,
                                  const std::string& _material,
                                  const ::Ogre::ColourValue& _color,
                                  float _thickness,
                                  float _length,
                                  unsigned int _sample)
{
    // Data to create cylinder in x axis
    const ::Ogre::Radian delta( ::Ogre::Degree(360.f / static_cast< float > (_sample)) );
    const ::Ogre::Quaternion rot( delta, ::Ogre::Vector3::UNIT_X );
    ::Ogre::Vector3 normal = ::Ogre::Quaternion( delta/2.f, ::Ogre::Vector3::UNIT_X ) * ::Ogre::Vector3::UNIT_Y;

    ::Ogre::Vector3 p0(0.f, _thickness, 0.f);
    ::Ogre::Vector3 p1(_length, _thickness, 0.f);
    ::Ogre::Vector3 p2 = rot * p0;
    ::Ogre::Vector3 p3 = rot * p1;

    // Create cylinder
    _object->begin(_material, ::Ogre::RenderOperation::OT_TRIANGLE_LIST);
    _object->colour(_color);
    // Initial points of the basis
    _object->position(::Ogre::Vector3::ZERO);
    _object->normal(::Ogre::Vector3::NEGATIVE_UNIT_X);
    _object->position(::Ogre::Vector3(_length, 0.f, 0.f));
    _object->normal(::Ogre::Vector3::UNIT_X);
    for (unsigned int i = 0; i < _sample; ++i)
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
        _object->normal(::Ogre::Vector3::NEGATIVE_UNIT_X);

        _object->position(p2);
        _object->normal(::Ogre::Vector3::NEGATIVE_UNIT_X);

        _object->position(p1);
        _object->normal(::Ogre::Vector3(_length, 0.f, 0.f));

        _object->position(p3);
        _object->normal(::Ogre::Vector3(_length, 0.f, 0.f));

        const unsigned int offset = 2+(i * 8);
        // Cylinder
        _object->triangle(offset+1, offset+3, offset);
        _object->triangle(offset+3, offset+2, offset);
        // Basis
        _object->triangle(0, offset+4, offset+5);
        _object->triangle(1, offset+6, offset+7);

        p0 = p2;
        p1 = p3;
        p2 = rot * p2;
        p3 = rot * p3;

        normal = rot * normal;
    }
    _object->end();
}

//-----------------------------------------------------------------------------

void ManualObject::createCone(::Ogre::ManualObject* _object,
                              const std::string& _material,
                              const ::Ogre::ColourValue& _color,
                              float _thickness,
                              float _length,
                              unsigned int _sample)
{
    // Data to create cone in x axis
    const ::Ogre::Radian delta( ::Ogre::Degree(360.f / static_cast< float >(_sample)) );
    const ::Ogre::Quaternion rot( delta, ::Ogre::Vector3::UNIT_X );
    ::Ogre::Vector3 normal = ::Ogre::Quaternion( delta/2.f, ::Ogre::Vector3::UNIT_X ) * ::Ogre::Vector3::UNIT_Y;

    ::Ogre::Vector3 p0(0.f, _thickness, 0.f);
    ::Ogre::Vector3 p1(_length, 0, 0.f);
    ::Ogre::Vector3 p2 = rot * p0;

    // Compute real normal
    const ::Ogre::Radian angle(std::atan(_thickness/ _length));
    normal = ::Ogre::Quaternion(angle, ::Ogre::Vector3::UNIT_Z) * normal;

    // Create cone
    _object->begin(_material, ::Ogre::RenderOperation::OT_TRIANGLE_LIST);
    _object->colour(_color);
    // Initial point of the basis
    _object->position(::Ogre::Vector3::ZERO);
    _object->normal(::Ogre::Vector3::NEGATIVE_UNIT_X);
    for (unsigned int i = 0; i < _sample; ++i)
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
        _object->normal(::Ogre::Vector3::NEGATIVE_UNIT_X);

        _object->position(p2);
        _object->normal(::Ogre::Vector3::NEGATIVE_UNIT_X);

        const unsigned int offset = 1+(i * 5);
        // Cone
        _object->triangle(offset, offset+1, offset+2);
        // basis
        _object->triangle(0, offset+3, offset+4);

        p0 = p2;
        p2 = rot * p2;

        normal = rot * normal;
    }
    _object->end();
}

} // namespace helper

} // namespace fwRenderOgre
