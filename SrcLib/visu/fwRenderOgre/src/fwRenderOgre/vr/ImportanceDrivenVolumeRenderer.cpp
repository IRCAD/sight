/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "fwRenderOgre/vr/ImportanceDrivenVolumeRenderer.hpp"

#include "fwRenderOgre/Layer.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <string>

namespace fwRenderOgre
{
namespace vr
{

class ImportanceDrivenVolumeRenderer::JFACompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    JFACompositorListener(float passIndex, float nbPasses, float& blurWeight) :
        m_passIndex(passIndex),
        m_nbPasses(nbPasses),
        m_blurWeight(blurWeight)
    {

    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        if(mtl->getName().find("JFA") != std::string::npos)
        {
            const ::Ogre::Technique* const tech = mtl->getBestTechnique();

            for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
            {
                ::Ogre::GpuProgramParametersSharedPtr vrParams = tech->getPass(i)->getFragmentProgramParameters();

                vrParams->setNamedConstant("u_passIndex", m_passIndex);
                vrParams->setNamedConstant("u_nbPasses", m_nbPasses);
            }
        }
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        if(mtl->getName().find("Blur") != std::string::npos)
        {
            const ::Ogre::Technique* const tech = mtl->getBestTechnique();

            for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
            {
                ::Ogre::GpuProgramParametersSharedPtr fParams = tech->getPass(i)->getFragmentProgramParameters();
                fParams->setNamedConstant("u_blurWeight", m_blurWeight);
            }
        }
    }

private:

    float m_passIndex;
    float m_nbPasses;

    float& m_blurWeight;
};

//------------------------------------------------------------------------------

const std::string s_NONE  = "None";
const std::string s_MIMP  = "MImP";
const std::string s_AIMC  = "AImC";
const std::string s_VPIMC = "VPImC";

const std::string s_IMPORTANCE_MASK_ENTRY_POINTS_COMPOSITOR = "ImportanceMask_EntryPointsComp";
const std::string s_MIMP_COMPOSITOR                         = "IDVR_MImP_Comp";
const std::string s_AIMC_COMPOSITOR                         = "IDVR_AImC_Comp";
const std::string s_VPIMC_COMPOSITOR                        = "IDVR_VPImC_Comp";
const std::string s_JFAINIT_COMPOSITOR                      = "JFAInit";
const std::string s_JFAPING_COMPOSITOR                      = "JFAPingComp";
const std::string s_JFAPONG_COMPOSITOR                      = "JFAPongComp";
const std::string s_JFAFINALPING_COMPOSITOR                 = "JFAFinalPingComp";
const std::string s_JFAFINALPONG_COMPOSITOR                 = "JFAFinalPongComp";

const std::string s_MIMP_DEFINE  = "IDVR=1";
const std::string s_AIMC_DEFINE  = "IDVR=2";
const std::string s_VPIMC_DEFINE = "IDVR=3";

const std::string s_CSG_DEFINE                  = "CSG=1";
const std::string s_CSG_TF_DEFINE               = "CSG_TF=1";
const std::string s_CSG_BORDER_DEFINE           = "CSG_BORDER=1";
const std::string s_CSG_DISABLE_CONTEXT_DEFINE  = "CSG_DISABLE_CONTEXT=1";
const std::string s_CSG_OPACITY_DECREASE_DEFINE = "CSG_OPACITY_DECREASE=1";
const std::string s_CSG_DEPTH_LINES_DEFINE      = "CSG_DEPTH_LINES=1";

const std::string s_CSG_MOD_GRAYSCALE_AVERAGE_DEFINE    = "CSG_GRAYSCALE=1";
const std::string s_CSG_MOD_GRAYSCALE_LIGHTNESS_DEFINE  = "CSG_GRAYSCALE=2";
const std::string s_CSG_MOD_GRAYSCALE_LUMINOSITY_DEFINE = "CSG_GRAYSCALE=3";
const std::string s_CSG_MOD_COLOR1_DEFINE               = "CSG_MODULATION=4";
const std::string s_CSG_MOD_COLOR2_DEFINE               = "CSG_MODULATION=5";
const std::string s_CSG_MOD_COLOR3_DEFINE               = "CSG_MODULATION=6";

const std::string s_IMPORTANCE_COMPOSITING_TEXTURE = "IC";
const std::string s_JUMP_FLOOD_ALGORITHM_TEXTURE   = "JFA";

static const std::string s_CSG_TF_TEXUNIT_NAME = "CSGTransferFunction";

//-----------------------------------------------------------------------------

ImportanceDrivenVolumeRenderer::ImportanceDrivenVolumeRenderer(std::string _parentId,
                                                               Layer::sptr _layer,
                                                               ::Ogre::SceneNode* _parentNode,
                                                               ::Ogre::TexturePtr _imageTexture,
                                                               ::Ogre::TexturePtr _maskTexture,
                                                               const TransferFunction::sptr& _gpuVolumeTF,
                                                               const TransferFunction::sptr& _gpuCSGTF,
                                                               PreIntegrationTable& _preintegrationTable,
                                                               bool _ambientOcclusion,
                                                               bool _colorBleeding,
                                                               bool _shadows,
                                                               double _aoFactor,
                                                               double _colorBleedingFactor) :
    fwRenderOgre::vr::RayTracingVolumeRenderer(_parentId, _layer, _parentNode, _imageTexture, _gpuVolumeTF,
                                               _preintegrationTable, _ambientOcclusion, _colorBleeding, _shadows,
                                               _aoFactor, _colorBleedingFactor),
    m_maskTexture(_maskTexture),
    m_idvrMethod(s_NONE),
    m_idvrCSGAngleCosine(std::cos(::glm::pi<float>() / 12.f)) /* cos(15 degrees) */,
    m_gpuCSGTF(_gpuCSGTF),
    m_idvrMaskRayEntriesCompositor(s_IMPORTANCE_MASK_ENTRY_POINTS_COMPOSITOR, s_PROXY_GEOMETRY_RQ_GROUP,
                                   compositor::Core::StereoModeType::NONE, false)
{
    m_RTVSharedParameters->addConstantDefinition("u_csgAngleCos", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_csgBorderThickness", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_colorModulationFactor", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_opacityDecreaseFactor", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_vpimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_aimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_csgBorderColor", ::Ogre::GCT_FLOAT3);
    m_RTVSharedParameters->addConstantDefinition("u_imageSpacing", ::Ogre::GCT_FLOAT3);
    m_RTVSharedParameters->addConstantDefinition("u_depthLinesSpacing", ::Ogre::GCT_INT1);
    m_RTVSharedParameters->addConstantDefinition("u_depthLinesWidth", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_CSGTFWindow", ::Ogre::GCT_FLOAT2);

    m_RTVSharedParameters->setNamedConstant("u_csgAngleCos", m_idvrCSGAngleCosine);
    m_RTVSharedParameters->setNamedConstant("u_csgBorderThickness", m_idvrCSGBorderThickness);
    m_RTVSharedParameters->setNamedConstant("u_colorModulationFactor", m_idvrCSGModulationFactor);
    m_RTVSharedParameters->setNamedConstant("u_opacityDecreaseFactor", m_idvrCSGOpacityDecreaseFactor);
    m_RTVSharedParameters->setNamedConstant("u_csgBorderColor", m_idvrCSGBorderColor);
    m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
    m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
    m_RTVSharedParameters->setNamedConstant("u_depthLinesSpacing", 10);
    m_RTVSharedParameters->setNamedConstant("u_depthLinesWidth", 1.f);

    m_fragmentShaderAttachements.push_back("ColorFormats_FP");

    this->createIDVRTechnique();
}

//-----------------------------------------------------------------------------

ImportanceDrivenVolumeRenderer::~ImportanceDrivenVolumeRenderer()
{
    const auto layer     = this->getLayer();
    auto* const viewport = layer->getViewport();

    this->cleanCompositorChain(viewport);
}

//------------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::updateCSGTF()
{
    if(m_idvrMethod == s_MIMP && this->m_idvrCSGTF)
    {
        const ::Ogre::MaterialPtr material       = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName);
        const ::Ogre::Technique* const technique = material->getTechnique(0);
        SLM_ASSERT("Technique not found", technique);
        const auto* const pass = technique->getPass(0);
        m_gpuCSGTF.lock()->bind(pass, s_CSG_TF_TEXUNIT_NAME, m_RTVSharedParameters, "u_CSGTFWindow");
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRMethod(std::string _method)
{
    bool isSupported(false);

    if(_method == s_NONE ||
       _method == s_MIMP ||
       _method == s_AIMC ||
       _method == s_VPIMC)
    {
        isSupported = true;
    }

    SLM_FATAL_IF("IDVR method '" + _method + "' isn't supported by the ray tracing volume renderer.", !isSupported);
    m_idvrMethod = _method;

    SLM_FATAL_IF("IDVR isn't compatible with stereo rendering yet.",
                 this->getLayer()->getStereoMode() != compositor::Core::StereoModeType::NONE && m_idvrMethod != s_NONE);

    this->createMaterialAndIDVRTechnique();
    this->updateCSGTF();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::initCompositors()
{
    // Start from an empty compositor chain
    const auto layer     = this->getLayer();
    auto* const viewport = layer->getViewport();

    this->cleanCompositorChain(viewport);
    this->buildICCompositors(viewport);
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::buildICCompositors(::Ogre::Viewport* const _vp)
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    ::Ogre::CompositorInstance* compositorInstance = nullptr;
    int compositorIndex = 1;

    if(m_idvrMethod != s_NONE)
    {
        compositorInstance = compositorManager.addCompositor(_vp, s_IMPORTANCE_MASK_ENTRY_POINTS_COMPOSITOR,
                                                             compositorIndex++);
        compositorInstance->setEnabled(true);
    }

    if(m_idvrMethod == s_MIMP)
    {
        compositorInstance = compositorManager.addCompositor(_vp, s_MIMP_COMPOSITOR, compositorIndex++);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        const double numPasses =
            std::nearbyint(std::max(std::log(m_camera->getViewport()->getActualWidth()) / std::log(2.0),
                                    std::log(m_camera->getViewport()->getActualHeight()) / std::log(2.0)));

        compositorInstance = compositorManager.addCompositor(_vp, s_JFAINIT_COMPOSITOR, compositorIndex++);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        m_compositorListeners.push_back(new JFACompositorListener(static_cast<float>(0),
                                                                  static_cast<float>(numPasses),
                                                                  m_idvrCSGBlurWeight));
        compositorInstance->addListener(m_compositorListeners.back());

        int i = 0;
        for(i = 0; i < numPasses - 2; i++)
        {
            if(i % 2 == 0)
            {
                compositorInstance = compositorManager.addCompositor(_vp, s_JFAPING_COMPOSITOR, compositorIndex++);
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }
            else
            {
                compositorInstance = compositorManager.addCompositor(_vp, s_JFAPONG_COMPOSITOR, compositorIndex++);
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }

            m_compositorListeners.push_back(new JFACompositorListener(static_cast<float>(i + 1),
                                                                      static_cast<float>(numPasses),
                                                                      m_idvrCSGBlurWeight));
            compositorInstance->addListener(m_compositorListeners.back());
        }

        if(i % 2 == 0)
        {
            compositorInstance = compositorManager.addCompositor(_vp, s_JFAFINALPING_COMPOSITOR, compositorIndex++);
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }
        else
        {
            compositorInstance = compositorManager.addCompositor(_vp, s_JFAFINALPONG_COMPOSITOR, compositorIndex++);
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }

        m_compositorListeners.push_back(new JFACompositorListener(static_cast<float>(i),
                                                                  static_cast<float>(numPasses),
                                                                  m_idvrCSGBlurWeight));
        compositorInstance->addListener(m_compositorListeners.back());
    }
    else if(m_idvrMethod == s_AIMC)
    {
        compositorInstance = compositorManager.addCompositor(_vp, s_AIMC_COMPOSITOR, compositorIndex);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);
    }
    else if(m_idvrMethod == s_VPIMC)
    {
        compositorInstance = compositorManager.addCompositor(_vp, s_VPIMC_COMPOSITOR, compositorIndex);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::cleanCompositorChain(Ogre::Viewport* const _vp)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    ::Ogre::CompositorChain* compChain = compositorManager.getCompositorChain(_vp);
    SLM_ASSERT("Can't find compositor chain", compChain);

    auto& compInstances = compChain->getCompositorInstances();
    // Then clean the whole chain
    const size_t numCompositors = compInstances.size();
    std::vector<size_t> removeCompositors;

    for(size_t i = 0; i < numCompositors; ++i)
    {
        ::Ogre::CompositorInstance* targetComp = compInstances[i];
        const std::string& compName = targetComp->getCompositor()->getName();
        SLM_ASSERT("Compositor instance is null", targetComp);
        if(compName == s_MIMP_COMPOSITOR ||
           compName == s_AIMC_COMPOSITOR||
           compName == s_VPIMC_COMPOSITOR||
           compName == s_JFAINIT_COMPOSITOR ||
           compName == s_JFAPING_COMPOSITOR ||
           compName == s_JFAPONG_COMPOSITOR ||
           compName == s_JFAFINALPING_COMPOSITOR ||
           compName == s_JFAFINALPONG_COMPOSITOR ||
           compName == m_idvrMaskRayEntriesCompositor.getName()
           )
        {
            removeCompositors.push_back(i);
        }
    }

    for(auto it = removeCompositors.rbegin(); it != removeCompositors.rend(); ++it )
    {
        compChain->removeCompositor(static_cast<size_t>(*it));
    }

    // Remove all associated listeners from the compositor chain
    std::for_each(m_compositorListeners.begin(), m_compositorListeners.end(),
                  [](::Ogre::CompositorInstance::Listener* listener) { delete listener; });

    m_compositorListeners.clear();

    // Reallocate chain resources, i.e. force chain recompiling.
    for(::Ogre::CompositorInstance* const instance : compInstances)
    {
        if(instance->getEnabled())
        {
            // Mark the instance as being dead for the resources to be allocated again when enabled.
            instance->setAlive(false);
            instance->setEnabled(true);
        }
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setPreIntegratedRendering(bool _preIntegratedRendering)
{
    m_preIntegratedRendering = _preIntegratedRendering;

    this->createMaterialAndIDVRTechnique();
}
//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCountersinkGeometry(bool _CSG)
{
    m_idvrCSG = _CSG;

    if(this->m_idvrMethod == s_MIMP)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGTF(bool _enable)
{
    m_idvrCSGTF = _enable;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCountersinkAngle(double _angle)
{
    m_idvrCSGAngleCosine = static_cast<float>(std::cos(::glm::radians(_angle)));

    if(m_idvrMethod == s_MIMP && m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_csgAngleCos", m_idvrCSGAngleCosine);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGBlurWeight(double _blurWeight)
{
    m_idvrCSGBlurWeight = static_cast<float>(_blurWeight);

    if(m_idvrMethod == s_MIMP && m_idvrCSG)
    {
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGBorder(bool _border)
{
    // FIXME: find a new way to display the csg border.
    m_idvrCSGBorder = _border;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGDisableContext(bool _discard)
{
    m_idvrCSGDisableContext = _discard;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGBorderThickness(double _thickness)
{
    m_idvrCSGBorderThickness = static_cast<float>(_thickness);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        m_RTVSharedParameters->setNamedConstant("u_csgBorderThickness", m_idvrCSGBorderThickness);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGBorderColor(std::array<std::uint8_t, 4> _color)
{
    m_idvrCSGBorderColor.r = _color[0] / 256.f;
    m_idvrCSGBorderColor.g = _color[1] / 256.f;
    m_idvrCSGBorderColor.b = _color[2] / 256.f;

    if(m_idvrMethod == s_MIMP && m_idvrCSG && (m_idvrCSGBorder || m_idvrCSGDepthLines))
    {
        m_RTVSharedParameters->setNamedConstant("u_csgBorderColor", m_idvrCSGBorderColor);
        this->getLayer()->requestRender();
    }
}

//------------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGGrayScale(bool _grayScale)
{
    m_idvrCSGGrayScale = _grayScale;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//------------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGGrayScaleMethod(IDVRCSGGrayScaleMethod _method)
{
    m_idvrCSGgrayscaleMethod = _method;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGModulation)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGModulation(bool _modulation)
{
    m_idvrCSGModulation = _modulation;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGModulationMethod(IDVRCSGModulationMethod _method)
{
    m_idvrCSGModulationMethod = _method;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGModulation)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGModulationFactor(double _modulationFactor)
{
    m_idvrCSGModulationFactor = static_cast<float>(_modulationFactor);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_colorModulationFactor", m_idvrCSGModulationFactor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGOpacityDecrease(bool _opacity)
{
    m_idvrCSGOpacityDecrease = _opacity;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGOpacityDecreaseFactor(double _opacity)
{
    m_idvrCSGOpacityDecreaseFactor = static_cast<float>(_opacity);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_opacityDecreaseFactor", m_idvrCSGOpacityDecreaseFactor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRDepthLines(bool _depthLines)
{
    m_idvrCSGDepthLines = _depthLines;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRDepthLinesSpacing(int _spacing)
{
    m_RTVSharedParameters->setNamedConstant("u_depthLinesSpacing", _spacing);
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRAImCAlphaCorrection(double _alphaCorrection)
{
    m_idvrAImCAlphaCorrection = static_cast<float>(_alphaCorrection);

    if(m_idvrMethod == s_AIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRVPImCAlphaCorrection(double _alphaCorrection)
{
    m_idvrVPImCAlphaCorrection = static_cast<float>(_alphaCorrection);

    if(m_idvrMethod == s_VPIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setImageSpacing(const ::Ogre::Vector3& _spacing)
{
    m_RTVSharedParameters->setNamedConstant("u_imageSpacing", _spacing);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::resizeViewport(int _w, int _h)
{
    this->RayTracingVolumeRenderer::resizeViewport(_w, _h);

    auto layer           = this->getLayer();
    auto* const viewport = layer->getViewport();

    this->cleanCompositorChain(viewport);
    this->buildICCompositors(viewport);
}

//-----------------------------------------------------------------------------

std::tuple<std::string, std::string, size_t> ImportanceDrivenVolumeRenderer::computeRayTracingDefines() const
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;

    // Compute regular ray tracing defines.
    std::tie(vpPPDefines, fpPPDefines, hash) = this->RayTracingVolumeRenderer::computeRayTracingDefines();

    std::ostringstream vpPPDefs, fpPPDefs;
    vpPPDefs << vpPPDefines;
    fpPPDefs << fpPPDefines;

    // Add IDVR defines.
    if(m_idvrMethod != s_NONE)
    {
        if(m_idvrMethod == s_MIMP)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_MIMP_DEFINE;
            if(m_idvrCSG)
            {
                fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DEFINE;

                if(m_idvrCSGTF)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_TF_DEFINE;
                }
                if(m_idvrCSGBorder)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_BORDER_DEFINE;
                }
                if(m_idvrCSGDisableContext)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DISABLE_CONTEXT_DEFINE;
                }
                if(m_idvrCSGOpacityDecrease)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_OPACITY_DECREASE_DEFINE;
                }
                if(m_idvrCSGDepthLines)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DEPTH_LINES_DEFINE;
                }

                if(m_idvrCSGModulation)
                {
                    switch(m_idvrCSGModulationMethod)
                    {
                        case IDVRCSGModulationMethod::COLOR1:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR1_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR2:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR2_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR3:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR3_DEFINE;
                            break;
                    }

                }

                if(m_idvrCSGGrayScale)
                {
                    switch(m_idvrCSGgrayscaleMethod)
                    {
                        case IDVRCSGGrayScaleMethod::AVERAGE_GRAYSCALE:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_AVERAGE_DEFINE;
                            break;
                        case IDVRCSGGrayScaleMethod::LIGHTNESS_GRAYSCALE:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_LIGHTNESS_DEFINE;
                            break;
                        case IDVRCSGGrayScaleMethod::LUMINOSITY_GRAYSCALE:
                            fpPPDefs <<
                            (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_LUMINOSITY_DEFINE;
                            break;
                    }

                }
            }
        }
        else if(m_idvrMethod == s_AIMC)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_AIMC_DEFINE;
        }
        else if(m_idvrMethod == s_VPIMC)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_VPIMC_DEFINE;
        }
    }

    return std::make_tuple(vpPPDefs.str(), fpPPDefs.str(), std::hash<std::string>{} (vpPPDefs.str() + fpPPDefs.str()));
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setRayCastingPassTextureUnits(Ogre::Pass* const _rayCastingPass,
                                                                   const std::string& _fpPPDefines) const
{
    this->RayTracingVolumeRenderer::setRayCastingPassTextureUnits(_rayCastingPass, _fpPPDefines);

    int nbTexUnits = int(_rayCastingPass->getTextureUnitStates().size());

    ::Ogre::GpuProgramParametersSharedPtr fpParams = _rayCastingPass->getFragmentProgramParameters();
    ::Ogre::TextureUnitState* texUnitState;

    // Importance Compositing texture: MImP | AImC | VPImC
    if(m_idvrMethod != s_NONE)
    {
        const std::string compositorRef = m_idvrMethod == s_MIMP ? s_MIMP_COMPOSITOR :
                                          m_idvrMethod == s_AIMC ? s_AIMC_COMPOSITOR : s_VPIMC_COMPOSITOR;
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(compositorRef, "IC", 0);

        fpParams->setNamedConstant("u_" + s_IMPORTANCE_COMPOSITING_TEXTURE, nbTexUnits++);
    }

    // JFA texture: MImP
    if(m_idvrMethod == s_MIMP)
    {
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(s_JFAINIT_COMPOSITOR, "JFAFinal", 0);

        fpParams->setNamedConstant("u_" + s_JUMP_FLOOD_ALGORITHM_TEXTURE, nbTexUnits++);

        if(this->m_idvrCSGTF)
        {
            const auto gpuTF = m_gpuCSGTF.lock();
            texUnitState = _rayCastingPass->createTextureUnitState();
            texUnitState->setName(s_CSG_TF_TEXUNIT_NAME);
            gpuTF->bind(_rayCastingPass, texUnitState->getName(), fpParams, "u_CSGTFWindow");

            fpParams->setNamedConstant("u_CSGTFTexture", nbTexUnits++);
        }
    }

    // Alpha Correction: AImC | VPImC
    if(m_idvrMethod == s_AIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
    }
    else if(m_idvrMethod == s_VPIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::createIDVRTechnique()
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();

    ::Ogre::String vpName("RTV_VP_" + std::to_string(hash));

    ::Ogre::MaterialPtr mat = mm.getByName(m_currentMtlName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ///////////////////////////////////////////////////////////////////////////
    /// Create the technique that is used in ImportanceCompositing.compositor
    if(m_idvrMethod != s_NONE )
    {
        auto* const tech = mat->createTechnique();
        SLM_ASSERT("Can't create a new technique", tech);

        auto* const pass = tech->createPass();
        SLM_ASSERT("Can't create a new pass", pass);

        pass->setCullingMode(::Ogre::CullingMode::CULL_ANTICLOCKWISE);
        pass->setSceneBlending(::Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
        pass->setVertexProgram(vpName);

        ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
        vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        if(m_idvrMethod == s_MIMP)
        {
            tech->setName("IDVR_MImP_Mat");
            tech->setSchemeName("IDVR_MImP_Mat");
            pass->setFragmentProgram("IDVR_MImP_FP");
        }
        else if(m_idvrMethod == s_AIMC)
        {
            tech->setName("IDVR_AImC_Mat");
            tech->setSchemeName("IDVR_AImC_Mat");
            pass->setFragmentProgram("IDVR_AImC_FP");
        }
        else if(m_idvrMethod == s_VPIMC)
        {
            tech->setName("IDVR_VPImC_Mat");
            tech->setSchemeName("IDVR_VPImC_Mat");
            pass->setFragmentProgram("IDVR_VPImC_FP");
        }

        ::Ogre::TextureUnitState* texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureName(m_maskTexture->getName(), ::Ogre::TEX_TYPE_3D);
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        const auto& rayEntryCompositorName = m_idvrMaskRayEntriesCompositor.getName();
        texUnitState = pass->createTextureUnitState();
        texUnitState->setName("entryPoints");
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(rayEntryCompositorName, rayEntryCompositorName + "Texture");
        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
    }

    this->initCompositors();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::createMaterialAndIDVRTechnique()
{
    this->createRayTracingMaterial();
    this->createIDVRTechnique();
}

//-----------------------------------------------------------------------------

} // namespace vr
} // namespace fwRenderOgre
