/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include "fwWX/validator/LongValidator.hpp"

#include "fwWX/operators/MeanParameterEditor.hpp"

namespace fwWX
{
namespace operators
{

MeanParameterEditor::MeanParameterEditor( wxWindow * parent, wxString title ) :
 wxDialog( parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_x ( 1 ),
	m_y ( 1 ),
	m_z ( 1 )
{
	//wxPanel * panel = new wxPanel(this ) ;

	wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;
	wxFlexGridSizer * dialogBoxFlexSizer = new wxFlexGridSizer(3, 2, 2, 20) ;


	wxStaticText * staticText1 = new wxStaticText( this, wxID_ANY, _T("Amplitude x : ") ) ;
	wxStaticText * staticText2 = new wxStaticText( this, wxID_ANY, _T("Amplitude y : ") ) ;
	wxStaticText * staticText3 = new wxStaticText( this, wxID_ANY, _T("Amplitude z : ") ) ;


	textCtrl1 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_x, 0, 1000 )) ;
	textCtrl2 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_y, 0, 1000 )) ;
	textCtrl3 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_z, 0, 1000 )) ;

	dialogBoxFlexSizer->Add( staticText1, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl1, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText2, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl2, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText3, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl3, 0, wxRIGHT, 10 ) ;

	dialogBoxSizerV->AddSpacer( 10 ) ;
	dialogBoxSizerV->Add(dialogBoxFlexSizer) ;

	dialogBoxSizerV->AddSpacer( 20 ) ;
	dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
	dialogBoxSizerV->AddSpacer( 10 );

	this->SetSizer( dialogBoxSizerV ) ;
	dialogBoxSizerV->SetSizeHints( this ) ;

}

//------------------------------------------------------------------------------

MeanParameterEditor::~MeanParameterEditor(){}


//------------------------------------------------------------------------------

float MeanParameterEditor::getX()
{
	return m_x ;
}

//------------------------------------------------------------------------------

float MeanParameterEditor::getY()
{
	return m_y ;
}

//------------------------------------------------------------------------------

float MeanParameterEditor::getZ()
{
	return m_z ;
}

} // operators
} // fwWX
