/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/MeshMsg.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "ioData/action/MeshGeneratorSrv.hpp"

namespace ioData
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::ioData::action::MeshGeneratorSrv , ::fwData::Mesh ) ;

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
            m_functor == "GenTriangle"
                    || m_functor == "GenQuad"
                    || m_functor == "GenTriangleQuad"
                    || m_functor == "ShakeMeshPoint"
                    || m_functor == "ColorizeMeshPoints"
                    || m_functor == "ColorizeMeshCells"
                    );
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
    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();
    SLM_ASSERT("Mesh dynamicCast failed", mesh);
    ::fwComEd::MeshMsg::NewSptr msg;
    try
    {
        if(m_functor == "GenTriangle")
        {
            ::fwDataTools::MeshGenerator::generateTriangleMesh(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::NEW_MESH );
        }
        else if(m_functor == "GenQuad")
        {
            ::fwDataTools::MeshGenerator::generateQuadMesh(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::NEW_MESH );
        }
        else if(m_functor == "GenTriangleQuad")
        {
            ::fwDataTools::MeshGenerator::generateTriangleQuadMesh(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::NEW_MESH );
        }
        else if(m_functor == "ShakeMeshPoint")
        {
            ::fwDataTools::MeshGenerator::shakePoint(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::VERTEX_MODIFIED );
        }
        else if(m_functor == "ColorizeMeshCells")
        {
            //::fwDataTools::MeshGenerator::colorizeMeshCells(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::CELL_COLORS_MODIFIED );
        }
        else if(m_functor == "ColorizeMeshPoints")
        {
            ::fwDataTools::MeshGenerator::colorizeMeshPoints(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::POINT_COLORS_MODIFIED );
        }
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

    ::fwServices::IEditionService::notify(this->getSptr(), mesh, msg);
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::info(std::ostream &_sstream )
{
    _sstream << "MeshGenerator Action" << std::endl;
}

//-----------------------------------------------------------------------------

}
}

