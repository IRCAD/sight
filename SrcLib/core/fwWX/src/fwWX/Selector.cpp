/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include <wx/sizer.h>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwWX/convert.hpp>

#include "fwWX/Selector.hpp"

REGISTER_BINDING( ::fwGui::ISelector, ::fwWX::Selector, ::fwGui::ISelector::FactoryRegistryKeyType , ::fwGui::ISelector::REGISTRY_KEY );

namespace fwWX
{

//------------------------------------------------------------------------------

Selector::Selector() : m_title("")
{}

//------------------------------------------------------------------------------

Selector::~Selector()
{}

//------------------------------------------------------------------------------

void Selector::setSelections(std::vector< std::string > _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void Selector::setTitle(std::string _title)
{
    this->m_title = _title;
}

//------------------------------------------------------------------------------

std::string Selector::show()
{
    wxDialog* dialog = new wxDialog( wxTheApp->GetTopWindow(), wxNewId(), ::fwWX::std2wx(this->m_title),
            wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX );

    wxArrayString items;
    BOOST_FOREACH( std::string selection, m_selections)
    {
        items.Add( ::fwWX::std2wx(selection) );
    }

    // Creates the static fields.
    wxChoice* typeCtrl = new wxChoice(dialog, wxNewId(), wxDefaultPosition, wxDefaultSize, items) ;
    typeCtrl->SetSelection(0);

    // Creates the default buttons.
    wxSizer  * defaultButtonSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton * okButton = new wxButton( dialog, wxID_OK, _("OK") );
    wxButton * cancelButton = new wxButton( dialog, wxID_CANCEL, _("Cancel") );

    okButton->SetDefault();
    defaultButtonSizer->Add( okButton, 0, 0 );
    defaultButtonSizer->Add( 5, 5 );
    defaultButtonSizer->Add( cancelButton, 0, 0 );

    // Creates the root sizer.
    wxSizer * rootSizer = new wxBoxSizer( wxVERTICAL );
    rootSizer->Add( typeCtrl, 0, wxGROW|wxALL, 10 );
    rootSizer->Add( defaultButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
    dialog->SetSizerAndFit( rootSizer );


    int choice = dialog->ShowModal();
    std::string selection = "";
    if( choice == wxID_OK )
    {
        selection = typeCtrl->GetStringSelection();
    }

    return selection;
}

//------------------------------------------------------------------------------

} // namespace fwWX


