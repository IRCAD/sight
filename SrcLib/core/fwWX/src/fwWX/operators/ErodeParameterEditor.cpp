/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/operators/ErodeParameterEditor.hpp"

namespace fwWX
{
namespace operators
{

ErodeParameterEditor::ErodeParameterEditor( wxWindow * parent, wxString name ) :
 wxDialog( parent, wxID_ANY, name, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_x ( 2 ),
	m_y ( 2 ),
	m_z ( 1 )
{
	//wxPanel * panel = new wxPanel(this ) ;

	wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;
	wxFlexGridSizer * dialogBoxFlexSizer = new wxFlexGridSizer(3, 2, 2, 10) ;


	wxStaticText * staticText1 = new wxStaticText( this, wxID_ANY, _T("Size x : ") ) ;
	wxStaticText * staticText2 = new wxStaticText( this, wxID_ANY, _T("Size y : ") ) ;
	wxStaticText * staticText3 = new wxStaticText( this, wxID_ANY, _T("Size z : ") ) ;


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

	dialogBoxSizerV->AddSpacer( 10 ) ;
	dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
	dialogBoxSizerV->AddSpacer( 10 );

	this->SetSizer( dialogBoxSizerV ) ;
	dialogBoxSizerV->SetSizeHints( this ) ;

}

//------------------------------------------------------------------------------

ErodeParameterEditor::~ErodeParameterEditor(){}


//------------------------------------------------------------------------------

float ErodeParameterEditor::getX()
{
	return m_x ;
}

//------------------------------------------------------------------------------

float ErodeParameterEditor::getY()
{
	return m_y ;
}

//------------------------------------------------------------------------------

float ErodeParameterEditor::getZ()
{
	return m_z ;
}

} // operators
} // fwWX
