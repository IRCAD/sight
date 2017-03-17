/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/SATVolumeIllumination.hpp"

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

namespace fwRenderOgre
{

namespace vr
{

//-----------------------------------------------------------------------------

class VolIllumCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    VolIllumCompositorListener(int& currentSliceIndex, int nbShells, int shellRadius,
                               float coneAngle, int samplesAlongCone) :
        m_currentSliceIndex(currentSliceIndex),
        m_nbShells(nbShells),
        m_shellRadius(shellRadius),
        m_coneAngle(coneAngle),
        m_samplesAlongCone(samplesAlongCone)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass* pass                                   = mat->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_sliceIndex", m_currentSliceIndex);
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
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

SATVolumeIllumination::SATVolumeIllumination(std::string parentId, ::Ogre::SceneManager* sceneManager,
                                             float satSizeRatio, bool ao, bool shadows, int nbShells, int shellRadius,
                                             float coneAngle, int samplesAlongCone) :
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

SATVolumeIllumination::~SATVolumeIllumination()
{
    m_illuminationVolume = m_sat.getSpareTexture();

}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::updateSatFromRatio(float _satSizeRatio)
{
    m_sat.updateSatFromRatio(_satSizeRatio);
    updateTexture();
}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::SATUpdate(::Ogre::TexturePtr _img, Ogre::TexturePtr _tf)
{
    m_sat.computeParallel(_img, _tf);
    this->updateVolIllum();
}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::updateVolIllum()
{
    // Do this for now but at the end we should use our own texture
    m_illuminationVolume = m_sat.getSpareTexture();

    const int depth = m_illuminationVolume->getDepth();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    VolIllumCompositorListener volIllumListener(m_currentSliceIndex, m_nbShells, m_shellRadius,
                                                m_coneAngle, m_samplesAlongCone);

    std::string currentMaterialName("VolIllum");

    currentMaterialName += m_ao ? "_AO" : "";
    currentMaterialName += m_shadows ? "_Shadows" : "";

    ::Ogre::MaterialPtr volIllumMtl       = ::Ogre::MaterialManager::getSingleton().getByName(currentMaterialName);
    ::Ogre::Pass* pass                    = volIllumMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState* satImgState = pass->getTextureUnitState("sat");

    satImgState->setTexture(m_sat.getTexture());

    // Update illumination volume slice by slice.
    for(m_currentSliceIndex = 0; m_currentSliceIndex < depth; ++m_currentSliceIndex)
    {
        ::Ogre::RenderTarget* rt = m_illuminationVolume->getBuffer()->getRenderTarget(m_currentSliceIndex);
        ::Ogre::Viewport* vp     = rt->getViewport(0);

        // Add compositor.
        compositorManager.addCompositor(vp, "VolumeIllumination");
        compositorManager.setCompositorEnabled(vp, "VolumeIllumination", true);

        ::Ogre::CompositorInstance* compInstance = compositorManager.getCompositorChain(vp)->getCompositor(
            "VolumeIllumination");

        ::Ogre::CompositionPass* pass = compInstance->getTechnique()->getOutputTargetPass()->getPass(0);

        if(pass->getMaterial()->getName() != currentMaterialName)
        {
            pass->setMaterialName(currentMaterialName);
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

void SATVolumeIllumination::updateTexture()
{
    ::Ogre::TexturePtr satTexture = m_sat.getSpareTexture();

    ::Ogre::TextureManager& textureManager = ::Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    textureManager.remove(m_parentId + BUFFER_NAME);

    m_illuminationVolume = textureManager.createManual(
        m_parentId + BUFFER_NAME,
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        ::Ogre::TEX_TYPE_3D,
        satTexture->getWidth(),
        satTexture->getHeight(),
        satTexture->getDepth(),
        0,
        ::Ogre::PF_A8R8G8B8,
        ::Ogre::TU_RENDERTARGET );

    if(!m_dummyCamera)
    {
        m_dummyCamera = m_sceneManager->createCamera(m_parentId + "_VolumeIllumination_DummyCamera");
    }

    const int depth = satTexture->getDepth();
    for(int sliceIndex = 0; sliceIndex < depth; ++sliceIndex)
    {
        // Init source buffer
        ::Ogre::RenderTarget* renderTarget = m_illuminationVolume->getBuffer()->getRenderTarget(sliceIndex);
        ::Ogre::Viewport* vp               = renderTarget->addViewport(m_dummyCamera);

        vp->setOverlaysEnabled(false);
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
