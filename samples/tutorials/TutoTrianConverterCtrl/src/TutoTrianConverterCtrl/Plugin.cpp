/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "TutoTrianConverterCtrl/Plugin.hpp"

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

namespace TutoTrianConverterCtrl
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::TutoTrianConverterCtrl::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    namespace po = boost::program_options;

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    ::fwRuntime::profile::Profile::ParamsContainer params = profile->getParams();

    std::string trianMeshPath;
    std::string vtkMeshPath;

    po::options_description cmdline_options("TutoTrianConverterCtrl options");
    cmdline_options.add_options()
        ("trian", po::value(&trianMeshPath)->required(), "Path of trian mesh file to convert")
        ("vtk", po::value(&vtkMeshPath)->required(), "Path of created vtk mesh file")
    ;

    po::positional_options_description p;
    p.add("trian", 1)
    .add("vtk", 2);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(params)
                  .options(cmdline_options)
                  .positional(p)
                  .run(),
                  vm);
        po::notify(vm);
    }
    catch(po::error& e)
    {
        std::cerr << cmdline_options << std::endl << e.what() << std::endl;
        return;
    }

    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

    m_mesh = ::fwData::Mesh::New();

    // create the service
    m_readerSrv = srvFactory->create("::ioData::STrianMeshReader");
    // register the mesh to the service
    ::fwServices::OSR::registerService(m_mesh, "data", ::fwServices::IService::AccessType::INOUT, m_readerSrv);

    ::fwRuntime::EConfigurationElement::sptr readerCfg         = ::fwRuntime::EConfigurationElement::New( "service" );
    ::fwRuntime::EConfigurationElement::sptr readerFilenameCfg = ::fwRuntime::EConfigurationElement::New( "file" );
    readerFilenameCfg->setValue(trianMeshPath);
    readerCfg->addConfigurationElement(readerFilenameCfg);
    m_readerSrv->setConfiguration( readerCfg );
    m_readerSrv->configure();

    // create the service
    m_writerSrv = srvFactory->create("::ioVTK::SMeshWriter");
    // register the mesh to the service
    ::fwServices::OSR::registerService(m_mesh, "data", ::fwServices::IService::AccessType::INPUT, m_writerSrv);

    ::fwRuntime::EConfigurationElement::sptr writerCfg         = ::fwRuntime::EConfigurationElement::New( "service" );
    ::fwRuntime::EConfigurationElement::sptr writerFilenameCfg = ::fwRuntime::EConfigurationElement::New( "file" );
    writerFilenameCfg->setValue(vtkMeshPath);
    writerCfg->addConfigurationElement(writerFilenameCfg);
    m_writerSrv->setConfiguration( writerCfg );
    m_writerSrv->configure();

    m_readerSrv->start();
    m_writerSrv->start();
    m_readerSrv->update();
    m_writerSrv->update();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
    if (m_writerSrv)
    {
        m_writerSrv->stop();
        ::fwServices::OSR::unregisterService( m_writerSrv );
    }
    if(m_readerSrv)
    {
        m_readerSrv->stop();
        ::fwServices::OSR::unregisterService( m_readerSrv );
    }
    m_mesh.reset();
}

//------------------------------------------------------------------------------

} // namespace TutoTrianConverterCtrl
