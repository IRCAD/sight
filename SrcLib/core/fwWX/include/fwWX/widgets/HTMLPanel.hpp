/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef HTMLPANEL_HPP_
#define HTMLPANEL_HPP_

#include <wx/wx.h>
#include <wx/wxhtml.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace widgets
{

/**
 * @brief	This class is used to show an html file or url
 * @class	HTMLPanel.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 * @note 	This class is not commented, because it is not used in the open framework tutorial.
 * @todo 	HTMLPanel is not commented.
 */
class FWWX_CLASS_API HTMLPanel : public wxPanel
{

public:

	FWWX_API HTMLPanel(wxWindow *parent, const std::string& url, const wxSize size = wxDefaultSize);


	FWWX_API void back();
	FWWX_API void forward();


	FWWX_API void OnHtmlLinkClicked(wxHtmlLinkEvent& event);
	FWWX_API void OnHtmlCellHover(wxHtmlCellEvent &event);
	FWWX_API void OnHtmlCellClicked(wxHtmlCellEvent &event);
private:
	wxHtmlWindow * m_html;
	std::string m_url;

    DECLARE_EVENT_TABLE()
};
}
}
#endif /*HTMLPANEL_HPP_*/
