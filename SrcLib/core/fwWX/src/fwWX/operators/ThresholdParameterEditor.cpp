/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include "fwWX/validator/LongValidator.hpp"

#include "fwWX/operators/ThresholdParameterEditor.hpp"

namespace fwWX
{
namespace operators
{

/*BEGIN_EVENT_TABLE( ThresholdParameterEditor, wxDialog )
//EVT_BUTTON( wxID_OK  , ThresholdParameterEditor::onClickOk )
END_EVENT_TABLE()*/

ThresholdParameterEditor::ThresholdParameterEditor( wxWindow * parent ) :
	wxDialog( parent, wxID_ANY, _("Threshold image"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
	outsideValue ( 0 ),
	insideValue ( 255 ),
	lowerThreshold ( -200 ),
	upperThreshold ( 300 )
{
	//wxPanel * panel = new wxPanel(this ) ;

	wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;
	wxFlexGridSizer * dialogBoxFlexSizer = new wxFlexGridSizer(4, 2, 15, 20) ;


	wxStaticText * staticText1 = new wxStaticText( this, wxID_ANY, _T("Inside value : ") ) ;
	wxStaticText * staticText2 = new wxStaticText( this, wxID_ANY, _T("Outside value : ") ) ;
	wxStaticText * staticText3 = new wxStaticText( this, wxID_ANY, _T("Lower threshold : ") ) ;
	wxStaticText * staticText4 = new wxStaticText( this, wxID_ANY, _T("Upper threshold : ") ) ;


	textCtrl1 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( insideValue )) ;
	textCtrl2 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( outsideValue )) ;
	textCtrl3 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( lowerThreshold )) ;
	textCtrl4 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( upperThreshold )) ;

	dialogBoxFlexSizer->Add( staticText1, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl1, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText2, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl2, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText3, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl3, 0, wxRIGHT, 10 ) ;

	dialogBoxFlexSizer->Add( staticText4, 0, wxLEFT, 10 ) ;
	dialogBoxFlexSizer->Add( textCtrl4, 0, wxRIGHT, 10 ) ;

	dialogBoxSizerV->Add(dialogBoxFlexSizer) ;

	dialogBoxSizerV->AddSpacer( 30 ) ;
	dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
	dialogBoxSizerV->AddSpacer( 10 );

	this->SetSizer( dialogBoxSizerV ) ;
	dialogBoxSizerV->SetSizeHints( this ) ;

}

//------------------------------------------------------------------------------

ThresholdParameterEditor::~ThresholdParameterEditor(){}


//------------------------------------------------------------------------------

float ThresholdParameterEditor::getOutsideValue()
{
	return outsideValue ;
}

//------------------------------------------------------------------------------

float ThresholdParameterEditor::getInsideValue()
{
	return insideValue ;
}

//------------------------------------------------------------------------------

float ThresholdParameterEditor::getLowerThreshold()
{
	return lowerThreshold ;
}

//------------------------------------------------------------------------------

float ThresholdParameterEditor::getUpperThreshold()
{
	return upperThreshold ;
}

//------------------------------------------------------------------------------

/*void ThresholdParameterEditor::onClickOk( wxCommandEvent & event )
{
	std::cout << "onClickOk" << std::endl ;
	bool error = false ;
	wxString msg = _T("Input values are wrong :") ;
	std::cout << "insideValue" << insideValue << std::endl ;
	std::cout << "outsideValue" << outsideValue << std::endl ;
	std::cout << "lowerThreshold" << lowerThreshold << std::endl ;
	std::cout << "upperThreshold" << upperThreshold << std::endl ;

	if ( ! (outsideValue > insideValue)  )
		{
			msg << _T("\n outsideValue should be greater than insideValue ") ;
			std::cout << "outsideValue should be greater than insideValue" << std::endl ;
			error = true ;
		}
	if ( ! (lowerThreshold < upperThreshold) )
		{
			msg << _T("\n upperThreshold should be greater than lowerThreshold ") ;
			std::cout << "upperThreshold should be greater than lowerThreshold" << std::endl ;
			error = true ;
		}

	if (error)
	{
		std::cout << "Values error" << std::endl ;
		wxMessageDialog(this, msg, _T("Values error"), wxICON_EXCLAMATION) ;
		std::cout << "Values error" << std::endl ;
	}
	else
	{
		std::cout << "No error, endModal(wxID_OK)  = " << wxID_OK << std::endl ;
		EndModal( wxID_OK ) ;
	}


}*/

} // operators
} // fwWX
