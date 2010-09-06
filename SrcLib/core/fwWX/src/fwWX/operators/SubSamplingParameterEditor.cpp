/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include <fwWX/validator/LongValidator.hpp>

#include "fwWX/operators/SubSamplingParameterEditor.hpp"

namespace fwWX
{

namespace operators
{

SubSamplingParameterEditor::SubSamplingParameterEditor( wxWindow * parent ) :
    wxDialog( parent, wxID_ANY, _("Image sub-sampling"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
    m_frequence ( 2 ),
    m_direction ( 1 )
{
    wxStaticText * subSamplingStaticText1 ;
    //wxStaticText * subSamplingStaticText2 ;
    wxStaticText * frequenceStaticText ;
    wxStaticText * directionStaticText ;
    wxTextCtrl * frequenceValue ;


    //wxBoxSizer * dialogBoxSizerH = new wxBoxSizer( wxHORIZONTAL ) ;
    wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;
    wxFlexGridSizer * dialogBoxFlexSizer = new wxFlexGridSizer(2, 2, 2, 10) ;

    dialogBoxSizerV->Add(
            subSamplingStaticText1 = new wxStaticText( this, wxID_ANY, _T("Sub-sampling frequence : keep an image for N images.") )
            );

    dialogBoxSizerV->AddSpacer( 10 );
    //dialogBoxFlexSizer->AddStretchSpacer() ;

    /*dialogBoxFlexSizer->Add(
            subSamplingStaticText2 = new wxStaticText( this, wxID_ANY, _T("delete an image for N images.") )
            ) ;*/

    dialogBoxFlexSizer->Add(
            frequenceStaticText = new wxStaticText( this, wxID_ANY, _T("N : ") )
            , 1, wxLEFT, 10 );
    dialogBoxFlexSizer->Add(
            frequenceValue = new wxTextCtrl( this, wxID_ANY, _T("2"))
            , 0, wxRIGHT, 10) ;
    frequenceValue->SetValidator( ::fwWX::validator::LongValidator( m_frequence ,1, 1000) );

    dialogBoxFlexSizer->Add(
            directionStaticText = new wxStaticText( this, wxID_ANY, _T("") )
            , 1, wxLEFT, 10 );
    //direction choices
    wxString directionChoices[3] = { _("Sagittal"), _("Frontal"), _("Axial")} ;

    dialogBoxFlexSizer->Add(
            m_directionRadioBox = new wxRadioBox( this, wxID_ANY, _("Direction"),wxDefaultPosition, wxDefaultSize, 3, directionChoices )
            , 0, wxRIGHT, 10) ;
    m_directionRadioBox->SetSelection( 0 ) ;

    dialogBoxSizerV->Add( dialogBoxFlexSizer, 0, wxALL|wxEXPAND ) ;
    dialogBoxSizerV->AddSpacer( 10 ) ;
    dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
    dialogBoxSizerV->AddSpacer( 10 );

    this->SetSizer( dialogBoxSizerV ) ;
    dialogBoxSizerV->SetSizeHints( this ) ;

    //createContent();

}

//------------------------------------------------------------------------------

SubSamplingParameterEditor::~SubSamplingParameterEditor(){}

//------------------------------------------------------------------------------

int SubSamplingParameterEditor::getFrequence()
{
    return (int)m_frequence ;
}

//------------------------------------------------------------------------------

int SubSamplingParameterEditor::getDirection()
{
    m_direction = m_directionRadioBox->GetSelection() ;
    return m_direction ;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
