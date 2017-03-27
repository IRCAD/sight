/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"

#include "fwRenderOgre/helper/Shading.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace fwRenderOgre
{
namespace compositor
{
namespace listener
{

//-----------------------------------------------------------------------------

AutoStereoCompositorListener::AutoStereoCompositorListener(unsigned int _numViewPoints, Ogre::Camera& _camera) :
    m_numViewPoints(_numViewPoints),
    m_camera(_camera),
    m_renderTargets(nullptr)
{
}

//-----------------------------------------------------------------------------

AutoStereoCompositorListener::AutoStereoCompositorListener(unsigned int _numViewPoints, Ogre::Camera& _camera,
                                                           std::vector<Ogre::TexturePtr>* renderTargets) :
    m_numViewPoints(_numViewPoints),
    m_camera(_camera),
    m_renderTargets(renderTargets)
{
}

//------------------------------------------------------------------------------

::Ogre::Technique* AutoStereoCompositorListener::handleSchemeNotFound(unsigned short _schemeIndex,
                                                                      const ::Ogre::String& _schemeName,
                                                                      ::Ogre::Material* _originalMaterial,
                                                                      unsigned short _lodIndex,
                                                                      const ::Ogre::Renderable* _renderable)
{
    ::Ogre::Technique* newTech = nullptr;
    if(_schemeName.find("AutoStereo") != std::string::npos)
    {
        SLM_ASSERT("Only handle 5 or 8 viewpoints", m_numViewPoints == 5 || m_numViewPoints == 8);

        const bool isRayTracedMtl = ::Ogre::StringUtil::startsWith( _originalMaterial->getName(), "RayTracedVolume");
        if(  isRayTracedMtl && m_renderTargets == nullptr )
        {
            return nullptr;
        }
        if( !isRayTracedMtl && m_renderTargets != nullptr )
        {
            return nullptr;
        }

        const int passId = std::stoi(_schemeName.substr(11));

        ::Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
        newTech                        = this->copyTechnique(defaultTech, _schemeName, _originalMaterial);

        const auto pass           = newTech->getPass(0);
        const auto baseName       = pass->getVertexProgramName();
        const auto sourceFileName = pass->getVertexProgram()->getSourceFile();
        const auto newName        = baseName + "+AutoStereo";

        ::fwRenderOgre::helper::Shading::GpuProgramParametersType parameters;
        parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "AUTOSTEREO=1"));

        ::fwRenderOgre::helper::Shading::createProgramFrom(newName, sourceFileName, parameters,
                                                           ::Ogre::GPT_VERTEX_PROGRAM, baseName);

        pass->setVertexProgram(newName);

        auto vertexParams = pass->getVertexProgramParameters();

        float eyeAngle = 0.f;
        float angle    = 0.f;
        if(m_numViewPoints == 5)
        {
            eyeAngle = 0.02321f;
            angle    = eyeAngle * -2.f;
        }
        else if(m_numViewPoints == 8)
        {
            eyeAngle = 0.01625f;
            angle    = eyeAngle * -3.5f;
        }

        angle += eyeAngle * passId;
        const ::Ogre::Matrix4 shearTransform = this->frustumShearTransform(angle);

        ::Ogre::Matrix4 projMat = m_camera.getProjectionMatrixWithRSDepth();
        projMat = projMat * shearTransform;

        projMat[1][0] = -projMat[1][0];
        projMat[1][1] = -projMat[1][1];
        projMat[1][2] = -projMat[1][2];
        projMat[1][3] = -projMat[1][3];

//        // First check that we did not already instanced Shared parameters
//        // This can happen when reinstancing this class (e.g. switching 3D mode)
//        try
//        {
//            m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().getSharedParameters("RTVParams");
//        }
//        catch(::Ogre::InvalidParametersException e)
//        {
//            m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters("RTVParams");

//            // define the shared param structure
//            m_RTVSharedParameters->addConstantDefinition("u_proj", ::Ogre::GCT_MATRIX_4X4);
//        }

//        vertexParams->addSharedParameters("RTVParams");

        vertexParams->setNamedConstant("u_proj", projMat);
        vertexParams->setNamedAutoConstant("u_worldView", ::Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);

        if(baseName == "RayTracedVolume_VP")
        {
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("entryPoints");

            OSLM_LOG("matrix proj " << projMat);

            OSLM_ASSERT("No texture named " << passId, texUnitState);
            texUnitState->setTextureName((*m_renderTargets)[static_cast<size_t>(passId)]->getName());
        }
    }

    return newTech;
}

//------------------------------------------------------------------------------

Ogre::Matrix4 AutoStereoCompositorListener::frustumShearTransform(float angle) const
{
    ::Ogre::Matrix4 shearTransform = ::Ogre::Matrix4::IDENTITY;

    const float focalLength = m_camera.getFocalLength();
    const float xshearFactor = std::tan(angle);

    shearTransform[0][2] = -xshearFactor;
    shearTransform[0][3] = -focalLength * xshearFactor;

    return shearTransform;
}

// ----------------------------------------------------------------------------

Ogre::Technique* AutoStereoCompositorListener::copyTechnique(::Ogre::Technique* _tech,
                                                             const ::Ogre::String& _schemeName,
                                                             ::Ogre::Material* _originalMaterial)
{
    ::Ogre::Technique* newTech = _originalMaterial->createTechnique();
    *newTech                   = *_tech;
    newTech->setName(_schemeName);
    newTech->setSchemeName(_schemeName);

    ::Ogre::Pass* pass = newTech->getPass(0);
    SLM_ASSERT("Empty pass", pass);

    return newTech;
}
//------------------------------------------------------------------------------

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
