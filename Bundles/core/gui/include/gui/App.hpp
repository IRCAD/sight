/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_APP_HPP_
#define GUI_APP_HPP_

#include <wx/app.h>
#include <wx/intl.h>

#include <fwRuntime/profile/Profile.hpp>

#include "gui/export.hpp"

class WXDLLEXPORT wxLocale;
class wxSingleInstanceChecker;

static void SaveUILanguage(wxLanguage lang);
wxLanguage GetUILanguage();
wxLanguage ChooseLanguage();
void ChangeUILanguage();

namespace gui
{

/**
 * @brief   Defines the wx application.
 * @class   App.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

GUI_CLASS_API class App : public wxApp
{
public:

    /**
     * @brief   Constructor
     */
    App();

    /**
     * @name    Overrides
     */
    /// @{

    bool OnInit();

    int OnExit();

    void usage(const std::string & mes) const;

    bool OnCmdLineParsed(wxCmdLineParser & parser);

    void OnInitCmdLine(wxCmdLineParser & parser);

#ifdef __WXMAC__
    virtual void MacOpenFile (  const wxString & fileName);

    virtual void MacReopenApp (  const wxString & fileName);

    virtual void MacNewFile (  const wxString & fileName);

    void eventMac(const wxString & fileName);
#endif

    /// @}


private:

    /**
     * @brief   locale (internationalization) for app
     */
    wxLocale * m_locale;

    /**
     * @brief   Application Profile
     */
    ::fwRuntime::profile::Profile::sptr m_profile;

    wxSingleInstanceChecker * m_checker;
};


} // namespace gui


#endif /*GUI_APP_HPP_*/
