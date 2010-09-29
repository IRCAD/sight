/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include "fwWX/validator/LongValidator.hpp"

#include "fwWX/operators/WindowingParameterEditor.hpp"


namespace fwWX
{
namespace operators
{

//------------------------------------------------------------------------------

WindowingParameterEditor::WindowingParameterEditor( wxWindow * parent ) :
    wxDialog( parent, wxID_ANY, _("Windowing parameter editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
    m_iWindowMinimum ( 0 ),
    m_iWindowMaximum ( 0 ),
    m_iOutputMinimum ( 0 ),
    m_iOutputMaximum ( 0 )
{
    createContent();
}

//------------------------------------------------------------------------------

WindowingParameterEditor::~WindowingParameterEditor(){}

//------------------------------------------------------------------------------

void WindowingParameterEditor::createContent()
{

    wxStaticText * pWindowMinimumInfo = new wxStaticText( this, wxID_ANY, _(" Window minimun : ") );
    wxStaticText * pWindowMaximumInfo = new wxStaticText( this, wxID_ANY, _(" Window maximum : ") );
    wxStaticText * pOutputMinimumInfo = new wxStaticText( this, wxID_ANY, _(" Output minimum : ") );
    wxStaticText * pOutputMaximunInfo = new wxStaticText( this, wxID_ANY, _(" Output maximum : ") );


    wxTextCtrl * pWindowMinimum = new wxTextCtrl( this, wxID_ANY );
    wxTextCtrl * pWindowMaximum = new wxTextCtrl( this, wxID_ANY );
    wxTextCtrl * pOutputMinimum = new wxTextCtrl( this, wxID_ANY );
    wxTextCtrl * pOutputMaximum = new wxTextCtrl( this, wxID_ANY );

    pWindowMinimum->SetValidator( ::fwWX::validator::LongValidator( m_iWindowMinimum ) );
    pWindowMaximum->SetValidator( ::fwWX::validator::LongValidator( m_iWindowMaximum ) );
    pOutputMinimum->SetValidator( ::fwWX::validator::LongValidator( m_iOutputMinimum ) );
    pOutputMaximum->SetValidator( ::fwWX::validator::LongValidator( m_iOutputMaximum ) );


    wxFlexGridSizer * sizer = new wxFlexGridSizer( 0, 2, 2, 10 );
    sizer->AddGrowableCol( 1 );

    sizer->Add( pWindowMinimumInfo, 0, wxLEFT, 10 );
    sizer->Add( pWindowMinimum, 1, wxRIGHT, 10 );

    sizer->Add( pWindowMaximumInfo, 0, wxLEFT, 10 );
    sizer->Add( pWindowMaximum, 1, wxRIGHT, 10 );

    sizer->AddSpacer( 2 );
    sizer->AddSpacer( 2 );

    sizer->Add( pOutputMinimumInfo, 0, wxLEFT, 10 );
    sizer->Add( pOutputMinimum, 1, wxRIGHT, 10 );

    sizer->Add( pOutputMaximunInfo, 0, wxLEFT, 10 );
    sizer->Add( pOutputMaximum, 1, wxRIGHT, 10 );

    // Add button
    wxSizer *   finalSizer  ( new wxBoxSizer( wxVERTICAL ) );
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

int WindowingParameterEditor::getWindowMinimun()
{
    return m_iWindowMinimum;
}

//------------------------------------------------------------------------------

int WindowingParameterEditor::getWindowMaximum()
{
    return m_iWindowMaximum;
}

//------------------------------------------------------------------------------

int WindowingParameterEditor::getOutputMinimum()
{
    return m_iOutputMinimum;
}

//------------------------------------------------------------------------------

int WindowingParameterEditor::getOutputMaximum()
{
    return m_iOutputMaximum;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
