/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <wx/valtext.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>

#include <iostream>

#include "fwWX/operators/RotationParameterEditor.hpp"

namespace fwWX
{

namespace operators
{

RotationParameterEditor::RotationParameterEditor( wxWindow * parent ) :
	wxDialog( parent, wxID_ANY, _("Image Rotation"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
	wxString rotationChoices[2] = { _("+90"), _("-90")} ;

	wxString directionChoices[3] = { _("Sagittal"), _("Frontal"), _("Axial")} ;

	wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;

#ifdef __MACOSX__
	wxSizer* staticBoxSizer = new wxBoxSizer( wxVERTICAL );
#else
	wxSizer* staticBoxSizer = new wxStaticBoxSizer( wxVERTICAL, this, wxT("Rotation (degrees) :"));
#endif

//	wxStaticBox * staticBox = new wxStaticBox(this, wxID_ANY, _("Rotation (degrees) :"));
//	wxStaticBoxSizer * staticBoxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL );

	wxFlexGridSizer * flexGridSizer = new wxFlexGridSizer(2, 2, 10, 2);

//	flexGridSizer->Add(
//			m_rotationRadioBox = new wxRadioBox( this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, 2, rotationChoices ),
//			0,
//			wxLEFT | wxRIGHT | wxEXPAND ,
//			10
//			);

	m_p90RadioButton = new wxRadioButton(this, wxID_ANY, _("+90"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_m90RadioButton = new wxRadioButton(this, wxID_ANY, _("-90"));

	::boost::filesystem::path pathImageP90 ( "share/fwWX_" + std::string(FWWX_VER) + "/rotation_p90.png" );
	wxString filenameP90 ( wxConvertMB2WX(pathImageP90.string().c_str() ) );
	wxImage imageP90;
	imageP90.LoadFile(filenameP90);
	wxBitmap bitmapP90(imageP90);

	::boost::filesystem::path pathImageM90 ( "share/fwWX_" + std::string(FWWX_VER) + "/rotation_m90.png" );
	wxString filenameM90 ( wxConvertMB2WX(pathImageM90.string().c_str() ) );
	wxImage imageM90;
	imageM90.LoadFile(filenameM90);
	wxBitmap bitmapM90(imageM90);

	wxStaticBitmap * StaticBitmapP90 = new wxStaticBitmap(this, wxID_ANY, bitmapP90);
	wxStaticBitmap * StaticBitmapM90 = new wxStaticBitmap(this, wxID_ANY, bitmapM90);

	flexGridSizer->Add(m_p90RadioButton, 0, wxLEFT | wxEXPAND, 10 );
	flexGridSizer->Add(StaticBitmapP90, 0, wxRIGHT, 10 );
	flexGridSizer->Add(m_m90RadioButton, 0, wxLEFT | wxEXPAND, 10 );
	flexGridSizer->Add(StaticBitmapM90, 0, wxRIGHT, 10 );

	staticBoxSizer->Add(flexGridSizer, 0, wxCENTER , 10);

	dialogBoxSizerV->Add(staticBoxSizer, 0,	wxLEFT | wxRIGHT | wxEXPAND , 10);

	dialogBoxSizerV->Add(
			m_directionRadioBox = new wxRadioBox( this, wxID_ANY, _T("Direction :"), wxDefaultPosition, wxDefaultSize, 3, directionChoices ),
			0,
			wxLEFT | wxRIGHT | wxEXPAND ,
			10
			);

	//m_rotationRadioBox->SetSelection( 0 ) ;
	m_directionRadioBox->SetSelection( 2 ) ;

	dialogBoxSizerV->AddSpacer( 30 ) ;
	dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
	dialogBoxSizerV->AddSpacer( 10 );

	this->SetSizer( dialogBoxSizerV ) ;
	dialogBoxSizerV->SetSizeHints( this ) ;


}

//------------------------------------------------------------------------------

RotationParameterEditor::~RotationParameterEditor(){}


//------------------------------------------------------------------------------

int RotationParameterEditor::getRotation()
{
	int rotation;
	if (m_p90RadioButton->GetValue() == true)
	{
		rotation = 0;
	}
	else
	{
		rotation = 1;
	}
	return rotation ;
}

//------------------------------------------------------------------------------

int RotationParameterEditor::getAxis()
{
	return m_directionRadioBox->GetSelection() ;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
