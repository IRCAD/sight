/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/validator/DoubleValidator.hpp"

#include "fwWX/operators/AnisotropicFilterParameterEditor.hpp"


namespace fwWX
{
namespace operators
{

//------------------------------------------------------------------------------

AnisotropicFilterParameterEditor::AnisotropicFilterParameterEditor( wxWindow * parent ) :
        wxDialog( parent, wxID_ANY, _("Anisotropic parameter editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
        m_dTimeStep ( 0.05 ),
        m_iNbIterations ( 5 ),
        m_dConductance ( 25 )
{
        createContent();
}

//------------------------------------------------------------------------------

AnisotropicFilterParameterEditor::~AnisotropicFilterParameterEditor(){}

//------------------------------------------------------------------------------

void AnisotropicFilterParameterEditor::createContent()
{

        wxStaticText * pTimeStepInfo = new wxStaticText( this, wxID_ANY, _(" Time step : ") );
        wxStaticText * pNbIterationsInfo = new wxStaticText( this, wxID_ANY, _(" Number of iterations : ") );
        wxStaticText * pConductanceInfo = new wxStaticText( this, wxID_ANY, _(" Coefficient diffusion : ") );


        wxTextCtrl * pTimeStep = new wxTextCtrl( this, wxID_ANY );
        wxTextCtrl * pNbIterations = new wxTextCtrl( this, wxID_ANY );
        wxTextCtrl * pConductance = new wxTextCtrl( this, wxID_ANY );

        pTimeStep->SetValidator( ::fwWX::validator::DoubleValidator( m_dTimeStep, 0.001, 1.0 ) );
        pNbIterations->SetValidator( ::fwWX::validator::LongValidator( m_iNbIterations, 0, 100 ) );
        pConductance->SetValidator( ::fwWX::validator::DoubleValidator( m_dConductance , 1.0, 100.0 ) );


        wxFlexGridSizer * sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
        sizer->AddGrowableCol( 1 );

        sizer->Add( pTimeStepInfo, 0, wxLEFT, 10 );
        sizer->Add( pTimeStep, 1, wxRIGHT, 10 );

        sizer->Add( pNbIterationsInfo, 0, wxLEFT, 10 );
        sizer->Add( pNbIterations, 1, wxRIGHT, 10 );

        sizer->Add( pConductanceInfo, 0, wxLEFT, 10 );
        sizer->Add( pConductance, 1, wxRIGHT, 10 );

        // Add button
        wxSizer *       finalSizer      ( new wxBoxSizer( wxVERTICAL ) );
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

double AnisotropicFilterParameterEditor::getTimeStep()
{
        return m_dTimeStep;
}

//------------------------------------------------------------------------------

int AnisotropicFilterParameterEditor::getNbIterations()
{
        return m_iNbIterations;
}

//------------------------------------------------------------------------------

double AnisotropicFilterParameterEditor::getConductance()
{
        return m_dConductance;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
