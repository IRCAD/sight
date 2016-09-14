/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/ChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include "fwRenderOgre/compositor/SaoListener.hpp"
#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>

#include <regex>

namespace fwRenderOgre
{

namespace compositor
{


//-----------------------------------------------------------------------------

const ChainManager::CompositorIdType ChainManager::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

//-----------------------------------------------------------------------------

ChainManager::ChainManager(::Ogre::Viewport* _viewport) :
    m_ogreViewport(_viewport)
{
    m_adaptorsObjectsOwner = ::fwData::Composite::New();
}

//-----------------------------------------------------------------------------

ChainManager::~ChainManager()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void ChainManager::addAvailableCompositor(CompositorIdType _compositorName)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(!(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR)).isNull())
    {
        compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager.removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    }

    // Add the new compositor
    m_compositorChain.push_back(CompositorType(_compositorName, false));
    ::Ogre::CompositorInstance* compositor = compositorManager.addCompositor(m_ogreViewport, _compositorName);

    // TODO: Handle this with a proper registration of the listener so that future extensions do not need to modify
    // anything here
    if (_compositorName == "SAO" )
    {
        compositor->addListener(new SaoListener(m_ogreViewport));
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void ChainManager::clearCompositorChain()
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    for(auto& chain : m_compositorChain)
    {
        compositorManager.setCompositorEnabled(m_ogreViewport, chain.first, false);
        compositorManager.removeCompositor(m_ogreViewport, chain.first);
    }

    m_compositorChain.clear();
}

//-----------------------------------------------------------------------------

void ChainManager::updateCompositorState(CompositorIdType _compositorName, bool _isEnabled,
                                         const std::string& _layerId, ::fwRenderOgre::SRender::sptr _renderService)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    // If there isn't any compositor available, the update operation can't be done
    if(!m_compositorChain.empty())
    {
        auto compositorToUpdate = std::find_if(m_compositorChain.begin(),
                                               m_compositorChain.end(),
                                               FindCompositorByName(_compositorName));

        if(compositorToUpdate != m_compositorChain.end())
        {
            compositorToUpdate->second = _isEnabled;
            compositorManager.setCompositorEnabled(m_ogreViewport, _compositorName, _isEnabled);
        }
    }

    this->updateCompositorAdaptors(_compositorName, _isEnabled, _layerId, _renderService);

}

//-----------------------------------------------------------------------------

void ChainManager::setCompositorChain(const std::vector<CompositorIdType>& _compositors,
                                      const std::string& _layerId, ::fwRenderOgre::SRender::sptr _renderService)
{
    this->clearCompositorChain();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    // Remove the final chain compositor if present
    if(!(compositorManager.getByName(FINAL_CHAIN_COMPOSITOR)).isNull())
    {
        compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager.removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    }

    for(const CompositorIdType& compositorName : _compositors)
    {
        if(compositorManager.resourceExists(compositorName))
        {
            m_compositorChain.push_back(CompositorType(compositorName, true));
            compositorManager.addCompositor(m_ogreViewport, compositorName);
            compositorManager.setCompositorEnabled(m_ogreViewport, compositorName, true);

            this->updateCompositorAdaptors(compositorName, true, _layerId, _renderService);
        }
        else
        {
            SLM_ERROR("'" + compositorName + "' does not refer to an existing compositor");
        }
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void ChainManager::addFinalCompositor()
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    compositorManager.addCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    compositorManager.setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, true);
}

//-----------------------------------------------------------------------------

void ChainManager::updateCompositorAdaptors(CompositorIdType _compositorName, bool _isEnabled,
                                            const std::string& _layerId, ::fwRenderOgre::SRender::sptr _renderService)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    ::Ogre::CompositorChain* compChain           = compositorManager.getCompositorChain(m_ogreViewport);
    ::Ogre::CompositorInstance* compositor       = compChain->getCompositor(_compositorName);
    SLM_ASSERT("The given compositor '" + _compositorName + "' doesn't exist in the compositor chain", compositor);

    ::Ogre::CompositionTechnique* tech = compositor->getTechnique();

    std::vector< ::Ogre::CompositionTargetPass*> targetPasses;

    // Collect target passes
    const size_t numTargetPasses = tech->getNumTargetPasses();
    for(size_t j = 0; j < numTargetPasses; ++j)
    {
        ::Ogre::CompositionTargetPass* targetPass = tech->getTargetPass(j);
        targetPasses.push_back(targetPass);
    }
    targetPasses.push_back(tech->getOutputTargetPass());

    for(const auto targetPass : targetPasses)
    {
        const size_t numPasses = targetPass->getNumPasses();

        for(size_t i = 0; i < numPasses; ++i)
        {
            const ::Ogre::CompositionPass* pass = targetPass->getPass(i);
            // We retrieve the parameters of the base material in a temporary material
            const ::Ogre::MaterialPtr material = pass->getMaterial();

            if(!material.isNull() )
            {
                const auto constants = ::fwRenderOgre::helper::Shading::findMaterialConstants(*material);
                for(const auto& constant : constants)
                {
                    const std::string& constantName = std::get<0>(constant);

                    // Skip constant that start with "eu_". They are not supposed to be set by the user.
                    const std::regex eu("eu.*");
                    if(std::regex_match(constantName, eu))
                    {
                        continue;
                    }

                    const auto shaderType = std::get<2>(constant);

                    const std::string shaderTypeStr = shaderType == ::Ogre::GPT_VERTEX_PROGRAM ? "vertex" :
                                                      shaderType == ::Ogre::GPT_FRAGMENT_PROGRAM ? "fragment" :
                                                      "geometry";

                    // Naming convention for shader parameters
                    fwTools::fwID::IDType id = _layerId + "_" + shaderTypeStr + "-" + constantName;

                    if(_isEnabled && this->getRegisteredService(id) == nullptr)
                    {
                        const auto& constantType  = std::get<1>(constant);
                        const auto& constantValue = std::get<3>(constant);

                        auto obj = ::fwRenderOgre::helper::Shading::createObjectFromShaderParameter(constantType,
                                                                                                    constantValue);

                        if(obj != nullptr)
                        {
                            obj->setName(constantName);

                            // Creates an Ogre adaptor and associates it with the f4s object
                            auto osr = ::fwServices::registry::ServiceFactory::getDefault();
                            ::fwServices::IService::sptr srv = osr->create( "::visuOgreAdaptor::SCompositorParameter" );
                            srv->setID(id);
                            ::fwServices::OSR::registerService( ::fwData::Object::constCast(obj), srv );

                            auto shaderParamService = ::fwRenderOgre::IAdaptor::dynamicCast(srv);
                            shaderParamService->setRenderService(_renderService);

                            ::fwServices::IService::ConfigType config;
                            config.add("service.config.<xmlattr>.layer", _layerId);
                            config.add("service.config.<xmlattr>.compositorName", _compositorName);
                            config.add("service.config.<xmlattr>.parameter", constantName);
                            config.add("service.config.<xmlattr>.shaderType", shaderTypeStr);

                            shaderParamService->setConfiguration(config);
                            shaderParamService->configure();
                            shaderParamService->start();
                            shaderParamService->connect();

                            // Add created subservice to current service
                            this->registerService(shaderParamService);

                            (*m_adaptorsObjectsOwner)[constantName] = obj;
                        }
                    }
                    else
                    {
                        this->unregisterService(id);
                        if(m_adaptorsObjectsOwner->at< ::fwData::Object>(constantName) != nullptr)
                        {
                            m_adaptorsObjectsOwner->getContainer().erase(constantName);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace fwRenderOgre
