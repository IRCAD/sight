/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/validator/DoubleValidator.hpp"
#include "fwWX/operators/GradientPropagationParameterEditor.hpp"


namespace fwWX
{
namespace operators
{
//------------------------------------------------------------------------------

GradientPropagationParameterEditor::GradientPropagationParameterEditor( wxWindow * parent ) :
	wxDialog( parent, wxID_ANY, _("Gradient propagation parameter editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_seedX ( 0 ),
	m_seedY ( 0 ),
	m_seedZ ( 0 ),
	m_intensityMin( 0 ),
	m_intensityMax( 255 ),
	m_gradientMin( 0.0 ),
	m_gradientMax( 10.0 ),
	m_color( 255 )
{
	createContent();
}

//------------------------------------------------------------------------------

GradientPropagationParameterEditor::~GradientPropagationParameterEditor(){}

//------------------------------------------------------------------------------

void GradientPropagationParameterEditor::createContent()
{
	wxStaticText * pseedXInfo = new wxStaticText( this, wxID_ANY, _(" X Coord : ") );
	wxStaticText * pseedYInfo = new wxStaticText( this, wxID_ANY, _(" Y Coord : ") );
	wxStaticText * pseedZInfo = new wxStaticText( this, wxID_ANY, _(" Z Coord : ") );
	wxStaticText * pIntMinInfo = new wxStaticText( this, wxID_ANY, _("Min intensity : ") );
	wxStaticText * pIntMaxInfo = new wxStaticText( this, wxID_ANY, _("Max intensity : ") );
	wxStaticText * pGradMinInfo = new wxStaticText( this, wxID_ANY, _("Min gradient : ") );
	wxStaticText * pGradMaxInfo = new wxStaticText( this, wxID_ANY, _("Max gradient : ") );
	wxStaticText * pColorInfo = new wxStaticText( this, wxID_ANY, _("Color : ") );

	wxTextCtrl * pseedX = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pseedY = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pseedZ = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pIntMin = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pIntMax = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pGradMin = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pGradMax = new wxTextCtrl( this, wxID_ANY );
	wxTextCtrl * pColor = new wxTextCtrl( this, wxID_ANY );

	pseedX->SetValidator( ::fwWX::validator::LongValidator( m_seedX ) );
	pseedY->SetValidator( ::fwWX::validator::LongValidator( m_seedY ) );
	pseedZ->SetValidator( ::fwWX::validator::LongValidator( m_seedZ ) );
	pIntMin->SetValidator( ::fwWX::validator::LongValidator( m_intensityMin ) );
	pIntMax->SetValidator( ::fwWX::validator::LongValidator( m_intensityMax ) );
	pGradMin->SetValidator( ::fwWX::validator::DoubleValidator( m_gradientMin ) );
	pGradMax->SetValidator( ::fwWX::validator::DoubleValidator( m_gradientMax ) );
	pColor->SetValidator( ::fwWX::validator::LongValidator( m_color ) );

	wxFlexGridSizer	* sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
	sizer->AddGrowableCol( 1 );

	sizer->Add( pseedXInfo, 1, wxLEFT, 10 );
	sizer->Add( pseedX, 1, wxRIGHT, 10 );

	sizer->Add( pseedYInfo, 1, wxLEFT, 10 );
	sizer->Add( pseedY, 1, wxRIGHT, 10 );

	sizer->Add( pseedZInfo, 1, wxLEFT, 10 );
	sizer->Add( pseedZ, 1, wxRIGHT, 10 );

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pIntMinInfo, 1, wxLEFT, 10 );
	sizer->Add( pIntMin, 1, wxRIGHT, 10 );

	sizer->Add( pIntMaxInfo, 1, wxLEFT, 10 );
	sizer->Add( pIntMax, 1, wxRIGHT, 10 );

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pGradMinInfo, 1, wxLEFT, 10 );
	sizer->Add( pGradMin, 1, wxRIGHT, 10 );

	sizer->Add( pGradMaxInfo, 1, wxLEFT, 10 );
	sizer->Add( pGradMax, 1, wxRIGHT, 10 );

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pColorInfo, 1, wxLEFT, 10 );
	sizer->Add( pColor, 1, wxRIGHT, 10 );

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

int GradientPropagationParameterEditor::getSeedX()
{
	return m_seedX;
}

//------------------------------------------------------------------------------

int GradientPropagationParameterEditor::getSeedY()
{
	return m_seedY;
}

//------------------------------------------------------------------------------

int GradientPropagationParameterEditor::getSeedZ()
{
	return m_seedZ;
}

//------------------------------------------------------------------------------

int GradientPropagationParameterEditor::getIntensityMin()
{
	return m_intensityMin;
}

//------------------------------------------------------------------------------

int GradientPropagationParameterEditor::getIntensityMax()
{
	return m_intensityMax;
}

//------------------------------------------------------------------------------

double GradientPropagationParameterEditor::getGradientMin()
{
	return m_gradientMin;
}

//------------------------------------------------------------------------------

double GradientPropagationParameterEditor::getGradientMax()
{
	return m_gradientMax;
}

//------------------------------------------------------------------------------

int GradientPropagationParameterEditor::getColor()
{
	return m_color;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
