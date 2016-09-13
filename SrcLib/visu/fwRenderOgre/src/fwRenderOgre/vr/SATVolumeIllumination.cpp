/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/SATVolumeIllumination.hpp"

#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
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
        m_nbShells         (nbShells),
        m_shellRadius      (shellRadius),
        m_coneAngle        (coneAngle),
        m_samplesAlongCone (samplesAlongCone)
    {
    }

    virtual void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass* pass                                   = mat->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_sliceIndex", m_currentSliceIndex);
    }

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
                                             float satSizeRatio, int nbShells, int shellRadius,
                                             float coneAngle, int samplesAlongCone) :
    m_sat(parentId, sceneManager, satSizeRatio),
    m_nbShells(nbShells),
    m_shellRadius(shellRadius),
    m_coneAngle(coneAngle),
    m_samplesAlongCone(samplesAlongCone)
{

}

//-----------------------------------------------------------------------------

SATVolumeIllumination::~SATVolumeIllumination()
{

}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::updateSAT(float _satSizeRatio)
{
    m_sat.updateSatFromRatio(_satSizeRatio);
}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::SATUpdate(Ogre::TexturePtr _img, Ogre::TexturePtr _tf)
{
    m_sat.computeParallel(_img, _tf);

    updateVolIllum();
}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::updateVolIllum()
{
    m_illuminationVolume = m_sat.getSpareTexture();

    const int depth = m_illuminationVolume->getDepth();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    VolIllumCompositorListener volIllumListener(m_currentSliceIndex, m_nbShells, m_shellRadius,
                                                m_coneAngle, m_samplesAlongCone);

    ::Ogre::MaterialPtr volIllumMtl       = ::Ogre::MaterialManager::getSingleton().getByName("VolumeAO");
    ::Ogre::Pass* pass                    = volIllumMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState* satImgState = pass->getTextureUnitState("sat");

    satImgState->setTexture(m_sat.getTexture());

    // Update illumination volume slice by slice.
    for(m_currentSliceIndex = 0; m_currentSliceIndex < depth; ++m_currentSliceIndex)
    {
        ::Ogre::RenderTarget* rt = m_illuminationVolume->getBuffer()->getRenderTarget(m_currentSliceIndex);
        ::Ogre::Viewport* vp     = rt->getViewport(0);

        // Add compositor.
        compositorManager.addCompositor(vp, "VolIllum");
        compositorManager.setCompositorEnabled(vp, "VolIllum", true);

        ::Ogre::CompositorInstance* compInstance = compositorManager.getCompositorChain(vp)->getCompositor("VolIllum");
        compInstance->addListener(&volIllumListener);

        // Compute volillum.
        rt->update(false);

        // Remove compositor.
        compInstance->removeListener(&volIllumListener);
        compositorManager.setCompositorEnabled(vp, "VolIllum", false);
        compositorManager.removeCompositor(vp, "VolIllum");
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
