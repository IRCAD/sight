/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "shading_test.hpp"

#include <data/color.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>
#include <data/real.hpp>

#include <viz/scene3d/helper/shading.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::shading_test);

namespace sight::viz::scene3d::helper::ut
{

//------------------------------------------------------------------------------

void shading_test::setUp()
{
}

//------------------------------------------------------------------------------

void shading_test::tearDown()
{
}

//------------------------------------------------------------------------------

void shading_test::get_permutation()
{
    std::string suffix;

    suffix = shading::get_permutation(data::material::shading_t::ambient, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Ambient+VT"), suffix);

    suffix = shading::get_permutation(data::material::shading_t::flat, false, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat"), suffix);

    suffix = shading::get_permutation(data::material::shading_t::flat, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat+VT"), suffix);

    suffix = shading::get_permutation(data::material::shading_t::phong, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT+DfsTex"), suffix);

    suffix = shading::get_permutation(data::material::shading_t::phong, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT"), suffix);
}

//------------------------------------------------------------------------------

void shading_test::get_r2vb_geometry_program_name()
{
    std::string prg_name;

    prg_name = shading::get_r2vb_geometry_program_name(data::mesh::cell_type_t::triangle, true, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT+DfsTex+PPColor_GP"), prg_name);

    prg_name = shading::get_r2vb_geometry_program_name(data::mesh::cell_type_t::triangle, false, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT_GP"), prg_name);

    prg_name = shading::get_r2vb_geometry_program_name(data::mesh::cell_type_t::tetra, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+VT+PPColor_GP"), prg_name);

    prg_name = shading::get_r2vb_geometry_program_name(data::mesh::cell_type_t::tetra, true, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+DfsTex+PPColor_GP"), prg_name);

    prg_name = shading::get_r2vb_geometry_program_name(data::mesh::cell_type_t::quad, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+VT+PPColor_GP"), prg_name);

    prg_name = shading::get_r2vb_geometry_program_name(data::mesh::cell_type_t::quad, false, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+PPColor_GP"), prg_name);
}

//------------------------------------------------------------------------------

void shading_test::set_permutation_in_program_name()
{
    std::string prg_name;

    prg_name = shading::set_permutation_in_program_name("Default/PixelLit+VT+DfsTex_VP", "PixelLit+DfsTex");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit+DfsTex_VP"), prg_name);

    prg_name = shading::set_permutation_in_program_name("CelShadingDepthPeeling/peel_PixelLit+VT+DfsTex_FP", "Flat+VT");
    CPPUNIT_ASSERT_EQUAL(std::string("CelShadingDepthPeeling/peel_Flat+VT_FP"), prg_name);

    prg_name = shading::set_permutation_in_program_name("HybridTransparency/peel_Ambient+VT_FP", "Flat");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel_Flat_FP"), prg_name);
}

//------------------------------------------------------------------------------

void shading_test::set_technique_in_program_name()
{
    std::string prg_name;

    prg_name = shading::set_technique_in_program_name("Default/PixelLit_FP", "DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("DepthPeeling/peel/PixelLit_FP"), prg_name);

    prg_name = shading::set_technique_in_program_name("DepthPeeling/peel_init/PixelLit_FP", "Default");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit_FP"), prg_name);

    prg_name = shading::set_technique_in_program_name("Default/Flat+VT_FP", "HybridTransparency/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel/Flat+VT_FP"), prg_name);
}

//------------------------------------------------------------------------------

void shading_test::create_object_from_shader_parameter()
{
    shading::constant_value_t value {};
    // Scalar types
    {
        value.d = {{2.0, 0., 0., 0.}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_DOUBLE1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), obj->get_classname());
        CPPUNIT_ASSERT_EQUAL(2.0, std::dynamic_pointer_cast<data::real>(obj)->get_value());
    }
    {
        value.f = {{2.5F, 0.F, 0.F, 0.F}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_FLOAT1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), obj->get_classname());
        CPPUNIT_ASSERT_EQUAL(2.5, std::dynamic_pointer_cast<data::real>(obj)->get_value());
    }
    {
        value.i = {{321, 0, 0, 0}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_INT1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::integer"), obj->get_classname());
        CPPUNIT_ASSERT_EQUAL(
            static_cast<std::int64_t>(321),
            std::dynamic_pointer_cast<data::integer>(obj)->get_value()
        );
    }

    {
        value.d = {{2.0, 4.5, 0., 0.}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_DOUBLE2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::dvec2"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::dvec2>(obj);
        CPPUNIT_ASSERT_EQUAL(2.0, (*typed_obj)[0]);
        CPPUNIT_ASSERT_EQUAL(4.5, (*typed_obj)[1]);
    }

    {
        value.d = {{-4.1, 1.5, 3.7, 0.}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_DOUBLE3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::dvec3"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::dvec3>(obj);
        CPPUNIT_ASSERT_EQUAL(-4.1, (*typed_obj)[0]);
        CPPUNIT_ASSERT_EQUAL(1.5, (*typed_obj)[1]);
        CPPUNIT_ASSERT_EQUAL(3.7, (*typed_obj)[2]);
    }

    {
        value.d = {{-1.1, -5.5, -1.7, 4.1}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_DOUBLE4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::dvec4"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::dvec4>(obj);
        CPPUNIT_ASSERT_EQUAL(-1.1, (*typed_obj)[0]);
        CPPUNIT_ASSERT_EQUAL(-5.5, (*typed_obj)[1]);
        CPPUNIT_ASSERT_EQUAL(-1.7, (*typed_obj)[2]);
        CPPUNIT_ASSERT_EQUAL(4.1, (*typed_obj)[3]);
    }

    {
        value.i = {{-1, 5, 0, 4}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_INT2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::ivec2"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::ivec2>(obj);
        CPPUNIT_ASSERT_EQUAL(data::ivec2::value_t::value_type(-1), (*typed_obj)[0]);
        CPPUNIT_ASSERT_EQUAL(data::ivec2::value_t::value_type(5), (*typed_obj)[1]);
    }
    {
        value.i = {{2, -4, 3, 4}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_INT3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::ivec3"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::ivec3>(obj);
        CPPUNIT_ASSERT_EQUAL(data::ivec3::value_t::value_type(2), (*typed_obj)[0]);
        CPPUNIT_ASSERT_EQUAL(data::ivec3::value_t::value_type(-4), (*typed_obj)[1]);
        CPPUNIT_ASSERT_EQUAL(data::ivec3::value_t::value_type(3), (*typed_obj)[2]);
    }
    {
        value.i = {{-1, 5, 9, 1}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_INT4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::ivec4"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::ivec4>(obj);
        CPPUNIT_ASSERT_EQUAL(data::ivec4::value_t::value_type(-1), (*typed_obj)[0]);
        CPPUNIT_ASSERT_EQUAL(data::ivec4::value_t::value_type(5), (*typed_obj)[1]);
        CPPUNIT_ASSERT_EQUAL(data::ivec4::value_t::value_type(9), (*typed_obj)[2]);
        CPPUNIT_ASSERT_EQUAL(data::ivec4::value_t::value_type(1), (*typed_obj)[3]);
    }
    static const double s_EPSILON = 1e-5;
    {
        value.f = {{21.1F, -2.5F, 9.F, 1.F}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_FLOAT2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::dvec2"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::dvec2>(obj);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(21.1, (*typed_obj)[0], s_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-2.5, (*typed_obj)[1], s_EPSILON);
    }
    {
        value.f = {{21.1F, 2.5F, -9.F, 1.F}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_FLOAT3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::dvec3"), obj->get_classname());

        auto typed_obj = std::dynamic_pointer_cast<data::dvec3>(obj);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(21.1, (*typed_obj)[0], s_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2.5, (*typed_obj)[1], s_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-9., (*typed_obj)[2], s_EPSILON);
    }
    {
        value.f = {{0.12F, .5F, 1.F, 8.F}};
        auto obj = shading::create_object_from_shader_parameter(Ogre::GpuConstantType::GCT_FLOAT4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::color"), obj->get_classname());

        data::color::sptr color = std::dynamic_pointer_cast<data::color>(obj);
        CPPUNIT_ASSERT_EQUAL(.12F, color->red());
        CPPUNIT_ASSERT_EQUAL(.5F, color->green());
        CPPUNIT_ASSERT_EQUAL(1.F, color->blue());
        CPPUNIT_ASSERT_EQUAL(8.F, color->alpha());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
