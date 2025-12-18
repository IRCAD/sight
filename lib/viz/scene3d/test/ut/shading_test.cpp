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

#include <doctest/doctest.h>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

TEST_SUITE("sight::viz::scene3d::helper::shading")
{
    TEST_CASE("get_permutation")
    {
        std::string suffix;

        suffix = sight::viz::scene3d::helper::shading::get_permutation(
            sight::data::material::shading_t::ambient,
            false,
            true
        );
        CHECK_EQ(std::string("Ambient+VT"), suffix);

        suffix = sight::viz::scene3d::helper::shading::get_permutation(
            sight::data::material::shading_t::flat,
            false,
            false
        );
        CHECK_EQ(std::string("Flat"), suffix);

        suffix = sight::viz::scene3d::helper::shading::get_permutation(
            sight::data::material::shading_t::flat,
            false,
            true
        );
        CHECK_EQ(std::string("Flat+VT"), suffix);

        suffix = sight::viz::scene3d::helper::shading::get_permutation(
            sight::data::material::shading_t::phong,
            true,
            true
        );
        CHECK_EQ(std::string("PixelLit+VT+DfsTex"), suffix);

        suffix = sight::viz::scene3d::helper::shading::get_permutation(
            sight::data::material::shading_t::phong,
            false,
            true
        );
        CHECK_EQ(std::string("PixelLit+VT"), suffix);
    }

    TEST_CASE("get_r2vb_geometry_program_name")
    {
        std::string prg_name;

        prg_name =
            sight::viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
                sight::data::mesh::cell_type_t::triangle,
                true,
                true,
                true
            );
        CHECK_EQ(std::string("R2VB/Triangles+VT+DfsTex+PPColor_GP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
                sight::data::mesh::cell_type_t::triangle,
                false,
                true,
                false
            );
        CHECK_EQ(std::string("R2VB/Triangles+VT_GP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
                sight::data::mesh::cell_type_t::tetra,
                false,
                true,
                true
            );
        CHECK_EQ(std::string("R2VB/Tetra+VT+PPColor_GP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
                sight::data::mesh::cell_type_t::tetra,
                true,
                false,
                true
            );
        CHECK_EQ(std::string("R2VB/Tetra+DfsTex+PPColor_GP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
                sight::data::mesh::cell_type_t::quad,
                false,
                true,
                true
            );
        CHECK_EQ(std::string("R2VB/Quad+VT+PPColor_GP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
                sight::data::mesh::cell_type_t::quad,
                false,
                false,
                true
            );
        CHECK_EQ(std::string("R2VB/Quad+PPColor_GP"), prg_name);
    }

    TEST_CASE("set_permutation_in_program_name")
    {
        std::string prg_name;

        prg_name =
            sight::viz::scene3d::helper::shading::set_permutation_in_program_name(
                "Default/PixelLit+VT+DfsTex_VP",
                "PixelLit+DfsTex"
            );
        CHECK_EQ(std::string("Default/PixelLit+DfsTex_VP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::set_permutation_in_program_name(
                "CelShadingDepthPeeling/peel_PixelLit+VT+DfsTex_FP",
                "Flat+VT"
            );
        CHECK_EQ(std::string("CelShadingDepthPeeling/peel_Flat+VT_FP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::set_permutation_in_program_name(
                "HybridTransparency/peel_Ambient+VT_FP",
                "Flat"
            );
        CHECK_EQ(std::string("HybridTransparency/peel_Flat_FP"), prg_name);
    }

    TEST_CASE("set_technique_in_program_name")
    {
        std::string prg_name;

        prg_name = sight::viz::scene3d::helper::shading::set_technique_in_program_name(
            "Default/PixelLit_FP",
            "DepthPeeling/peel"
        );
        CHECK_EQ(std::string("DepthPeeling/peel/PixelLit_FP"), prg_name);

        prg_name =
            sight::viz::scene3d::helper::shading::set_technique_in_program_name(
                "DepthPeeling/peel_init/PixelLit_FP",
                "Default"
            );
        CHECK_EQ(std::string("Default/PixelLit_FP"), prg_name);

        prg_name = sight::viz::scene3d::helper::shading::set_technique_in_program_name(
            "Default/Flat+VT_FP",
            "HybridTransparency/peel"
        );
        CHECK_EQ(std::string("HybridTransparency/peel/Flat+VT_FP"), prg_name);
    }

    TEST_CASE("create_object_from_shader_parameter")
    {
        sight::viz::scene3d::helper::shading::constant_value_t value {};

        // Scalar types
        {
            value.d = {{2.0, 0., 0., 0.}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_DOUBLE1,
                    value
                );
            CHECK_EQ(std::string("sight::data::real"), obj->get_classname());
            CHECK_EQ(2.0, std::dynamic_pointer_cast<sight::data::real>(obj)->get_value());
        }

        {
            value.f = {{2.5F, 0.F, 0.F, 0.F}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_FLOAT1,
                    value
                );
            CHECK_EQ(std::string("sight::data::real"), obj->get_classname());
            CHECK_EQ(2.5, std::dynamic_pointer_cast<sight::data::real>(obj)->get_value());
        }

        {
            value.i = {{321, 0, 0, 0}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_INT1,
                    value
                );
            CHECK_EQ(std::string("sight::data::integer"), obj->get_classname());
            CHECK_EQ(
                static_cast<std::int64_t>(321),
                std::dynamic_pointer_cast<sight::data::integer>(obj)->get_value()
            );
        }

        {
            value.d = {{2.0, 4.5, 0., 0.}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_DOUBLE2,
                    value
                );
            CHECK_EQ(std::string("sight::data::dvec2"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::dvec2>(obj);
            CHECK_EQ(2.0, (*typed_obj)[0]);
            CHECK_EQ(4.5, (*typed_obj)[1]);
        }

        {
            value.d = {{-4.1, 1.5, 3.7, 0.}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_DOUBLE3,
                    value
                );
            CHECK_EQ(std::string("sight::data::dvec3"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::dvec3>(obj);
            CHECK_EQ(-4.1, (*typed_obj)[0]);
            CHECK_EQ(1.5, (*typed_obj)[1]);
            CHECK_EQ(3.7, (*typed_obj)[2]);
        }

        {
            value.d = {{-1.1, -5.5, -1.7, 4.1}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_DOUBLE4,
                    value
                );
            CHECK_EQ(std::string("sight::data::dvec4"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::dvec4>(obj);
            CHECK_EQ(-1.1, (*typed_obj)[0]);
            CHECK_EQ(-5.5, (*typed_obj)[1]);
            CHECK_EQ(-1.7, (*typed_obj)[2]);
            CHECK_EQ(4.1, (*typed_obj)[3]);
        }

        {
            value.i = {{-1, 5, 0, 4}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_INT2,
                    value
                );
            CHECK_EQ(std::string("sight::data::ivec2"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::ivec2>(obj);
            CHECK_EQ(sight::data::ivec2::value_t::value_type(-1), (*typed_obj)[0]);
            CHECK_EQ(sight::data::ivec2::value_t::value_type(5), (*typed_obj)[1]);
        }

        {
            value.i = {{2, -4, 3, 4}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_INT3,
                    value
                );
            CHECK_EQ(std::string("sight::data::ivec3"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::ivec3>(obj);
            CHECK_EQ(sight::data::ivec3::value_t::value_type(2), (*typed_obj)[0]);
            CHECK_EQ(sight::data::ivec3::value_t::value_type(-4), (*typed_obj)[1]);
            CHECK_EQ(sight::data::ivec3::value_t::value_type(3), (*typed_obj)[2]);
        }

        {
            value.i = {{-1, 5, 9, 1}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_INT4,
                    value
                );
            CHECK_EQ(std::string("sight::data::ivec4"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::ivec4>(obj);
            CHECK_EQ(sight::data::ivec4::value_t::value_type(-1), (*typed_obj)[0]);
            CHECK_EQ(sight::data::ivec4::value_t::value_type(5), (*typed_obj)[1]);
            CHECK_EQ(sight::data::ivec4::value_t::value_type(9), (*typed_obj)[2]);
            CHECK_EQ(sight::data::ivec4::value_t::value_type(1), (*typed_obj)[3]);
        }

        static const double s_EPSILON = 1e-5;

        {
            value.f = {{21.1F, -2.5F, 9.F, 1.F}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_FLOAT2,
                    value
                );
            CHECK_EQ(std::string("sight::data::dvec2"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::dvec2>(obj);
            CHECK(doctest::Approx((*typed_obj)[0]).epsilon(s_EPSILON) == 21.1);
            CHECK(doctest::Approx((*typed_obj)[1]).epsilon(s_EPSILON) == -2.5);
        }

        {
            value.f = {{21.1F, 2.5F, -9.F, 1.F}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_FLOAT3,
                    value
                );
            CHECK_EQ(std::string("sight::data::dvec3"), obj->get_classname());
            auto typed_obj = std::dynamic_pointer_cast<sight::data::dvec3>(obj);
            CHECK(doctest::Approx((*typed_obj)[0]).epsilon(s_EPSILON) == 21.1);
            CHECK(doctest::Approx((*typed_obj)[1]).epsilon(s_EPSILON) == 2.5);
            CHECK(doctest::Approx((*typed_obj)[2]).epsilon(s_EPSILON) == -9.);
        }

        {
            value.f = {{0.12F, .5F, 1.F, 8.F}};
            auto obj =
                sight::viz::scene3d::helper::shading::create_object_from_shader_parameter(
                    Ogre::GpuConstantType::GCT_FLOAT4,
                    value
                );
            CHECK_EQ(std::string("sight::data::color"), obj->get_classname());
            sight::data::color::sptr color = std::dynamic_pointer_cast<sight::data::color>(obj);
            CHECK_EQ(.12F, color->red());
            CHECK_EQ(.5F, color->green());
            CHECK_EQ(1.F, color->blue());
            CHECK_EQ(8.F, color->alpha());
        }
    }
}
