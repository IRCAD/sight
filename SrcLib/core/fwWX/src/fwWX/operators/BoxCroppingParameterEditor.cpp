/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"

#include "fwWX/operators/BoxCroppingParameterEditor.hpp"


namespace fwWX
{
namespace operators
{

//------------------------------------------------------------------------------

BoxCroppingParameterEditor::BoxCroppingParameterEditor( wxWindow * parent, ::fwData::Image::sptr pImage ) :
	wxDialog( parent, wxID_ANY, _("Box cropping parameter editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	m_xMin ( 0 ),
	m_yMin ( 0 ),
	m_zMin ( 0 ),
	m_xMax ( pImage->getSize()[0]-1 ),
	m_yMax ( pImage->getSize()[1]-1 ),
	m_zMax ( pImage->getSize()[2]-1 ),
	m_pImage( pImage )
{
	createContent();
}

//------------------------------------------------------------------------------

BoxCroppingParameterEditor::~BoxCroppingParameterEditor(){}

//------------------------------------------------------------------------------

void BoxCroppingParameterEditor::createContent()
{
	wxStaticText * pXMinInfo = new wxStaticText( this, wxID_ANY, _(" Sagittal slice min : ") );
	wxStaticText * pXMaxInfo = new wxStaticText( this, wxID_ANY, _(" Sagittal slice max : ") );
	wxStaticText * pYMinInfo = new wxStaticText( this, wxID_ANY, _(" Frontal slice min : ") );
	wxStaticText * pYMaxInfo = new wxStaticText( this, wxID_ANY, _(" Frontal slice max : ") );
	wxStaticText * pZMinInfo = new wxStaticText( this, wxID_ANY, _(" Axial slice min : ") );
	wxStaticText * pZMaxInfo = new wxStaticText( this, wxID_ANY, _(" Axial slice max : ") );

	wxTextCtrl * pZMin = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pZMax = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pYMin = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pYMax = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pXMin = new wxTextCtrl( this, wxID_ANY);
	wxTextCtrl * pXMax = new wxTextCtrl( this, wxID_ANY);

	pXMin->SetValidator( ::fwWX::validator::LongValidator( m_xMin , 0, m_pImage->getSize()[0]-1 ));
	pYMin->SetValidator( ::fwWX::validator::LongValidator( m_yMin , 0, m_pImage->getSize()[1]-1 ));
	pZMin->SetValidator( ::fwWX::validator::LongValidator( m_zMin , 0, m_pImage->getSize()[2]-1 ));
	pXMax->SetValidator( ::fwWX::validator::LongValidator( m_xMax , 0, m_pImage->getSize()[0]-1 ));
	pYMax->SetValidator( ::fwWX::validator::LongValidator( m_yMax , 0, m_pImage->getSize()[1]-1 ));
	pZMax->SetValidator( ::fwWX::validator::LongValidator( m_zMax , 0, m_pImage->getSize()[2]-1 ));

	wxFlexGridSizer	* sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
	sizer->AddGrowableCol( 1 );

	sizer->Add( pZMinInfo, 0, wxLEFT, 10 );
	sizer->Add( pZMin, 0, wxRIGHT, 10);
	sizer->Add( pZMaxInfo, 0, wxLEFT, 10 );
	sizer->Add( pZMax, 0, wxRIGHT, 10);

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pYMinInfo, 0, wxLEFT, 10 );
	sizer->Add( pYMin, 0, wxRIGHT, 10 );
	sizer->Add( pYMaxInfo, 0, wxLEFT, 10 );
	sizer->Add( pYMax, 0, wxRIGHT, 10);

	sizer->AddSpacer( 2 );
	sizer->AddSpacer( 2 );

	sizer->Add( pXMinInfo, 0, wxLEFT, 10);
	sizer->Add( pXMin, 0, wxRIGHT, 10);
	sizer->Add( pXMaxInfo, 0, wxLEFT, 10 );
	sizer->Add( pXMax, 0, wxRIGHT, 10);

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

int BoxCroppingParameterEditor::getXMin()
{
	return m_xMin;
}

//------------------------------------------------------------------------------

int BoxCroppingParameterEditor::getYMin()
{
	return m_yMin;
}

//------------------------------------------------------------------------------

int BoxCroppingParameterEditor::getZMin()
{
	return m_zMin;
}

//------------------------------------------------------------------------------

int BoxCroppingParameterEditor::getXMax()
{
	return m_xMax;
}

//------------------------------------------------------------------------------

int BoxCroppingParameterEditor::getYMax()
{
	return m_yMax;
}

//------------------------------------------------------------------------------

int BoxCroppingParameterEditor::getZMax()
{
	return m_zMax;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
