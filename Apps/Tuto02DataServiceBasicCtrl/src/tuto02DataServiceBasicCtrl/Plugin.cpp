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
    m_readerSrv = ::fwServices::add(m_image, "::io::IReader", "::ioVTK::ImageReaderService");
    ::fwRuntime::EConfigurationElement::NewSptr readerCfg( "filename" );
    readerCfg->setAttributeValue("id", "./TutoData/patient1.vtk");
    m_readerSrv->setConfiguration( readerCfg ) ;
    m_readerSrv->configure();

    m_renderSrv = ::fwServices::add(m_image, "::fwRender::IRender", "::vtkSimpleNegato::RendererService", "myRenderingTuto");
    m_comChannel = ::fwServices::registerCommunicationChannel( m_image , m_renderSrv);

    m_frameSrv = ::fwServices::add(m_image, "::fwGui::IFrameSrv", "::gui::frame::DefaultFrame");

    ::fwRuntime::EConfigurationElement::NewSptr frameCfg("");
    ::fwRuntime::EConfigurationElement::NewSptr guiCfg("gui");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameCfg("frame");
    guiCfg->addConfigurationElement(guiFrameCfg);
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameNameCfg("name");
    guiFrameNameCfg->setValue("tutoDataServiceBasic");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameIconCfg("icon");
    guiFrameIconCfg->setValue("Bundles/Tuto02DataServiceBasic_0-1/tuto.ico");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameMinSizeCfg("minSize");
    guiFrameMinSizeCfg->setAttributeValue("width", "800");
    guiFrameMinSizeCfg->setAttributeValue("height", "600");
    guiFrameCfg->addConfigurationElement(guiFrameNameCfg);
    guiFrameCfg->addConfigurationElement(guiFrameIconCfg);
    guiFrameCfg->addConfigurationElement(guiFrameMinSizeCfg);

    ::fwRuntime::EConfigurationElement::NewSptr guiRegistryCfg("registry");
    ::fwRuntime::EConfigurationElement::NewSptr guiRegistryViewCfg("view");
    guiRegistryViewCfg->setAttributeValue("sid", "myRenderingTuto");
    guiRegistryViewCfg->setAttributeValue("start", "yes");
    guiRegistryCfg->addConfigurationElement(guiRegistryViewCfg);

    m_frameSrv->setConfiguration( frameCfg ) ;
    m_frameSrv->configure();

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
    ::fwServices::unregisterCommunicationChannel(m_image, m_comChannel);
    m_readerSrv->stop();
    m_frameSrv->stop();
    ::fwServices::OSR::unregisterService( m_readerSrv ) ;
    ::fwServices::OSR::unregisterService( m_frameSrv ) ;
    ::fwServices::OSR::unregisterService( m_renderSrv ) ;
}

//------------------------------------------------------------------------------

} // namespace tuto02DataServiceBasicCtrl
