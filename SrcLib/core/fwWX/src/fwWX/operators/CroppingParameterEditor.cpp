/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/operators/CroppingParameterEditor.hpp"

namespace fwWX
{
namespace operators
{

CroppingParameterEditor::CroppingParameterEditor( wxWindow * parent, int max ) :
 wxDialog( parent, wxID_ANY, _("Image Cropping"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_min ( 0 ),
	m_max ( 0 )
{
	//wxPanel * panel = new wxPanel(this ) ;

	wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;
	wxFlexGridSizer * dialogBoxFlexSizer = new wxFlexGridSizer(3, 2, 2, 20) ;

	wxStaticText * staticText1 = new wxStaticText( this, wxID_ANY, _T("Min : ") ) ;
	wxStaticText * staticText2 = new wxStaticText( this, wxID_ANY, _T("Max : ") ) ;
	wxStaticText * staticText3 = new wxStaticText( this, wxID_ANY, _T("Axis : ") ) ;

	wxString directionChoices[3] = { _("X"), _("Y"), _("Z")} ;

	textCtrl1 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_min, 0, max )) ;
	textCtrl2 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_max, 0, max )) ;

	dialogBoxFlexSizer->Add( staticText1, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl1, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText2, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl2, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText3, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add(
					m_directionRadioBox = new wxRadioBox( this, wxID_ANY, _T("Axis"),wxDefaultPosition, wxDefaultSize, 3, directionChoices )
					) ;
	m_directionRadioBox->SetSelection( 2 ) ;
	//m_directionRadioBox->Enable(false) ;

	dialogBoxSizerV->AddSpacer( 10 ) ;
	dialogBoxSizerV->Add(dialogBoxFlexSizer) ;

	dialogBoxSizerV->AddSpacer( 20 ) ;
	dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
	dialogBoxSizerV->AddSpacer( 10 );

	this->SetSizer( dialogBoxSizerV ) ;
	dialogBoxSizerV->SetSizeHints( this ) ;

}

//------------------------------------------------------------------------------

CroppingParameterEditor::~CroppingParameterEditor(){}


//------------------------------------------------------------------------------

int CroppingParameterEditor::getMin()
{
	return (int) m_min ;
}

//------------------------------------------------------------------------------

int CroppingParameterEditor::getMax()
{
	return (int) m_max ;
}

//------------------------------------------------------------------------------

int CroppingParameterEditor::getAxis()
{
	return m_directionRadioBox->GetSelection() ;
}

} // operators
} // fwWX
