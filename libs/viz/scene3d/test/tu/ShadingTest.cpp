/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "ShadingTest.hpp"

#include "core/tools/Type.hpp"

#include "data/Array.hpp"
#include "data/Color.hpp"
#include "data/Float.hpp"
#include "data/Integer.hpp"

#include <viz/scene3d/helper/Shading.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::ShadingTest);

namespace sight::viz::scene3d
{

namespace helper
{

namespace ut
{

//------------------------------------------------------------------------------

ShadingTest::ShadingTest()
{
}

//------------------------------------------------------------------------------

void ShadingTest::setUp()
{
    auto& mgr     = Ogre::MaterialManager::getSingleton();
    auto material = mgr.createOrRetrieve("TestMaterial", viz::scene3d::RESOURCE_GROUP);

    m_material = Ogre::dynamic_pointer_cast<Ogre::Material>(material.first);
    m_material->createTechnique();
}

//------------------------------------------------------------------------------

void ShadingTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ShadingTest::isColorTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("Technique is not set", tech);

    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
}

//------------------------------------------------------------------------------

void ShadingTest::isPeelTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("Technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
}

//------------------------------------------------------------------------------

void ShadingTest::isGeometricTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("Technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
}

//------------------------------------------------------------------------------

void ShadingTest::isDepthOnlyTechnique()
{
    Ogre::Technique* tech = m_material->getTechnique(0);
    SIGHT_ASSERT("Technique is not set", tech);
    //cspell: disable
    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("depth");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("Celfalse, ShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    //cspell: enable
}

//------------------------------------------------------------------------------

void ShadingTest::getPermutation()
{
    std::string suffix;

    suffix = Shading::getPermutation(data::Material::AMBIENT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Ambient+VT"), suffix);

    suffix = Shading::getPermutation(data::Material::FLAT, false, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat"), suffix);

    suffix = Shading::getPermutation(data::Material::FLAT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat+VT"), suffix);

    suffix = Shading::getPermutation(data::Material::PHONG, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT+DfsTex"), suffix);

    suffix = Shading::getPermutation(data::Material::PHONG, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT"), suffix);
}

//------------------------------------------------------------------------------

void ShadingTest::getR2VBGeometryProgramName()
{
    std::string prgName;

    prgName = Shading::getR2VBGeometryProgramName(data::Mesh::CellType::TRIANGLE, true, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT+DfsTex+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(data::Mesh::CellType::TRIANGLE, false, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(data::Mesh::CellType::TETRA, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+VT+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(data::Mesh::CellType::TETRA, true, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+DfsTex+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(data::Mesh::CellType::QUAD, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+VT+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(data::Mesh::CellType::QUAD, false, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+PPColor_GP"), prgName);
}

//------------------------------------------------------------------------------

void ShadingTest::setPermutationInProgramName()
{
    std::string prgName;

    prgName = Shading::setPermutationInProgramName("Default/PixelLit+VT+DfsTex_VP", "PixelLit+DfsTex");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit+DfsTex_VP"), prgName);

    prgName = Shading::setPermutationInProgramName("CelShadingDepthPeeling/peel_PixelLit+VT+DfsTex_FP", "Flat+VT");
    CPPUNIT_ASSERT_EQUAL(std::string("CelShadingDepthPeeling/peel_Flat+VT_FP"), prgName);

    prgName = Shading::setPermutationInProgramName("HybridTransparency/peel_Ambient+VT_FP", "Flat");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel_Flat_FP"), prgName);
}

//------------------------------------------------------------------------------

void ShadingTest::setTechniqueInProgramName()
{
    std::string prgName;

    prgName = Shading::setTechniqueInProgramName("Default/PixelLit_FP", "DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("DepthPeeling/peel/PixelLit_FP"), prgName);

    prgName = Shading::setTechniqueInProgramName("DepthPeeling/PeelInit/PixelLit_FP", "Default");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit_FP"), prgName);

    prgName = Shading::setTechniqueInProgramName("Default/Flat+VT_FP", "HybridTransparency/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel/Flat+VT_FP"), prgName);
}

//------------------------------------------------------------------------------

void ShadingTest::createObjectFromShaderParameter()
{
    data::Object::sptr obj;

    // Scalar types
    {
        Shading::ConstantValueType value;

        value.d = {{2.0, 0., 0., 0.}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Float"), obj->getClassname());
        CPPUNIT_ASSERT_EQUAL(2.0f, data::Float::dynamicCast(obj)->getValue());

        value.f = {{2.5f, 0.f, 0.f, 0.f}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Float"), obj->getClassname());
        CPPUNIT_ASSERT_EQUAL(2.5f, data::Float::dynamicCast(obj)->getValue());

        value.i = {{321, 0, 0, 0}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT1, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Integer"), obj->getClassname());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(321), data::Integer::dynamicCast(obj)->getValue());
    }

    // Array types
    {
        data::Array::sptr arrayObject;
        data::Array::SizeType size;
        Shading::ConstantValueType value;

        value.d = {{2.0, 4.5, 0., 0.}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_DOUBLE, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(2.0, arrayObject->at<double>({0}));
            CPPUNIT_ASSERT_EQUAL(4.5, arrayObject->at<double>({1}));
        }

        value.d = {{-4.1, 1.5, 3.7, 0.}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_DOUBLE, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(-4.1, arrayObject->at<double>({0}));
            CPPUNIT_ASSERT_EQUAL(1.5, arrayObject->at<double>({1}));
            CPPUNIT_ASSERT_EQUAL(3.7, arrayObject->at<double>({2}));
        }

        value.d = {{-1.1, -5.5, -1.7, 4.1}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_DOUBLE4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_DOUBLE, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(-1.1, arrayObject->at<double>({0}));
            CPPUNIT_ASSERT_EQUAL(-5.5, arrayObject->at<double>({1}));
            CPPUNIT_ASSERT_EQUAL(-1.7, arrayObject->at<double>({2}));
            CPPUNIT_ASSERT_EQUAL(4.1, arrayObject->at<double>({3}));
        }

        value.i = {{-1, 5, 0, 4}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_INT32, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(-1, arrayObject->at<int>({0}));
            CPPUNIT_ASSERT_EQUAL(5, arrayObject->at<int>({1}));
        }

        value.i = {{2, -4, 3, 4}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_INT32, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(2, arrayObject->at<int>({0}));
            CPPUNIT_ASSERT_EQUAL(-4, arrayObject->at<int>({1}));
            CPPUNIT_ASSERT_EQUAL(3, arrayObject->at<int>({2}));
        }

        value.i = {{-1, 5, 9, 1}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_INT4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_INT32, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(-1, arrayObject->at<int>({0}));
            CPPUNIT_ASSERT_EQUAL(5, arrayObject->at<int>({1}));
            CPPUNIT_ASSERT_EQUAL(9, arrayObject->at<int>({2}));
            CPPUNIT_ASSERT_EQUAL(1, arrayObject->at<int>({3}));
        }

        value.f = {{21.1f, -2.5f, 9.f, 1.f}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT2, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_FLOAT, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(21.1f, arrayObject->at<float>({0}));
            CPPUNIT_ASSERT_EQUAL(-2.5f, arrayObject->at<float>({1}));
        }

        value.f = {{21.1f, 2.5f, -9.f, 1.f}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT3, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Array"), obj->getClassname());

        arrayObject = data::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), size[0]);
        CPPUNIT_ASSERT_EQUAL(core::tools::Type::s_FLOAT, arrayObject->getType());
        {
            const auto dumpLock = arrayObject->lock();
            CPPUNIT_ASSERT_EQUAL(21.1f, arrayObject->at<float>({0}));
            CPPUNIT_ASSERT_EQUAL(2.5f, arrayObject->at<float>({1}));
            CPPUNIT_ASSERT_EQUAL(-9.f, arrayObject->at<float>({2}));
        }

        value.f = {{0.12f, .5f, 1.f, 8.f}};
        obj     = Shading::createObjectFromShaderParameter(Ogre::GpuConstantType::GCT_FLOAT4, value);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::Color"), obj->getClassname());

        data::Color::sptr color = data::Color::dynamicCast(obj);
        CPPUNIT_ASSERT_EQUAL(.12f, color->red());
        CPPUNIT_ASSERT_EQUAL(.5f, color->green());
        CPPUNIT_ASSERT_EQUAL(1.f, color->blue());
        CPPUNIT_ASSERT_EQUAL(8.f, color->alpha());
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace helper

} //namespace sight::viz::scene3d
