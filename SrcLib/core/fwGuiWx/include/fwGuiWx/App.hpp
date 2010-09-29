/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_APP_HPP_
#define FWGUIWX_APP_HPP_

#include <wx/app.h>
#include <wx/intl.h>

#include <fwRuntime/profile/Profile.hpp>

#include "fwGuiWx/config.hpp"

class WXDLLEXPORT wxLocale;
class wxSingleInstanceChecker;

static void SaveUILanguage(wxLanguage lang);
wxLanguage GetUILanguage();
wxLanguage ChooseLanguage();
void ChangeUILanguage();

namespace fwGuiWx
{

/**
 * @brief   Defines the wx application.
 * @class   App.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

class FWGUIWX_CLASS_API App : public wxApp
{
public:

    /**
     * @brief   Constructor
     */
    FWGUIWX_API App();

    /**
     * @name    Overrides
     */
    /// @{

    FWGUIWX_API bool OnInit();

    FWGUIWX_API int OnExit();

    FWGUIWX_API void usage(const std::string & mes) const;

    FWGUIWX_API bool OnCmdLineParsed(wxCmdLineParser & parser);

    FWGUIWX_API void OnInitCmdLine(wxCmdLineParser & parser);

#ifdef __WXMAC__
    FWGUIWX_API virtual void MacOpenFile (  const wxString & fileName);

    FWGUIWX_API virtual void MacReopenApp (  const wxString & fileName);

    FWGUIWX_API virtual void MacNewFile (  const wxString & fileName);

    FWGUIWX_API void eventMac(const wxString & fileName);
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


} // namespace fwGuiWx


#endif /*FWGUIWX_APP_HPP_*/
