/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/validator/DoubleValidator.hpp"

#include "fwWX/operators/ConnectedComponentParameterEditor.hpp"


namespace fwWX
{
namespace operators
{

//------------------------------------------------------------------------------

ConnectedComponentParameterEditor::ConnectedComponentParameterEditor( wxWindow * parent ) :
	wxDialog( parent, wxID_ANY, _("Connected component"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_iBackground ( 0 ),
	m_dThreshold ( 1.0 ),
	m_iNbComponent ( 2 )
{
	createContent();
}

//------------------------------------------------------------------------------

ConnectedComponentParameterEditor::~ConnectedComponentParameterEditor(){}

//------------------------------------------------------------------------------

void ConnectedComponentParameterEditor::createContent()
{
	wxStaticText * pBackgroundInfo = new wxStaticText( this, wxID_ANY, _(" Background color : ") );
	wxStaticText * pThresholdInfo = new wxStaticText( this, wxID_ANY, _(" Threshold distance : ") );
	wxStaticText * pNbComponentInfo = new wxStaticText( this, wxID_ANY, _(" Number of Connected components : ") );

	wxTextCtrl * pBackground = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pThreshold = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pNbComponent = new wxTextCtrl( this, wxID_ANY );

	pBackground->SetValidator( ::fwWX::validator::LongValidator( m_iBackground ) );
	pThreshold->SetValidator( ::fwWX::validator::DoubleValidator( m_dThreshold ) );
	pNbComponent->SetValidator( ::fwWX::validator::LongValidator( m_iNbComponent, 0, 10000 ) );

	wxFlexGridSizer	* sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
	sizer->AddGrowableCol( 1 );

	sizer->Add( pBackgroundInfo, 1, wxLEFT, 10 );
	sizer->Add( pBackground, 1, wxRIGHT, 10);

	sizer->Add( pThresholdInfo, 1, wxLEFT, 10 );
	sizer->Add( pThreshold, 1, wxRIGHT, 10);

	sizer->Add( pNbComponentInfo, 1, wxLEFT, 10 );
	sizer->Add( pNbComponent, 1, wxRIGHT, 10);

	// Add button
	wxSizer *	finalSizer	( new wxBoxSizer( wxVERTICAL ) );
	finalSizer->AddSpacer( 10 );
	finalSizer->Add( sizer, 1, wxALL|wxEXPAND );
	finalSizer->AddSpacer( 20 );
	finalSizer->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
	finalSizer->AddSpacer( 10 );

	SetSizer( finalSizer );
	finalSizer->SetSizeHints( this ) ;
	CenterOnParent();
}

//------------------------------------------------------------------------------

double ConnectedComponentParameterEditor::getThreshold()
{
	return m_dThreshold;
}

//------------------------------------------------------------------------------

int ConnectedComponentParameterEditor::getBackground()
{
	return m_iBackground;
}

//------------------------------------------------------------------------------

int ConnectedComponentParameterEditor::getNbComponent()
{
	return m_iNbComponent;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
