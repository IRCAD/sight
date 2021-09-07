/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "viz/scene3d/vr/IllumAmbientOcclusionSAT.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

//-----------------------------------------------------------------------------

namespace sight::viz::scene3d
{

namespace vr
{

//-----------------------------------------------------------------------------

class VolIllumCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    VolIllumCompositorListener(
        int& currentSliceIndex,
        int nbShells,
        int shellRadius,
        float coneAngle,
        int samplesAlongCone
    ) :
        m_currentSliceIndex(currentSliceIndex),
        m_nbShells(nbShells),
        m_shellRadius(shellRadius),
        m_coneAngle(coneAngle),
        m_samplesAlongCone(samplesAlongCone)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32 /*pass_id*/, ::Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass* pass                                   = mat->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_sliceIndex", m_currentSliceIndex);
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32 /*pass_id*/, ::Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass* pass                                   = mat->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_nbShells", m_nbShells);
        volIllumParams->setNamedConstant("u_shellRadius", m_shellRadius);

        volIllumParams->setNamedConstant("u_scatteringConeAngle", m_coneAngle);
        volIllumParams->setNamedConstant("u_nbSamplesAlongCone", m_samplesAlongCone);
    }

private:

    int& m_currentSliceIndex;

    int m_nbShells;
    int m_shellRadius;

    float m_coneAngle;
    int m_samplesAlongCone;
};

//-----------------------------------------------------------------------------

IllumAmbientOcclusionSAT::IllumAmbientOcclusionSAT(
    std::string parentId,
    ::Ogre::SceneManager* sceneManager,
    float satSizeRatio,
    bool ao,
    bool shadows,
    int nbShells,
    int shellRadius,
    float coneAngle,
    int samplesAlongCone
) :
    m_sat(parentId, sceneManager, satSizeRatio),
    m_ao(ao),
    m_shadows(shadows),
    m_nbShells(nbShells),
    m_shellRadius(shellRadius),
    m_coneAngle(coneAngle),
    m_samplesAlongCone(samplesAlongCone),
    m_parentId(parentId),
    m_dummyCamera(nullptr),
    m_sceneManager(sceneManager)
{
}

//-----------------------------------------------------------------------------

IllumAmbientOcclusionSAT::~IllumAmbientOcclusionSAT()
{
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::updateSatFromRatio(float _satSizeRatio)
{
    m_sat.updateSatFromRatio(_satSizeRatio);
    updateTexture();
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::SATUpdate(
    ::Ogre::TexturePtr _img,
    const viz::scene3d::TransferFunction::sptr& _tf,
    float _sampleDistance
)
{
    m_sat.computeParallel(_img, _tf, _sampleDistance);
    this->updateVolIllum();
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::updateVolIllum()
{
    // Do this for now but at the end we should use our own texture
    m_illuminationVolume = m_sat.getSpareTexture();

    const int depth = static_cast<int>(m_illuminationVolume->getDepth());

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    VolIllumCompositorListener volIllumListener(m_currentSliceIndex, m_nbShells, m_shellRadius,
                                                m_coneAngle, m_samplesAlongCone);

    std::string currentMaterialName("VolIllum");

    currentMaterialName += m_ao ? "_AO" : "";
    currentMaterialName += m_shadows ? "_Shadows" : "";

    ::Ogre::MaterialPtr volIllumMtl = ::Ogre::MaterialManager::getSingleton().getByName(
        currentMaterialName,
        RESOURCE_GROUP
    );
    ::Ogre::Pass* pass                    = volIllumMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState* satImgState = pass->getTextureUnitState("sat");

    satImgState->setTexture(m_sat.getTexture());

    // Update illumination volume slice by slice.
    for(m_currentSliceIndex = 0 ; m_currentSliceIndex < depth ; ++m_currentSliceIndex)
    {
        ::Ogre::RenderTarget* rt =
            m_illuminationVolume->getBuffer()->getRenderTarget(static_cast<size_t>(m_currentSliceIndex));
        ::Ogre::Viewport* vp = rt->getViewport(0);

        // Add compositor.
        compositorManager.addCompositor(vp, "VolumeIllumination");
        compositorManager.setCompositorEnabled(vp, "VolumeIllumination", true);

        ::Ogre::CompositorInstance* compInstance = compositorManager.getCompositorChain(vp)->getCompositor(
            "VolumeIllumination"
        );

        const auto& passes                = compInstance->getTechnique()->getOutputTargetPass()->getPasses();
        ::Ogre::CompositionPass* compPass = passes[0];

        if(compPass->getMaterial()->getName() != currentMaterialName)
        {
            compPass->setMaterialName(currentMaterialName);
        }

        compInstance->addListener(&volIllumListener);

        // Compute volillum.
        rt->update(false);

        // Remove compositor.
        compInstance->removeListener(&volIllumListener);
        compositorManager.setCompositorEnabled(vp, "VolumeIllumination", false);
        compositorManager.removeCompositor(vp, "VolumeIllumination");
    }
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::updateTexture()
{
    ::Ogre::TexturePtr satTexture = m_sat.getSpareTexture();

    ::Ogre::TextureManager& textureManager = ::Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    textureManager.remove(m_parentId + BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);

    m_illuminationVolume = textureManager.createManual(
        m_parentId + BUFFER_NAME,
        viz::scene3d::RESOURCE_GROUP,
        ::Ogre::TEX_TYPE_3D,
        satTexture->getWidth(),
        satTexture->getHeight(),
        satTexture->getDepth(),
        0,
        ::Ogre::PF_A8R8G8B8,
        ::Ogre::TU_RENDERTARGET
    );

    if(!m_dummyCamera)
    {
        m_dummyCamera = m_sceneManager->createCamera(m_parentId + "_VolumeIllumination_DummyCamera");
    }

    const int depth = static_cast<int>(satTexture->getDepth());
    for(int sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        // Init source buffer
        ::Ogre::RenderTarget* renderTarget =
            m_illuminationVolume->getBuffer()->getRenderTarget(static_cast<size_t>(sliceIndex));
        ::Ogre::Viewport* vp = renderTarget->addViewport(m_dummyCamera);

        vp->setOverlaysEnabled(false);
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
