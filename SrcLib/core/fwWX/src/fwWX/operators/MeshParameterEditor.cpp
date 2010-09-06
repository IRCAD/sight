/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <wx/valtext.h>

#include <iostream>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/operators/MeshParameterEditor.hpp"

namespace fwWX
{
namespace operators
{

MeshParameterEditor::MeshParameterEditor( wxWindow * parent ) :
 wxDialog( parent, wxID_ANY, _("Image Mesh"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ),
    m_value ( 0 ),
    m_minVoxelSize(1),
    m_maxVoxelSize(1)
{
    //wxPanel * panel = new wxPanel(this ) ;

    wxBoxSizer * dialogBoxSizerV = new wxBoxSizer( wxVERTICAL ) ;
    wxFlexGridSizer * dialogBoxFlexSizer = new wxFlexGridSizer(3, 2, 2, 20) ;

    wxStaticText * staticText1 = new wxStaticText( this, wxID_ANY, _T("Value : ") ) ;
    wxStaticText * staticText2 = new wxStaticText( this, wxID_ANY, _T("Min voxel size : ") ) ;
    wxStaticText * staticText3 = new wxStaticText( this, wxID_ANY, _T("Max voxel size : ") ) ;

    textCtrl1 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_value )) ;
    textCtrl2 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_minVoxelSize)) ;
    textCtrl3 = new wxTextCtrl( this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0, ::fwWX::validator::LongValidator( m_maxVoxelSize)) ;

    dialogBoxFlexSizer->Add( staticText1, 0, wxLEFT, 10 ) ;
    dialogBoxFlexSizer->Add( textCtrl1, 0, wxRIGHT, 10 ) ;

    dialogBoxFlexSizer->Add( staticText2, 0, wxLEFT, 10 ) ;
    dialogBoxFlexSizer->Add( textCtrl2, 0, wxRIGHT, 10 ) ;

    dialogBoxFlexSizer->Add( staticText3, 0, wxLEFT, 10 ) ;
    dialogBoxFlexSizer->Add( textCtrl3, 0, wxRIGHT, 10 ) ;

    dialogBoxSizerV->AddSpacer( 10 ) ;
    dialogBoxSizerV->Add(dialogBoxFlexSizer) ;

    dialogBoxSizerV->AddSpacer( 20 ) ;
    dialogBoxSizerV->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
    dialogBoxSizerV->AddSpacer( 10 );

    this->SetSizer( dialogBoxSizerV ) ;
    dialogBoxSizerV->SetSizeHints( this ) ;

}

//------------------------------------------------------------------------------

MeshParameterEditor::~MeshParameterEditor(){}


//------------------------------------------------------------------------------

int MeshParameterEditor::getValue()
{
    return (int) m_value ;
}

//------------------------------------------------------------------------------

int MeshParameterEditor::getMinVoxelSize()
{
    return (int) m_minVoxelSize ;
}

//------------------------------------------------------------------------------

int MeshParameterEditor::getMaxVoxelSize()
{
    return (int) m_maxVoxelSize ;
}

//------------------------------------------------------------------------------

} // operators
} // fwWX
