/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/operators/BoxParameterEditor.hpp"


namespace fwWX
{
namespace operators
{
//------------------------------------------------------------------------------

BoxParameterEditor::BoxParameterEditor( wxWindow * parent, ::fwData::Image::sptr pImage ) :
	wxDialog( parent, wxID_ANY, _("Box parameter editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_xMin ( 0 ),
	m_yMin ( 0 ),
	m_zMin ( 0 ),
	m_xMax ( pImage->getSize()[0]-1 ),
	m_yMax ( pImage->getSize()[1]-1 ),
	m_zMax ( pImage->getSize()[2]-1 ),
	m_fillValue ( 0 )
{
	createContent();
}

//------------------------------------------------------------------------------

BoxParameterEditor::~BoxParameterEditor(){}

//------------------------------------------------------------------------------

void BoxParameterEditor::createContent()
{
	wxStaticText * pXMinInfo = new wxStaticText( this, wxID_ANY, _(" x min : ") );
	wxStaticText * pYMinInfo = new wxStaticText( this, wxID_ANY, _(" y min : ") );
	wxStaticText * pZMinInfo = new wxStaticText( this, wxID_ANY, _(" z min : ") );
	wxStaticText * pXMaxInfo = new wxStaticText( this, wxID_ANY, _(" x max : ") );
	wxStaticText * pYMaxInfo = new wxStaticText( this, wxID_ANY, _(" y max : ") );
	wxStaticText * pZMaxInfo = new wxStaticText( this, wxID_ANY, _(" z max : ") );
	wxStaticText * pFillValueInfo = new wxStaticText( this, wxID_ANY, _(" fill value : ") );

	wxTextCtrl * pXMin = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pXMax = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pYMin = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pYMax = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pZMin = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pZMax = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pFillValue = new wxTextCtrl( this, wxID_ANY );

	pXMin->SetValidator( ::fwWX::validator::LongValidator( m_xMin ) );
	pYMin->SetValidator( ::fwWX::validator::LongValidator( m_yMin ) );
	pZMin->SetValidator( ::fwWX::validator::LongValidator( m_zMin ) );
	pXMax->SetValidator( ::fwWX::validator::LongValidator( m_xMax ) );
	pYMax->SetValidator( ::fwWX::validator::LongValidator( m_yMax ) );
	pZMax->SetValidator( ::fwWX::validator::LongValidator( m_zMax ) );
	pFillValue->SetValidator( ::fwWX::validator::LongValidator( m_fillValue ) );


	wxFlexGridSizer	* sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
	sizer->AddGrowableCol( 1 );

	sizer->Add( pXMinInfo, 1, wxLEFT, 10 );
	sizer->Add( pXMin, 1, wxRIGHT, 10);
	sizer->Add( pXMaxInfo, 1, wxLEFT, 10 );
	sizer->Add( pXMax, 1, wxRIGHT, 10);

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pYMinInfo, 1, wxLEFT, 10 );
	sizer->Add( pYMin, 1, wxRIGHT, 10);
	sizer->Add( pYMaxInfo, 1, wxLEFT, 10 );
	sizer->Add( pYMax, 1, wxRIGHT, 10);

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pZMinInfo, 1, wxLEFT, 10 );
	sizer->Add( pZMin, 1, wxRIGHT, 10);
	sizer->Add( pZMaxInfo, 1, wxLEFT, 10 );
	sizer->Add( pZMax, 1, wxRIGHT, 10);

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pFillValueInfo, 1 , wxLEFT, 10 );
	sizer->Add( pFillValue, 1, wxRIGHT, 10);

	m_checkbox = new wxCheckBox(this, wxID_ANY, _("is inside"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
//	m_checkbox->SetValue(true);

	sizer->Add( m_checkbox, 1 , wxLEFT, 10 );


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

int BoxParameterEditor::getXMin()
{
	return m_xMin;
}

//------------------------------------------------------------------------------

int BoxParameterEditor::getYMin()
{
	return m_yMin;
}

//------------------------------------------------------------------------------

int BoxParameterEditor::getZMin()
{
	return m_zMin;
}

//------------------------------------------------------------------------------

int BoxParameterEditor::getXMax()
{
	return m_xMax;
}

//------------------------------------------------------------------------------

int BoxParameterEditor::getYMax()
{
	return m_yMax;
}

//------------------------------------------------------------------------------

int BoxParameterEditor::getZMax()
{
	return m_zMax;
}

//------------------------------------------------------------------------------

bool BoxParameterEditor::isInside()
{
	return m_checkbox->IsChecked();
}

//------------------------------------------------------------------------------

int BoxParameterEditor::getFillValue()
{
	return m_fillValue;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
