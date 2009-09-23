/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_SELECTOR_HPP_
#define FWWX_SELECTOR_HPP_
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <vector>
#include <map>

#include "fwWX/config.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

/**
 * @brief	Selector allowing the choice of an element among severals (_selections)
 * @class	Selector.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 */

class FWWX_CLASS_API Selector :  public wxDialog
{
public:

	enum
	{
		ID_ITEM_SELECTED = wxID_HIGHEST + 1
	};

	/**
	 * @brief Constructor builds a dialog box proposing a string list
	 * @param[in] _parent Pointer to a wxWindow to use as the parent window
	 * @param[in] _title Dialog box title
	 * @param[in] _selections The string list that can be chosen with the dialog box.
	 * The constructor builds a dialog box proposing a string list and translates the string list with a wxWidgets dictionary loaded by the application.
	 */
	FWWX_API Selector( wxWindow * _parent , wxString _title , std::vector< std::string > _selections) ;

	/**
	 * @brief Returns the selection (among those defined in _selections)
	 * @return The selected string
	 */
	FWWX_API std::string getSelectedString() ;

protected :

	/**
	 * @brief Method called on selection modification. Do nothing at this time.
	 */
	FWWX_API void onSelectedItem( wxCommandEvent & event ) ;

	/// Editor Fields
	wxChoice * m_typeCtrl;

	/**
	 * @brief Map to associate each string of the string list with its own translation.
	 * in order to be able to untranslate each string contained in the initial string list
	 */
	::std::map< wxString, wxString > m_translateToUntranslate;

	DECLARE_EVENT_TABLE()
};


}

#endif /*FWWX_SELECTOR_HPP_*/
