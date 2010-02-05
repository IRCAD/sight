/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/validator/DoubleValidator.hpp"

#include "fwWX/operators/LabelingParameterEditor.hpp"


namespace fwWX
{
namespace operators
{

//------------------------------------------------------------------------------

LabelingParameterEditor::LabelingParameterEditor( wxWindow * parent ) :
        wxDialog( parent, wxID_ANY, _("Labeling parameter editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
        m_iBackground ( 0 ),
        m_dThreshold ( 1.0 )
{
        createContent();
}

//------------------------------------------------------------------------------

LabelingParameterEditor::~LabelingParameterEditor(){}

//------------------------------------------------------------------------------

void LabelingParameterEditor::createContent()
{
        wxStaticText * pBackgroundInfo = new wxStaticText( this, wxID_ANY, _(" Background color : ") );
        wxStaticText * pThresholdInfo = new wxStaticText( this, wxID_ANY, _(" Threshold distance : ") );

        wxTextCtrl * pBackground = new wxTextCtrl( this, wxID_ANY );
        wxTextCtrl * pThreshold = new wxTextCtrl( this, wxID_ANY );

        pBackground->SetValidator( ::fwWX::validator::LongValidator( m_iBackground ) );
        pThreshold->SetValidator( ::fwWX::validator::DoubleValidator( m_dThreshold ) );


        wxFlexGridSizer * sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
        sizer->AddGrowableCol( 1 );

        sizer->Add( pBackgroundInfo, 1, wxLEFT, 10  );
        sizer->Add( pBackground, 1, wxRIGHT, 10);

        sizer->Add( pThresholdInfo, 1, wxLEFT, 10  );
        sizer->Add( pThreshold, 1, wxRIGHT, 10);

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

double LabelingParameterEditor::getThreshold()
{
        return m_dThreshold;
}

//------------------------------------------------------------------------------

int LabelingParameterEditor::getBackground()
{
        return m_iBackground;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
