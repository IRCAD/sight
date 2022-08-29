/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

///@brief Internal listener used to update the material according to the parameters.
class VolIllumCompositorListener : public Ogre::CompositorInstance::Listener
{
public:

    VolIllumCompositorListener(
        const int& currentSliceIndex,
        const IllumAmbientOcclusionSAT::sat_parameters_t& parameters
    ) :
        m_currentSliceIndex(currentSliceIndex),
        m_parameters(parameters)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat)
    {
        Ogre::Pass* pass                                   = mat->getTechnique(0)->getPass(0);
        Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_sliceIndex", m_currentSliceIndex);
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat)
    {
        Ogre::Pass* pass                                   = mat->getTechnique(0)->getPass(0);
        Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_nbShells", m_parameters.shells);
        volIllumParams->setNamedConstant("u_shellRadius", m_parameters.radius);

        volIllumParams->setNamedConstant("u_scatteringConeAngle", m_parameters.angle);
        volIllumParams->setNamedConstant("u_nbSamplesAlongCone", m_parameters.samples);
    }

private:

    const int& m_currentSliceIndex;

    const IllumAmbientOcclusionSAT::sat_parameters_t& m_parameters;
};

//-----------------------------------------------------------------------------

IllumAmbientOcclusionSAT::IllumAmbientOcclusionSAT(
    std::string parentId,
    Ogre::SceneManager* sceneManager,
    bool ao,
    bool shadows,
    std::optional<sat_parameters_t> parameters
) :
    m_ao(ao),
    m_shadows(shadows),
    m_parameters(parameters.value_or(sat_parameters_t {})),
    m_parentId(parentId),
    m_sceneManager(sceneManager),
    m_sat(m_parentId, m_sceneManager, m_parameters.size_ratio)
{
}

//-----------------------------------------------------------------------------

IllumAmbientOcclusionSAT::~IllumAmbientOcclusionSAT()
{
    if(m_dummyCamera != nullptr)
    {
        m_sceneManager->destroyCamera(m_dummyCamera.get());
    }
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::updateSatFromRatio(float _satSizeRatio)
{
    m_parameters.size_ratio = _satSizeRatio;
    m_sat.updateSatFromRatio(m_parameters.size_ratio);
    updateTexture();
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::SATUpdate(
    const Texture::sptr& _img,
    const TransferFunction::sptr& _tf,
    float _sampleDistance
)
{
    m_sat.computeParallel(_img, _tf, _sampleDistance);
    this->updateVolumeIllumination();
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::updateVolumeIllumination()
{
    // Do this for now but at the end we should use our own texture
    m_illuminationVolume = m_sat.getSpareTexture();

    if(m_illuminationVolume != nullptr)
    {
        const int depth = static_cast<int>(m_illuminationVolume->getDepth());

        //Should never happen, but who knows
        if(depth > 0)
        {
            Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();

            const std::string currentMaterialName = "VolIllum"
                                                    + std::string(m_ao ? "_AO" : "")
                                                    + std::string(m_shadows ? "_Shadows" : "");

            Ogre::MaterialPtr volIllumMtl = Ogre::MaterialManager::getSingleton().getByName(
                currentMaterialName,
                RESOURCE_GROUP
            );

            if(volIllumMtl->getNumTechniques() > 0)
            {
                Ogre::Technique* const technique = volIllumMtl->getTechnique(0);
                if(technique->getNumPasses() > 0)
                {
                    Ogre::Pass* const pass                    = technique->getPass(0);
                    Ogre::TextureUnitState* const satImgState = pass->getTextureUnitState("sat");

                    if(satImgState != nullptr)
                    {
                        satImgState->setTexture(m_sat.getTexture());

                        //Update illumination volume slice by slice.
                        for(m_currentSliceIndex = 0 ; m_currentSliceIndex < depth ; ++m_currentSliceIndex)
                        {
                            //The current render target index
                            const std::size_t target = static_cast<size_t>(m_currentSliceIndex);

                            Ogre::RenderTarget* const rt =
                                m_illuminationVolume->getBuffer()->getRenderTarget(target);

                            if(rt->getNumViewports() > 0)
                            {
                                Ogre::Viewport* const vp = rt->getViewport(0);

                                //Temporary listener used to update shaders parameters when the target is updated.s
                                VolIllumCompositorListener listener(m_currentSliceIndex, m_parameters);

                                //Add compositor
                                compositorManager.addCompositor(vp, "VolumeIllumination");
                                compositorManager.setCompositorEnabled(vp, "VolumeIllumination", true);

                                Ogre::CompositorChain* const compChain       = compositorManager.getCompositorChain(vp);
                                Ogre::CompositorInstance* const compInstance = compChain->getCompositor(
                                    "VolumeIllumination"
                                );

                                //Compositor instance technique
                                auto* const tech                      = compInstance->getTechnique();
                                const auto& passes                    = tech->getOutputTargetPass()->getPasses();
                                Ogre::CompositionPass* const compPass = passes[0];

                                if(compPass->getMaterial()->getName() != currentMaterialName)
                                {
                                    compPass->setMaterialName(currentMaterialName);
                                }

                                //Add the temporary listener
                                compInstance->addListener(&listener);

                                // Compute the volume illumination
                                rt->update(false);

                                // Remove it
                                compInstance->removeListener(&listener);

                                compositorManager.setCompositorEnabled(vp, "VolumeIllumination", false);
                                compositorManager.removeCompositor(vp, "VolumeIllumination");
                            }
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IllumAmbientOcclusionSAT::updateTexture()
{
    Ogre::TexturePtr satTexture = m_sat.getSpareTexture();

    Ogre::TextureManager& textureManager = Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    textureManager.remove(m_parentId + BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);

    m_illuminationVolume = textureManager.createManual(
        m_parentId + BUFFER_NAME,
        viz::scene3d::RESOURCE_GROUP,
        Ogre::TEX_TYPE_3D,
        satTexture->getWidth(),
        satTexture->getHeight(),
        satTexture->getDepth(),
        0,
        Ogre::PF_A8R8G8B8,
        Ogre::TU_RENDERTARGET
    );

    if(m_dummyCamera == nullptr)
    {
        m_dummyCamera.reset(m_sceneManager->createCamera(m_parentId + "_VolumeIllumination_DummyCamera"));
    }

    const int depth = static_cast<int>(satTexture->getDepth());
    for(int sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        // Init source buffer
        Ogre::RenderTarget* renderTarget =
            m_illuminationVolume->getBuffer()->getRenderTarget(static_cast<std::size_t>(sliceIndex));
        Ogre::Viewport* vp = renderTarget->addViewport(m_dummyCamera.get());

        vp->setOverlaysEnabled(false);
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
