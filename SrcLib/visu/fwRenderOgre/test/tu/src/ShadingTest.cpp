/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ShadingTest.hpp"

#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/Utils.hpp"

#include "fwTools/Type.hpp"

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::helper::ut::ShadingTest );

namespace fwRenderOgre
{
namespace helper
{
namespace ut
{

//------------------------------------------------------------------------------

void ShadingTest::setUp()
{
    m_ogreRoot = Utils::getOgreRoot();

    auto& mgr = ::Ogre::MaterialManager::getSingleton();
    m_material = mgr.create("TestMaterial", ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    m_material->createTechnique();
}

//------------------------------------------------------------------------------

void ShadingTest::tearDown()
{
    m_ogreRoot = nullptr;
    Utils::destroyOgreRoot();
}

//------------------------------------------------------------------------------

void ShadingTest::isColorTechnique()
{
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

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
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

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
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

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
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

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
}

//------------------------------------------------------------------------------

void ShadingTest::getPermutation()
{
    std::string suffix;

    suffix = Shading::getPermutation(::fwData::Material::AMBIENT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Ambient+VT"), suffix);

    suffix = Shading::getPermutation(::fwData::Material::FLAT, false, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat"), suffix);

    suffix = Shading::getPermutation(::fwData::Material::FLAT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat+VT"), suffix);

    suffix = Shading::getPermutation(::fwData::Material::GOURAUD, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Gouraud+DfsTex"), suffix);

    suffix = Shading::getPermutation(::fwData::Material::GOURAUD, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Gouraud+VT+DfsTex"), suffix);

    suffix = Shading::getPermutation(::fwData::Material::PHONG, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT+DfsTex"), suffix);

    suffix = Shading::getPermutation(::fwData::Material::PHONG, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT"), suffix);

}

//------------------------------------------------------------------------------

void ShadingTest::getR2VBGeometryProgramName()
{
    std::string prgName;

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TRIANGLE, true, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT+DfsTex+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TRIANGLE, false, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TETRA, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+VT+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TETRA, true, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+DfsTex+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::QUAD, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+VT+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::QUAD, false, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+PPColor_GP"), prgName);
}

//------------------------------------------------------------------------------

void ShadingTest::setPermutationInProgramName()
{
    std::string prgName;

    prgName = Shading::setPermutationInProgramName("Default/PixelLit+VT+DfsTex_VP", "Gouraud+DfsTex");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/Gouraud+DfsTex_VP"), prgName);

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
    ::fwData::Object::sptr obj;

    // Scalar types
    {
        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_DOUBLE1);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Float"), obj->className());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_FLOAT1);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Float"), obj->className());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_INT1);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Integer"), obj->className());
    }

    // Array types
    {
        ::fwData::Array::sptr arrayObject;
        ::fwData::Array::SizeType size;

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_DOUBLE2);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_FLOAT, arrayObject->getType());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_DOUBLE3);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_FLOAT, arrayObject->getType());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_DOUBLE4);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_FLOAT, arrayObject->getType());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_INT2);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_INT32, arrayObject->getType());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_INT3);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_INT32, arrayObject->getType());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_INT4);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_INT32, arrayObject->getType());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_FLOAT2);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Array"), obj->className());

        arrayObject = ::fwData::Array::dynamicCast(obj);
        size        = arrayObject->getSize();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), size[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), arrayObject->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_FLOAT, arrayObject->getType());
    }

    // Other
    {
        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_FLOAT3);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Point"), obj->className());

        obj = Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType::GCT_FLOAT4);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Color"), obj->className());
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre
