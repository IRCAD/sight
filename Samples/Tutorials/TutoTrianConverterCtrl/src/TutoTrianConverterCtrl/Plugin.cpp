/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TutoTrianConverterCtrl/Plugin.hpp"

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/AppConfig.hpp>

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

    m_mesh      = ::fwData::Mesh::New();
    m_readerSrv = ::fwServices::add(m_mesh, "::io::IReader",
                                    "::ioData::STrianMeshReader");
    ::fwRuntime::EConfigurationElement::sptr readerCfg         = ::fwRuntime::EConfigurationElement::New( "service" );
    ::fwRuntime::EConfigurationElement::sptr readerFilenameCfg = ::fwRuntime::EConfigurationElement::New( "file" );
    readerFilenameCfg->setValue(trianMeshPath);
    readerCfg->addConfigurationElement(readerFilenameCfg);
    m_readerSrv->setConfiguration( readerCfg );
    m_readerSrv->configure();

    m_writerSrv = ::fwServices::add(m_mesh, "::io::IWriter",
                                    "::ioVTK::SMeshWriter");
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
