/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>
#include <iostream>
#include <sstream>
#include <locale.h>

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/wxprec.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/config.h>
#include <wx/filefn.h>
#include <wx/snglinst.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#ifdef __MACOSX__
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <fwTools/Os.hpp>

#include <fwRuntime/RuntimeException.hpp>

#include <fwGuiWx/convert.hpp>
#include <fwGuiWx/LoggerInitializer.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "fwGuiWx/App.hpp"

namespace fwGuiWx
{

//-----------------------------------------------------------------------------

App::App()
{
    SetAppName( wxGetTranslation("launcher") );
#ifdef __MACOSX__
    ProcessSerialNumber PSN;
    GetCurrentProcess(&PSN);
    TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
#endif
}

//-----------------------------------------------------------------------------

void App::usage( const std::string & mes ) const
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Exception Caught");
    messageBox.setMessage( mes );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//-----------------------------------------------------------------------------

bool App::OnInit()
{
    ::fwGuiWx::LoggerInitializer::initialize();

    m_profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", m_profile);

#ifndef TDVPM_COMPLIANT
    m_locale = new wxLocale();
    ::boost::filesystem::path pathLauncher;
    pathLauncher = ::boost::filesystem::current_path() / "share" / "launcher_0-1" / "locale" ;

    wxLocale::AddCatalogLookupPathPrefix( ::fwGuiWx::std2wx( pathLauncher.string() ));
    m_locale->Init(GetUILanguage());
    m_locale->AddCatalog(_T("launcher"));
    m_locale->AddCatalog(_T("runtime"));
    // Set the locale to C for all number of all application
    setlocale(LC_NUMERIC,"C");
#else
    setlocale(LC_ALL,"C");
#endif

    wxApp::OnInit();

    std::string appName = m_profile->getName();
#ifndef TDVPM_COMPLIANT
    m_locale->AddCatalog(::fwGuiWx::std2wx(appName), wxLANGUAGE_FRENCH, _T("utf-8"));
#endif
    SetAppName( ::fwGuiWx::std2wx(appName) );

    std::string checkerPath = ::fwTools::os::getUserDataDir("IRCAD", appName, true);

    if (checkerPath.empty())
    {
        checkerPath = ::fwTools::os::getUserDataDir("IRCAD", "", true);
    }
    if (checkerPath.empty())
    {
        checkerPath = ::fwTools::os::getUserDataDir("", "", true);
    }
    SLM_ASSERT("Unable to find user's data dir.", !checkerPath.empty());

    m_checker = new wxSingleInstanceChecker();
    if (m_profile->getCheckSingleInstance())
    {
        m_checker->Create( ::fwGuiWx::std2wx(appName) + wxGetTranslation(".pid"), ::fwGuiWx::std2wx(checkerPath));
        if ( m_checker->IsAnotherRunning() )
        {
            wxLogError(wxGetTranslation("Another " + ::fwGuiWx::std2wx(appName) + wxGetTranslation(" instance is already running, aborting.")));
            return false;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------

int App::OnExit()
{
    SLM_TRACE_FUNC();

    delete m_checker;
    return 0;
}

//-----------------------------------------------------------------------------

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
        { wxCMD_LINE_OPTION, "p", "profile", "path to the profile to launch",wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_OPTION, "s", "substitute", "substitute parameters : name1@value1@name2@value2...", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_PARAM, NULL, NULL, "old style parameters", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_NONE }
};

//-----------------------------------------------------------------------------

void App::OnInitCmdLine(wxCmdLineParser & parser)
{
    wxApp::OnInitCmdLine(parser);
    parser.SetDesc(cmdLineDesc);
}

//-----------------------------------------------------------------------------

void App::OnUnhandledException()
{
    // we're called from an exception handler so we can re-throw the exception
    // to recover its type
    std::string what;
    try
    {
        throw;
    }
    catch ( std::exception& e )
    {
        what = e.what();
    }
    catch ( ... )
    {
        what = "unknown exception";
    }

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Exception Caught");
    messageBox.setMessage( what );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();

    throw ::fwRuntime::RuntimeException( what );
}

//-----------------------------------------------------------------------------

#ifdef __WXMAC__
void App::MacOpenFile (  const wxString & fileName)
{
    SLM_TRACE("MacOpenFile");
    eventMac(fileName);
}

//-----------------------------------------------------------------------------

void App::MacNewFile (  const wxString & fileName)
{
    SLM_TRACE("MacNewFile");
    eventMac(fileName);
}

//-----------------------------------------------------------------------------

void App::MacReopenApp (  const wxString & fileName)
{
    SLM_TRACE("MacReopenApp");
    eventMac(fileName);
}

//-----------------------------------------------------------------------------

void App::eventMac(const wxString & fileName)
{
    wxCommandEvent tEvent(wxEventFwOpen, wxIDEventFwOpen);
    tEvent.SetString( fileName );
    tEvent.SetEventObject( this );
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    if (frame != NULL)
        frame->GetEventHandler()->ProcessEvent( tEvent );
    else
        SLM_FATAL ("Window not found !");
}

#endif

} // namespace fwGuiWx


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if NEED_CHOOSELANG_UI
static void SaveUILanguage(wxLanguage lang)
{
    if (lang == wxLANGUAGE_UNKNOWN)
        return;
    if (lang == wxLANGUAGE_DEFAULT)
        wxConfig::Get()->Write(_T("ui_language"), _T("default"));
    else
        wxConfig::Get()->Write(_T("ui_language"),
                wxLocale::GetLanguageInfo(lang)->CanonicalName);
}
#endif // NEED_CHOOSELANG_UI

//-----------------------------------------------------------------------------

wxLanguage GetUILanguage()
{
#if !NEED_CHOOSELANG_UI
    return wxLANGUAGE_DEFAULT;
#else
    wxLanguage lang(wxLANGUAGE_DEFAULT);
    wxString lng = wxConfig::Get()->Read(_T("ui_language"));
    if (lng.empty())
    {
        lang = ChooseLanguage();
        if (lang != wxLANGUAGE_UNKNOWN)
            SaveUILanguage(lang);
        else
            lang = wxLANGUAGE_DEFAULT;
    }
    else if (lng != _T("default"))
    {
        const wxLanguageInfo *info = wxLocale::FindLanguageInfo(lng);
        if (info != NULL)
            lang = (wxLanguage)info->Language;
        else
            wxLogError(wxGetTranslation("Uknown locale code '%s' in registry."), lng.c_str());
    }
    return lang;
#endif // NEED_CHOOSELANG_UI
}

//-----------------------------------------------------------------------------

#if NEED_CHOOSELANG_UI
wxLanguage ChooseLanguage()
{
    struct LangInfo
    {
        const wxChar *name;
        wxLanguage code;
    };

    LangInfo langs[] =
    {
            { wxGetTranslation("(Use default language)"), wxLANGUAGE_DEFAULT },

            { _T("English"), wxLANGUAGE_ENGLISH },
            { _T("French"), wxLANGUAGE_FRENCH },

            { NULL, wxLANGUAGE_UNKNOWN }
    };

    wxArrayString arr;
    for (int i = 0; langs[i].name; i++)
        arr.Add(langs[i].name);

    int choice = wxGetSingleChoiceIndex(
            wxGetTranslation("Select your prefered language"),
            wxGetTranslation("Language selection"),
            arr);
    if (choice == -1)
        return wxLANGUAGE_UNKNOWN;
    else
        return langs[choice].code;
}
#endif // NEED_CHOOSELANG_UI

