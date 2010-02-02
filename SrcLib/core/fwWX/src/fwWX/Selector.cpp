/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>
#include <fwCore/base.hpp>

#include "fwWX/Selector.hpp"
#include "fwWX/convert.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( Selector, wxDialog )
EVT_CHOICE( Selector::ID_ITEM_SELECTED, Selector::onSelectedItem )
END_EVENT_TABLE()

//------------------------------------------------------------------------------

Selector::Selector( wxWindow * parent , wxString _title , std::vector< std::string > _selections) :
	wxDialog	( parent, -1, _title, wxDefaultPosition,
			wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX)
			{

	wxArrayString items;
	for( std::vector< std::string >::iterator iter = _selections.begin() ; iter != _selections.end() ; ++iter )
	{
//		wxString name = wxConvertMB2WX( iter->c_str() );
		wxString name = std2wx(*iter);
//		wxString translatedName = wxGetTranslation(name);
		wxString translatedName = name;
		items.Add( translatedName );
		m_translateToUntranslate[translatedName] = name;
	}

	// Creates the static fields.
	//m_typeCtrl = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items) ;
	m_typeCtrl = new wxChoice(this, ID_ITEM_SELECTED, wxDefaultPosition, wxDefaultSize, items) ;
	m_typeCtrl->SetSelection(0);


	// Creates the default buttons.
	wxSizer		* defaultButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	wxButton	* okButton = new wxButton( this, wxID_OK, _("OK") );
	wxButton	* cancelButton = new wxButton( this, wxID_CANCEL, _("Cancel") );

	okButton->SetDefault();
	defaultButtonSizer->Add( okButton, 0, 0 );
	defaultButtonSizer->Add( 5, 5 );
	defaultButtonSizer->Add( cancelButton, 0, 0 );

	// Creates the root sizer.
	wxSizer	* rootSizer = new wxBoxSizer( wxVERTICAL );
	rootSizer->Add( m_typeCtrl, 0, wxGROW|wxALL, 10 );
	rootSizer->Add( defaultButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
	SetSizerAndFit( rootSizer );
			}

//------------------------------------------------------------------------------

std::string Selector::getSelectedString()
{
	return  wx2std(m_translateToUntranslate[m_typeCtrl->GetStringSelection()]);
}

//------------------------------------------------------------------------------

void Selector::onSelectedItem( wxCommandEvent & event )
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

}


