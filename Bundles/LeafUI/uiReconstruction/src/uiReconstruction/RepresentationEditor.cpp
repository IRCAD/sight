/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/window.h>
#include <wx/event.h>
#include <wx/sizer.h>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwComEd/MaterialMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>

#include <fwWX/convert.hpp>

#include "uiReconstruction/RepresentationEditor.hpp"

namespace uiReconstruction
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiReconstruction::RepresentationEditor , ::fwData::Reconstruction ) ;


RepresentationEditor::RepresentationEditor() throw()
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

RepresentationEditor::~RepresentationEditor() throw()
{}

//------------------------------------------------------------------------------

void RepresentationEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::gui::editor::IEditor::starting();

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );

    wxString radioBoxRepresentationChoices[] = { wxT("Surface"), wxT("Point"), wxT("Wireframe"), wxT("Edge") };
    int radioBoxRepresentationNChoices = sizeof( radioBoxRepresentationChoices ) / sizeof( wxString );
    m_radioBoxRepresentation = new wxRadioBox( m_container, wxNewId(), wxT("Representation"), wxDefaultPosition, wxDefaultSize, radioBoxRepresentationNChoices, radioBoxRepresentationChoices, 1, wxRA_SPECIFY_COLS );
    m_radioBoxRepresentation->SetSelection(0);

    wxString radioBoxShadingChoices[] = { wxT("Flat"), wxT("Gouraud"), wxT("Phong") };
    int radioBoxShadingNChoices = sizeof( radioBoxShadingChoices ) / sizeof( wxString );
    m_radioBoxShading = new wxRadioBox( m_container, wxNewId(), wxT("Shading"), wxDefaultPosition, wxDefaultSize, radioBoxShadingNChoices, radioBoxShadingChoices, 1, wxRA_SPECIFY_COLS );
    m_radioBoxShading->SetSelection(0);

    m_normalsCheckBox = new wxCheckBox( m_container, wxNewId(), _("Show normals"));
    m_normalsCheckBox->SetToolTip(_("Show or hide normals"));

    int border = 3;
    sizer->Add( m_radioBoxRepresentation, 1, wxEXPAND|wxALL, border);
    sizer->Add( m_radioBoxShading, 1, wxEXPAND|wxALL, border);
    sizer->Add( m_normalsCheckBox, 1, wxEXPAND|wxALL, border);

    m_container->SetSizer( sizer );
    m_container->Layout();

    m_container->Enable(false);

    m_container->Bind( wxEVT_COMMAND_RADIOBOX_SELECTED, &RepresentationEditor::onChangeRepresentation, this,  m_radioBoxRepresentation->GetId());
    m_container->Bind( wxEVT_COMMAND_RADIOBOX_SELECTED, &RepresentationEditor::onChangeShading, this,  m_radioBoxShading->GetId());
    m_container->Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &RepresentationEditor::onShowNormals, this,  m_normalsCheckBox->GetId());
    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_container->Unbind( wxEVT_COMMAND_RADIOBOX_SELECTED, &RepresentationEditor::onChangeRepresentation, this,  m_radioBoxRepresentation->GetId());
    m_container->Unbind( wxEVT_COMMAND_RADIOBOX_SELECTED, &RepresentationEditor::onChangeShading, this,  m_radioBoxShading->GetId());
    m_container->Unbind( wxEVT_COMMAND_CHECKBOX_CLICKED, &RepresentationEditor::onShowNormals, this,  m_normalsCheckBox->GetId());

    ::gui::editor::IEditor::stopping();
}

//------------------------------------------------------------------------------

void RepresentationEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::gui::editor::IEditor::configuring();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No Reconstruction!", reconstruction);

    m_material = reconstruction->getMaterial() ;
    m_container->Enable(!reconstruction->getOrganName().empty());

    this->refreshRepresentation();
    this->refreshNormals();
    this->refreshShading() ;
}

//------------------------------------------------------------------------------

void RepresentationEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void RepresentationEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeRepresentation( wxCommandEvent & event )
{
    int choix = m_radioBoxRepresentation->GetSelection();
    ::fwData::Material::REPRESENTATION_MODE selectedMode = ::fwData::Material::MODE_SURFACE;

    switch(choix)
    {
        case 0 :
        {
            selectedMode = ::fwData::Material::MODE_SURFACE;
            break ;
        }
        case 1 :
        {
            selectedMode = ::fwData::Material::MODE_POINT;
            break ;
        }
        case 2 :
        {
            selectedMode = ::fwData::Material::MODE_WIREFRAME;
            break ;
        }
        case 3 :
        {
            selectedMode = ::fwData::Material::MODE_EDGE;
            break ;
        }
    }

    m_material->setRepresentationMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeShading( wxCommandEvent & event )
{
    int choix = m_radioBoxShading->GetSelection();
    ::fwData::Material::SHADING_MODE selectedMode = ::fwData::Material::MODE_PHONG;

    switch(choix)
    {
        case 0 :
        {
            selectedMode = ::fwData::Material::MODE_FLAT;
            break ;
        }
        case 1 :
        {
            selectedMode = ::fwData::Material::MODE_GOURAUD;
            break ;
        }
        case 2 :
        {
            selectedMode = ::fwData::Material::MODE_PHONG;
            break ;
        }
    }

    m_material->setShadingMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshRepresentation()
{
    int representationMode = m_material->getRepresentationMode();

    switch(representationMode)
    {
    case ::fwData::Material::MODE_SURFACE:
    {
        m_radioBoxRepresentation->SetSelection(0);
        break ;
    }
    case ::fwData::Material::MODE_POINT:
    {
        m_radioBoxRepresentation->SetSelection(1);
        break ;
    }
    case ::fwData::Material::MODE_WIREFRAME:
    {
        m_radioBoxRepresentation->SetSelection(2);
        break ;
    }
    case ::fwData::Material::MODE_EDGE:
    {
        m_radioBoxRepresentation->SetSelection(3);
        break ;
    }
    default :
        m_radioBoxRepresentation->SetSelection(0);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshShading()
{
    int shadingMode = m_material->getShadingMode();

    switch(shadingMode)
    {
    case ::fwData::Material::MODE_FLAT:
    {
        m_radioBoxShading->SetSelection(0);
        break ;
    }
    case ::fwData::Material::MODE_GOURAUD:
    {
        m_radioBoxShading->SetSelection(1);
        break ;
    }
    case ::fwData::Material::MODE_PHONG:
    {
        m_radioBoxShading->SetSelection(2);
        break ;
    }
    default :
        m_radioBoxShading->SetSelection(2);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshNormals()
{
    m_normalsCheckBox->SetValue(m_material->getOptionsMode() == ::fwData::Material::MODE_NORMALS);
}

//------------------------------------------------------------------------------

void RepresentationEditor::onShowNormals(wxCommandEvent & event )
{
    if ( m_normalsCheckBox->IsChecked() )
    {
        m_material->setOptionsMode( ::fwData::Material::MODE_NORMALS );
    }
    else
    {
        m_material->setOptionsMode( ::fwData::Material::MODE_STANDARD );
    }

    this->notifyTriangularMesh();
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMaterial()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();

    ::fwComEd::MaterialMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), reconstruction->getMaterial(), msg);
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyTriangularMesh()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();

    ::fwComEd::MaterialMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), reconstruction->getTriangularMesh(), msg);
}
}

