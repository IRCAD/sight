/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/op/Com.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwServices/ComChannelService.hpp>

#include "tutoTrianConverterCtrl/Plugin.hpp"

namespace tutoTrianConverterCtrl
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::tutoTrianConverterCtrl::Plugin");

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
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    ::fwRuntime::profile::Profile::ParamsContainer params = profile->getParams();

    if(params.size() >= 4 )
    {
        std::string trianMeshPath = params.at(2);
        std::string vtkMeshPath = params.at(3);

        m_triangularMesh = ::fwData::TriangularMesh::New();
        m_readerSrv = ::fwServices::add(m_triangularMesh, "::io::IReader", "::ioData::TriangularMeshReaderService");
        ::fwRuntime::EConfigurationElement::NewSptr readerCfg( "service" );
        ::fwRuntime::EConfigurationElement::NewSptr readerFilenameCfg( "filename" );
        readerFilenameCfg->setValue(trianMeshPath);
        readerCfg->addConfigurationElement(readerFilenameCfg);
        m_readerSrv->setConfiguration( readerCfg ) ;
        m_readerSrv->configure();

        m_writerSrv = ::fwServices::add(m_triangularMesh, "::io::IWriter", "::ioVTK::MeshWriterService");
        ::fwRuntime::EConfigurationElement::NewSptr writerCfg( "service" );
        ::fwRuntime::EConfigurationElement::NewSptr writerFilenameCfg( "filename" );
        writerFilenameCfg->setAttributeValue("id", vtkMeshPath);
        writerCfg->addConfigurationElement(writerFilenameCfg);
        m_writerSrv->setConfiguration( writerCfg ) ;
        m_writerSrv->configure();

        m_readerSrv->start();
        m_writerSrv->start();
        m_readerSrv->update();
        m_writerSrv->update();
    }
    else
    {
        std::cout << "usage: launcher profile.xml meshSourceFile.trian meshDestFile.vtk" << std::endl;
    }
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//------------------------------------------------------------------------------

void Plugin::uninitialize() throw()
{
    m_writerSrv->stop();
    m_readerSrv->stop();
    ::fwServices::OSR::unregisterService( m_readerSrv ) ;
    ::fwServices::OSR::unregisterService( m_writerSrv ) ;
    m_triangularMesh.reset();
}

//------------------------------------------------------------------------------

} // namespace tutoTrianConverterCtrl
