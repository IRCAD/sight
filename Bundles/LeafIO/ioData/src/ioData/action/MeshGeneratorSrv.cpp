/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/TriangularMeshMsg.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "ioData/action/MeshGeneratorSrv.hpp"

namespace ioData
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::ioData::action::MeshGeneratorSrv , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

MeshGeneratorSrv::MeshGeneratorSrv() throw()
{
}

//-----------------------------------------------------------------------------

MeshGeneratorSrv::~MeshGeneratorSrv() throw()
{
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::configuring() throw( ::fwTools::Failed )
{
    this->initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("Sorry you must have one (and only one) <config functor=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "Sorry, missing attribute functor in <config> xml element.", configElement->hasAttribute("functor") );
    m_functor = configElement->getExistingAttributeValue("functor");
    OSLM_ASSERT("Wrong functor name "<<m_functor << " (required GenTriangle, GenQuad or GenTriangleQuad)",
            m_functor == "GenTriangle" || m_functor == "GenQuad" || m_functor == "GenTriangleQuad");
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwTools::Object::sptr obj = this->getObject();
    if(m_functor == "GenTriangle")
    {
        ::fwData::TriangularMesh::sptr trian = this->getObject< ::fwData::TriangularMesh >();
        SLM_ASSERT("TriangularMesh dynamicCast failed", trian);
        ::fwData::Mesh::NewSptr mesh;
        ::fwDataTools::MeshGenerator::NewSptr generator;
        try
        {
            generator->generateTriangleMesh(mesh);
            ::fwDataTools::MeshGenerator::toTriangularMesh(mesh, trian);
        }
        catch (const std::exception & e)
        {
            std::stringstream ss;
            ss << "Warning during generating : " << e.what();

            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    ss.str(),
                    ::fwGui::dialog::IMessageDialog::WARNING);
        }
        ::fwComEd::TriangularMeshMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH );
        ::fwServices::IEditionService::notify(this->getSptr(), trian, msg);
    }
    else if(m_functor == "GenQuad")
    {
        SLM_WARN("Todo GenQuad");
    }
    else if(m_functor == "GenTriangleQuad")
    {
        ::fwData::TriangularMesh::sptr trian = this->getObject< ::fwData::TriangularMesh >();
        SLM_ASSERT("TriangularMesh dynamicCast failed", trian);
        ::fwData::Mesh::NewSptr mesh;
        try
        {
            ::fwDataTools::MeshGenerator::fromTriangularMesh(trian, mesh);
            ::fwDataTools::MeshGenerator::toTriangularMesh(mesh, trian);
        }
        catch (const std::exception & e)
        {
            std::stringstream ss;
            ss << "Warning during generating : " << e.what();

            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    ss.str(),
                    ::fwGui::dialog::IMessageDialog::WARNING);
        }
        ::fwComEd::TriangularMeshMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH );
        ::fwServices::IEditionService::notify(this->getSptr(), trian, msg);
    }
    ::fwServices::ObjectMsg::NewSptr objectMsg;
    objectMsg->addEvent( "VALUE_IS_MODIFIED" );
    ::fwServices::IEditionService::notify(this->getSptr(), obj, objectMsg);
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::info(std::ostream &_sstream )
{
    _sstream << "MeshGenerator Action" << std::endl;
}

//-----------------------------------------------------------------------------

}
}

