/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/op/Com.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwServices/ComChannelService.hpp>

#include "tuto02DataServiceBasicCtrl/Plugin.hpp"

namespace tuto02DataServiceBasicCtrl
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::tuto02DataServiceBasicCtrl::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() throw()
{}

//------------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//------------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{}

//------------------------------------------------------------------------------

void Plugin::initialize() throw( ::fwRuntime::RuntimeException )
{
    m_image = ::fwData::Image::New();

    // Reader service
    m_readerSrv = ::fwServices::add(m_image, "::io::IReader", "::ioVTK::ImageReaderService");
    ::fwRuntime::EConfigurationElement::NewSptr readerCfg( "service" );
    ::fwRuntime::EConfigurationElement::NewSptr readerFilenameCfg( "filename" );
    readerFilenameCfg->setAttributeValue("id", "./TutoData/patient1.vtk");
    readerCfg->addConfigurationElement(readerFilenameCfg);
    m_readerSrv->setConfiguration( readerCfg ) ;
    m_readerSrv->configure();

    // Render service
    m_renderSrv = ::fwServices::add(m_image, "::fwRender::IRender", "::vtkSimpleNegato::RendererService", "myRenderingTuto");
    m_renderSrv->configure();

    // ComChannel service
    m_comChannel = ::fwServices::registerCommunicationChannel( m_image , m_renderSrv);
    m_comChannel->configure();

    // Frame service
    m_frameSrv = ::fwServices::add(m_image, "::fwGui::IFrameSrv", "::gui::frame::DefaultFrame");

    ::fwRuntime::EConfigurationElement::NewSptr frameCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr guiCfg("gui");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameCfg("frame");

    ::fwRuntime::EConfigurationElement::NewSptr guiFrameNameCfg("name");
    guiFrameNameCfg->setValue("tutoDataServiceBasic");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameIconCfg("icon");
    guiFrameIconCfg->setValue("Bundles/Tuto02DataServiceBasicCtrl_0-1/tuto.ico");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameMinSizeCfg("minSize");
    guiFrameMinSizeCfg->setAttributeValue("width", "800");
    guiFrameMinSizeCfg->setAttributeValue("height", "600");
    guiFrameCfg->addConfigurationElement(guiFrameNameCfg);
    guiFrameCfg->addConfigurationElement(guiFrameIconCfg);
    guiFrameCfg->addConfigurationElement(guiFrameMinSizeCfg);
    guiCfg->addConfigurationElement(guiFrameCfg);

    ::fwRuntime::EConfigurationElement::NewSptr registryCfg("registry");
    ::fwRuntime::EConfigurationElement::NewSptr registryViewCfg("view");
    registryViewCfg->setAttributeValue("sid", "myRenderingTuto");
    registryViewCfg->setAttributeValue("start", "yes");
    registryCfg->addConfigurationElement(registryViewCfg);

    frameCfg->addConfigurationElement(guiCfg);
    frameCfg->addConfigurationElement(registryCfg);

    m_frameSrv->setConfiguration( frameCfg ) ;
    m_frameSrv->configure();

    // Start app
    m_comChannel->start();
    m_readerSrv->start();
    m_frameSrv->start();

    m_readerSrv->update();
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//------------------------------------------------------------------------------

void Plugin::uninitialize() throw()
{
    m_comChannel->stop();
    m_readerSrv->stop();
    m_frameSrv->stop();
    ::fwServices::OSR::unregisterService( m_comChannel ) ;
    ::fwServices::OSR::unregisterService( m_readerSrv ) ;
    ::fwServices::OSR::unregisterService( m_frameSrv ) ;
    ::fwServices::OSR::unregisterService( m_renderSrv ) ;
    m_image.reset();
}

//------------------------------------------------------------------------------

} // namespace tuto02DataServiceBasicCtrl
