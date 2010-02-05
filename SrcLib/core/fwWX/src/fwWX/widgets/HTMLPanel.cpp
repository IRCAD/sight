/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/fs_inet.h>

#include <fwCore/base.hpp>
#include <boost/filesystem/operations.hpp>

#include "fwWX/widgets/HTMLPanel.hpp"

namespace fwWX
{

namespace widgets
{

//------------------------------------------------------------------------------

HTMLPanel::HTMLPanel(wxWindow *parent, const std::string& _url, const wxSize size) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
{
#ifdef _WIN32
	//TODO fixme explicit reload HTML parser for win
	wxInitialize();
#endif
	wxFileSystem::AddHandler(new wxInternetFSHandler);

	// File
	::boost::filesystem::path htmlPath (_url);
	wxString file ( wxConvertMB2WX(htmlPath.string().c_str() ) );

	// wxHtmlWindow
	m_html = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, size, wxHW_SCROLLBAR_NEVER);
	m_html->SetBorders(10);
	m_html->LoadPage(file);
	m_html->SetSize(m_html->GetInternalRepresentation()->GetWidth(),
				m_html->GetInternalRepresentation()->GetHeight());

}

HTMLPanel::HTMLPanel(wxWindow *parent, const wxString& _html, const wxSize size) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
{
#ifdef _WIN32
    //TODO fixme explicit reload HTML parser for win
    wxInitialize();
#endif
    wxFileSystem::AddHandler(new wxInternetFSHandler);

    // wxHtmlWindow
    m_html = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, size, wxHW_SCROLLBAR_NEVER);
    m_html->SetBorders(10);
    m_html->SetPage(_html);
    m_html->SetSize(m_html->GetInternalRepresentation()->GetWidth(),
                m_html->GetInternalRepresentation()->GetHeight());

}

void HTMLPanel::back()
{
    if (!m_html->HistoryBack())
    {
    	SLM_TRACE("You reached prehistory era");
    }
}

void HTMLPanel::forward()
{
    if (!m_html->HistoryForward())
    {
        SLM_TRACE("No more items in history");
    }
}

void HTMLPanel::OnHtmlLinkClicked(wxHtmlLinkEvent &event)
{
	//OSLM_TRACE("The url "<< event.GetLinkInfo().GetHref().c_str() <<" has been clicked");
	wxString url ( wxConvertMB2WX(event.GetLinkInfo().GetHref().c_str() ) );
    // skipping this event the default behaviour (load the clicked URL)
    // will happen...
	wxLaunchDefaultBrowser(url);
//    event.Skip();
}

void HTMLPanel::OnHtmlCellHover(wxHtmlCellEvent &event)
{
	OSLM_TRACE("Mouse moved over cell " << event.GetCell()
			<< "-" << event.GetPoint().x
			<< "-" << event.GetPoint().y);
}

void HTMLPanel::OnHtmlCellClicked(wxHtmlCellEvent &event)
{
	OSLM_TRACE("Click over cell " << event.GetCell()
    		<< "-" << event.GetPoint().x
    		<< "-" << event.GetPoint().y);

    // if we don't skip the event, OnHtmlLinkClicked won't be called!
    event.Skip();
}
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(HTMLPanel, wxPanel)
    EVT_HTML_LINK_CLICKED(wxID_ANY, HTMLPanel::OnHtmlLinkClicked)
    EVT_HTML_CELL_HOVER(wxID_ANY, HTMLPanel::OnHtmlCellHover)
    EVT_HTML_CELL_CLICKED(wxID_ANY, HTMLPanel::OnHtmlCellClicked)
END_EVENT_TABLE()

//------------------------------------------------------------------------------

} // widgets
} // fwWX
