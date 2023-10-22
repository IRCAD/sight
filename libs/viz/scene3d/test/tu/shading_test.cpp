/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "core/type.hpp"

#include "data/array.hpp"
#include "data/color.hpp"
#include "data/integer.hpp"
#include "data/real.hpp"

#include <viz/scene3d/helper/shading.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::shading_test);

namespace sight::viz::scene3d::helper::ut
{

//------------------------------------------------------------------------------

shading_test::shading_test()
= default;

//------------------------------------------------------------------------------

void shading_test::setUp()
{
    auto& mgr     = Ogre::MaterialManager::getSingleton();
    auto material = mgr.createOrRetrieve("TestMaterial", viz::scene3d::RESOURCE_GROUP);

    m_material = Ogre::dynamic_pointer_cast<Ogre::Material>(material.first);
    m_material->createTechnique();
}

//------------------------------------------------------------------------------

void shading_test::tearDown()
{
}

//------------------------------------------------------------------------------

void shading_test::isColorTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("technique is not set", tech);

    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isColorTechnique(*tech));

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, shading::isColorTechnique(*tech));
}

//------------------------------------------------------------------------------

void shading_test::isPeelTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isPeelTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isPeelTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isPeelTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isPeelTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isPeelTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isPeelTechnique(*tech));
}

//------------------------------------------------------------------------------

void shading_test::isGeometricTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isGeometricTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isGeometricTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, shading::isGeometricTechnique(*tech));
}

//------------------------------------------------------------------------------

void shading_test::isDepthOnlyTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("technique is not set", tech);
    //cspell: disable
    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("depth");
    CPPUNIT_ASSERT_EQUAL(true, shading::isDepthOnlyTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(true, shading::isDepthOnlyTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(true, shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(true, shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("Celfalse, ShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, shading::isDepthOnlyTechnique(*tech));
    //cspell: enable
}

//------------------------------------------------------------------------------

void shading_test::getPermutation()
{
    std::string suffix;

    suffix = shading::getPermutation(data::material::AMBIENT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Ambient+VT"), suffix);

    suffix = shading::getPermutation(data::material::FLAT, false, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat"), suffix);

    suffix = shading::getPermutation(data::material::FLAT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat+VT"), suffix);

    suffix = shading::getPermutation(data::material::PHONG, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT+DfsTex"), suffix);

    suffix = shading::getPermutation(data::material::PHONG, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT"), suffix);
}

//------------------------------------------------------------------------------

void shading_test::getR2VBGeometryProgramName()
{
    std::string prg_name;

    prg_name = shading::getR2VBGeometryProgramName(data::mesh::cell_type_t::TRIANGLE, true, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT+DfsTex+PPColor_GP"), prg_name);

    prg_name = shading::getR2VBGeometryProgramName(data::mesh::cell_type_t::TRIANGLE, false, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT_GP"), prg_name);

    prg_name = shading::getR2VBGeometryProgramName(data::mesh::cell_type_t::TETRA, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+VT+PPColor_GP"), prg_name);

    prg_name = shading::getR2VBGeometryProgramName(data::mesh::cell_type_t::TETRA, true, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+DfsTex+PPColor_GP"), prg_name);

    prg_name = shading::getR2VBGeometryProgramName(data::mesh::cell_type_t::QUAD, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+VT+PPColor_GP"), prg_name);

    prg_name = shading::getR2VBGeometryProgramName(data::mesh::cell_type_t::QUAD, false, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+PPColor_GP"), prg_name);
}

//------------------------------------------------------------------------------

void shading_test::setPermutationInProgramName()
{
    std::string prg_name;

    prg_name = shading::setPermutationInProgramName("Default/PixelLit+VT+DfsTex_VP", "PixelLit+DfsTex");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit+DfsTex_VP"), prg_name);

    prg_name = shading::setPermutationInProgramName("CelShadingDepthPeeling/peel_PixelLit+VT+DfsTex_FP", "Flat+VT");
    CPPUNIT_ASSERT_EQUAL(std::string("CelShadingDepthPeeling/peel_Flat+VT_FP"), prg_name);

    prg_name = shading::setPermutationInProgramName("HybridTransparency/peel_Ambient+VT_FP", "Flat");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel_Flat_FP"), prg_name);
}

//------------------------------------------------------------------------------

void shading_test::setTechniqueInProgramName()
{
    std::string prg_name;

    prg_name = shading::setTechniqueInProgramName("Default/PixelLit_FP", "DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("DepthPeeling/peel/PixelLit_FP"), prg_name);

    prg_name = shading::setTechniqueInProgramName("DepthPeeling/PeelInit/PixelLit_FP", "Default");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit_FP"), prg_name);

    prg_name = shading::setTechniqueInProgramName("Default/Flat+VT_FP", "HybridTransparency/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel/Flat+VT_FP"), prg_name);
}

//------------------------------------------------------------------------------

void shading_test::createObjectFromShaderParameter()
{
    data::object::sptr obj;

    // Scalar types
    {
        shading::constant_value_t value {};

        value.d = {{2.0, 0., 0., 0.}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), obj->get_classname());
        CPPUNIT_ASSERT_EQUAL(2.0, std::dynamic_pointer_cast<data::real>(obj)->getValue());

        value.f = {{2.5F, 0.F, 0.F, 0.F}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), obj->get_classname());
        CPPUNIT_ASSERT_EQUAL(2.5, std::dynamic_pointer_cast<data::real>(obj)->getValue());

        value.i = {{321, 0, 0, 0}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::integer"), obj->get_classname());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(321), std::dynamic_pointer_cast<data::integer>(obj)->getValue());
    }

    // Array types
    {
        data::array::sptr array_object;
        data::array::size_t size;
        shading::constant_value_t value {};

        value.d = {{2.0, 4.5, 0., 0.}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::DOUBLE, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(2.0, array_object->at<double>({0}));
            CPPUNIT_ASSERT_EQUAL(4.5, array_object->at<double>({1}));
        }

        value.d = {{-4.1, 1.5, 3.7, 0.}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::DOUBLE, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(-4.1, array_object->at<double>({0}));
            CPPUNIT_ASSERT_EQUAL(1.5, array_object->at<double>({1}));
            CPPUNIT_ASSERT_EQUAL(3.7, array_object->at<double>({2}));
        }

        value.d = {{-1.1, -5.5, -1.7, 4.1}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::DOUBLE, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(-1.1, array_object->at<double>({0}));
            CPPUNIT_ASSERT_EQUAL(-5.5, array_object->at<double>({1}));
            CPPUNIT_ASSERT_EQUAL(-1.7, array_object->at<double>({2}));
            CPPUNIT_ASSERT_EQUAL(4.1, array_object->at<double>({3}));
        }

        value.i = {{-1, 5, 0, 4}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::INT32, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(-1, array_object->at<int>({0}));
            CPPUNIT_ASSERT_EQUAL(5, array_object->at<int>({1}));
        }

        value.i = {{2, -4, 3, 4}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::INT32, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(2, array_object->at<int>({0}));
            CPPUNIT_ASSERT_EQUAL(-4, array_object->at<int>({1}));
            CPPUNIT_ASSERT_EQUAL(3, array_object->at<int>({2}));
        }

        value.i = {{-1, 5, 9, 1}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::INT32, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(-1, array_object->at<int>({0}));
            CPPUNIT_ASSERT_EQUAL(5, array_object->at<int>({1}));
            CPPUNIT_ASSERT_EQUAL(9, array_object->at<int>({2}));
            CPPUNIT_ASSERT_EQUAL(1, array_object->at<int>({3}));
        }

        value.f = {{21.1F, -2.5F, 9.F, 1.F}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::FLOAT, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(21.1F, array_object->at<float>({0}));
            CPPUNIT_ASSERT_EQUAL(-2.5F, array_object->at<float>({1}));
        }

        value.f = {{21.1F, 2.5F, -9.F, 1.F}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::array"), obj->get_classname());

        array_object = std::dynamic_pointer_cast<data::array>(obj);
        size         = array_object->size();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::type::FLOAT, array_object->getType());
        {
            const auto dump_lock = array_object->dump_lock();
            CPPUNIT_ASSERT_EQUAL(21.1F, array_object->at<float>({0}));
            CPPUNIT_ASSERT_EQUAL(2.5F, array_object->at<float>({1}));
            CPPUNIT_ASSERT_EQUAL(-9.F, array_object->at<float>({2}));
        }

        value.f = {{0.12F, .5F, 1.F, 8.F}};
        obj     = shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT4, value);
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
