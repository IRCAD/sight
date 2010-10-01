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
#include <wx/stattext.h>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwWX/convert.hpp>

#include "fwGuiWx/dialog/MultiSelectorDialog.hpp"

REGISTER_BINDING( ::fwGui::dialog::IMultiSelectorDialog, ::fwGuiWx::dialog::MultiSelectorDialog, ::fwGui::dialog::IMultiSelectorDialog::FactoryRegistryKeyType , ::fwGui::dialog::IMultiSelectorDialog::REGISTRY_KEY );

namespace fwGuiWx
{
namespace dialog
{

//------------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog() : m_title(""), m_message("")
{}

//------------------------------------------------------------------------------

MultiSelectorDialog::~MultiSelectorDialog()
{}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string _title)
{
    this->m_title = _title;
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    wxDialog* dialog = new wxDialog( wxTheApp->GetTopWindow(), wxNewId(), ::fwWX::std2wx(this->m_title),
            wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX );

    wxArrayString items;
    BOOST_FOREACH( Selections::value_type selection, m_selections)
    {
        items.Add( ::fwWX::std2wx(selection.first) );
    }

    wxCheckListBox* typeCtrl = new wxCheckListBox(dialog, wxNewId(), wxDefaultPosition, wxDefaultSize, items) ;
    typeCtrl->SetSelection(0);

    int item=0;
    BOOST_FOREACH( Selections::value_type selection, m_selections)
    {
        typeCtrl->Check( item, selection.second  );
        item++;
    }

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
    if(!m_message.empty())
    {
        wxStaticText* msgText = new wxStaticText(dialog, wxNewId(), ::fwWX::std2wx(m_message));
        rootSizer->Add( msgText, 0, wxGROW|wxALL, 10 );
    }
    rootSizer->Add( typeCtrl, 1, wxALL|wxEXPAND, 10 );
    rootSizer->Add( defaultButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
    dialog->SetSizerAndFit( rootSizer );


    int choice = dialog->ShowModal();
    Selections selections;
    if( choice == wxID_OK )
    {
        int item=0;
        BOOST_FOREACH( Selections::value_type selection, m_selections)
        {
            selections[selection.first] = typeCtrl->IsChecked(item);
            item++;
         }
    }
    dialog->Destroy();
    return selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string &msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiWx


