/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include "fwWX/operators/FlipParameterEditor.hpp"

namespace fwWX
{
namespace operators
{

FlipParameterEditor::FlipParameterEditor( wxWindow * parent ) :
        wxDialog( parent, wxID_ANY, _("Image Flip"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
        wxString directionChoices[3] = { _("Axial"), _("Frontal"), _("Sagittal")} ;

        wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;

        dialogBoxSizerV->Add(
                        m_directionRadioBox = new wxRadioBox( this, wxID_ANY, _T("Direction"), wxDefaultPosition, wxDefaultSize, 3, directionChoices ),
                        0,
                        wxLEFT | wxRIGHT | wxEXPAND ,
                        10
                        );

        dialogBoxSizerV->AddSpacer( 10 ) ;
        dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
        dialogBoxSizerV->AddSpacer( 10 );

        this->SetSizer( dialogBoxSizerV ) ;
        dialogBoxSizerV->SetSizeHints( this ) ;


}

//------------------------------------------------------------------------------

FlipParameterEditor::~FlipParameterEditor(){}


//------------------------------------------------------------------------------

int FlipParameterEditor::getAxis()
{
        return m_directionRadioBox->GetSelection() ;
}

//------------------------------------------------------------------------------

} //operators
} //fwWX
