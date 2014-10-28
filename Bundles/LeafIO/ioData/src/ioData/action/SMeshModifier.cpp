/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/MeshMsg.hpp>

#include <fwData/Mesh.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "ioData/action/SMeshModifier.hpp"

namespace ioData
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::ioData::action::SMeshModifier , ::fwData::Mesh ) ;

//-----------------------------------------------------------------------------

SMeshModifier::SMeshModifier() throw()
{
}

//-----------------------------------------------------------------------------

SMeshModifier::~SMeshModifier() throw()
{
}

//-----------------------------------------------------------------------------

void SMeshModifier::configuring() throw( ::fwTools::Failed )
{
    this->initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("Sorry you must have one (and only one) <config functor=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "Sorry, missing attribute functor in <config> xml element.", configElement->hasAttribute("functor") );
    m_functor = configElement->getExistingAttributeValue("functor");
    OSLM_ASSERT("Wrong functor name "<<m_functor << " (required GenTriangle, GenQuad or GenTriangleQuad)",
                       m_functor == "ShakeMeshPoint"
                    || m_functor == "ColorizeMeshPoints"
                    || m_functor == "ColorizeMeshCells"
                    || m_functor == "ComputePointNormals"
                    || m_functor == "ComputeCellNormals"
                    || m_functor == "ShakePointNormals"
                    || m_functor == "ShakeCellNormals"
                    || m_functor == "MeshDeformation"
                    );

}

//-----------------------------------------------------------------------------

void SMeshModifier::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
    ::fwDataTools::Mesh::initRand();
}

//-----------------------------------------------------------------------------

void SMeshModifier::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SMeshModifier::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SMeshModifier::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();
    SLM_ASSERT("Mesh dynamicCast failed", mesh);
    ::fwComEd::MeshMsg::sptr msg = ::fwComEd::MeshMsg::New();
    try
    {
        if(m_functor == "ShakeMeshPoint")
        {
            ::fwDataTools::Mesh::shakePoint(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::VERTEX_MODIFIED );
        }
        else if(m_functor == "ColorizeMeshCells")
        {
            ::fwDataTools::Mesh::colorizeMeshCells(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::CELL_COLORS_MODIFIED );
        }
        else if(m_functor == "ColorizeMeshPoints")
        {
            ::fwDataTools::Mesh::colorizeMeshPoints(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::POINT_COLORS_MODIFIED );
        }
        else if(m_functor == "ComputeCellNormals")
        {
            ::fwDataTools::Mesh::generateCellNormals(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED );
        }
        else if(m_functor == "ComputePointNormals")
        {
            ::fwDataTools::Mesh::generatePointNormals(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED );
        }
        else if(m_functor == "ShakeCellNormals")
        {
            ::fwDataTools::Mesh::shakeCellNormals(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED );
        }
        else if(m_functor == "ShakePointNormals")
        {
            ::fwDataTools::Mesh::shakePointNormals(mesh);
            msg->addEvent( ::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED );
        }
        else if(m_functor == "MeshDeformation")
        {
            m_animator.computeDeformation( mesh, 100, 50 );
            msg->addEvent( ::fwComEd::MeshMsg::VERTEX_MODIFIED );
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

void SMeshModifier::info(std::ostream &_sstream )
{
    _sstream << "MeshGenerator Action" << std::endl;
}

//-----------------------------------------------------------------------------

}
}

