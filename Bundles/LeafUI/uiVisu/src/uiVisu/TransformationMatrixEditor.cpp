/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/window.h>
#include <wx/event.h>
#include <wx/sizer.h>

#include <cmath>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwServices/Base.hpp>


#include <fwWX/convert.hpp>

#include "uiVisu/TransformationMatrixEditor.hpp"

namespace uiVisu
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::TransformationMatrixEditor , ::fwData::TransformationMatrix3D ) ;


TransformationMatrixEditor::TransformationMatrixEditor() throw()
{
    addNewHandledEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED);
}

//------------------------------------------------------------------------------

TransformationMatrixEditor::~TransformationMatrixEditor() throw()
{}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::gui::editor::IEditor::starting();

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );

    m_angleSlider = new wxSlider(m_container, wxNewId(), 0, 0, 360, wxDefaultPosition, wxDefaultSize , wxSL_BOTH|wxSL_HORIZONTAL|wxSL_LABELS|wxSL_TOP ) ;

    sizer->Add(m_angleSlider, 0, wxALL|wxEXPAND);
    m_container->SetSizer( sizer );
    m_container->Layout();

    m_container->Bind(wxEVT_COMMAND_SLIDER_UPDATED, &TransformationMatrixEditor::onSliderChange, this,  m_angleSlider->GetId());
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_container->Unbind(wxEVT_COMMAND_SLIDER_UPDATED, &TransformationMatrixEditor::onSliderChange, this,  m_angleSlider->GetId());

    ::gui::editor::IEditor::stopping();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::gui::editor::IEditor::configuring();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void TransformationMatrixEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    ::fwData::TransformationMatrix3D::sptr tm3D = this->getObject< ::fwData::TransformationMatrix3D >();

    int angle = acos (tm3D->getCoefficient(0,0)) * 180.0 / M_PI;
    m_angleSlider->SetValue(angle);
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::onSliderChange( wxCommandEvent& event )
{
    ::fwData::TransformationMatrix3D::sptr tm3D = this->getObject< ::fwData::TransformationMatrix3D >();

    int angle = m_angleSlider->GetValue();
    double angleRad = angle*M_PI/180.0;

    double cosAngle = cos(angleRad);
    double sinAngle = sin(angleRad);

    tm3D->setCoefficient(0,0, cosAngle); tm3D->setCoefficient(0,1, -sinAngle); tm3D->setCoefficient(0,2, 0); tm3D->setCoefficient(0,3, 0);
    tm3D->setCoefficient(1,0, sinAngle); tm3D->setCoefficient(1,1, cosAngle);  tm3D->setCoefficient(1,2, 0); tm3D->setCoefficient(1,2, 0);
    tm3D->setCoefficient(2,0, 0);        tm3D->setCoefficient(2,1, 0);         tm3D->setCoefficient(2,2, 1); tm3D->setCoefficient(2,3, 0);
    tm3D->setCoefficient(3,0, 0);        tm3D->setCoefficient(3,1, 0);         tm3D->setCoefficient(3,2, 0); tm3D->setCoefficient(3,3, 1);

    ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), tm3D, msg);
}

//------------------------------------------------------------------------------

}

