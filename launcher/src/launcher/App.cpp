/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>
#include <iostream>
#include <sstream>
#include <locale.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/wxprec.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/config.h>
#include <wx/filefn.h>
#include <wx/snglinst.h>

#ifdef __MACOSX__
#include <ApplicationServices/ApplicationServices.h>
#include <fwWX/convert.hpp>
#endif

#include <fwTools/Os.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/io/ProfileReader.hpp>
#include <fwRuntime/io/XMLSubstitute.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "launcher/App.hpp"

/**
 * @brief	Creates the application instance.
 */
IMPLEMENT_APP(launcher::App);


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
            wxLogError(_("Uknown locale code '%s' in registry."), lng.c_str());
    }
    return lang;
#endif // NEED_CHOOSELANG_UI
}

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
        { _("(Use default language)"), wxLANGUAGE_DEFAULT },

        { _T("English"), wxLANGUAGE_ENGLISH },
        { _T("French"), wxLANGUAGE_FRENCH },

        { NULL, wxLANGUAGE_UNKNOWN }
    };

    wxArrayString arr;
    for (int i = 0; langs[i].name; i++)
        arr.Add(langs[i].name);

    int choice = wxGetSingleChoiceIndex(
            _("Select your prefered language"),
            _("Language selection"),
            arr);
    if (choice == -1)
        return wxLANGUAGE_UNKNOWN;
    else
        return langs[choice].code;
}
/*
void ChangeUILanguage()
{
    wxLanguage lang = ChooseLanguage();
    if (lang == wxLANGUAGE_UNKNOWN)
        return;
    SaveUILanguage(lang);
    wxMessageBox(_("You must restart launcher for this change to take effect."),
                 _T("launcher"),
                 wxOK | wxCENTRE | wxICON_INFORMATION);
}
*/
#endif // NEED_CHOOSELANG_UI

namespace launcher
{


App::App()
{
	SetAppName( _("launcher") );
#ifdef __MACOSX__
	ProcessSerialNumber PSN;
	GetCurrentProcess(&PSN);
	TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
#endif
}


void App::usage( const std::string & mes ) const
{
	wxMessageBox(wxConvertMB2WX(mes.c_str()),
					_("Exception Caught"),
					wxOK|wxICON_ERROR);
}

bool App::OnInit()
{
	try
	{
#ifndef TDVPM_COMPLIANT
		m_locale = new wxLocale();
		std::string pathLauncher("./share/launcher_");
		wxLocale::AddCatalogLookupPathPrefix(wxConvertMB2WX( ( ( pathLauncher + LAUNCHER_VER ) + "/locale").c_str() ) );
		//m_locale->Init(wxLocale::GetSystemLanguage());
		m_locale->Init(GetUILanguage());
		m_locale->AddCatalog(_T("launcher"));
		m_locale->AddCatalog(_T("runtime"));
		// Set the locale to C for all number of all application
		setlocale(LC_NUMERIC,"C");
#else
		setlocale(LC_ALL,"C");
#endif

		wxApp::OnInit();

		if( m_profilePathIsValid )
		{
			try
			{
				::fwRuntime::addBundles("./Bundles");
				m_profile = ::fwRuntime::io::ProfileReader::createProfile(m_profilePath);
				OSLM_INFO("Launcher -- m_profile: " << m_profile);
				::fwRuntime::profile::setCurrentProfile(m_profile);

                std::string appName = m_profile->getName();
#ifndef TDVPM_COMPLIANT
				m_locale->AddCatalog(wxConvertMB2WX(appName.c_str()), wxLANGUAGE_FRENCH, _T("utf-8"));
				//m_locale->AddCatalog(wxConvertMB2WX(appName.c_str()), wxLANGUAGE_ENGLISH, _T("utf-8"));
#endif
				SetAppName( wxConvertMB2WX(appName.c_str()) );


                std::string checkerPath = ::fwTools::os::getUserDataDir("IRCAD", appName, true);

                if (checkerPath.empty())
                {
                    checkerPath = ::fwTools::os::getUserDataDir("IRCAD", "", true);
                }
                if (checkerPath.empty())
                {
                    checkerPath = ::fwTools::os::getUserDataDir("", "", true);
                }
                SLM_ASSERT("Unable to find user's data dir.", !checkerPath.empty())

                m_checker = new wxSingleInstanceChecker();
                if (m_profile->getCheckSingleInstance())
                {
                    m_checker->Create(appName + ".pid", checkerPath);
                    if ( m_checker->IsAnotherRunning() )
                    {
                        wxLogError(_("Another " +appName+ " instance is already running, aborting."));
                        return false;
                    }
                }


				m_profile->start();
			}
			catch( const std::exception & exception )
			{
				throw ::fwRuntime::RuntimeException( std::string(m_profilePath + ": invalid profile file. ") + exception.what() );
			}
			return true;
		}
		else
		{
			return false;
		}

	}
	catch(const std::exception & exception)
	{
		wxMessageBox(wxConvertMB2WX(exception.what()),
					 _("Exception Caught"),
					 wxOK|wxICON_ERROR);
		return false;
	}
}

int App::OnExit()
{
    m_profile->stop();
    delete m_checker;
    return 0;
}



bool App::OnCmdLineParsed(wxCmdLineParser & parser)
{
	bool parsing;

	parsing = wxApp::OnCmdLineParsed(parser);

	// Retrieves the substitute parameters
	wxString value;
	parser.Found("s", &value);
	if(!value.IsEmpty())
	{
	    std::string str ( wxConvertWX2MB(value) );
	    typedef ::boost::tokenizer< ::boost::char_separator<char> >   tokenizer;
	    ::boost::char_separator<char> sep("@");
	    tokenizer tokens(str, sep);
	    assert ( std::distance (tokens.begin(),tokens.end())%2 == 0 );
	    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	    {
	        std::string key = *tok_iter;
	        std::string val = (*++tok_iter);
	        ::fwRuntime::io::XMLSubstitute::getDefault()->getRefDictionary()[key]= val ;
	        std::cout << "token : "<< key << " - " << val << std::endl;
	    }
	}

	::boost::filesystem::path launcherPath((const char*)(wxPathOnly(wxTheApp->argv[0])).mb_str(wxConvUTF8));
	if(!exists(launcherPath/"Bundles"))
	{
		// Are-you in /bin directory?
		launcherPath = launcherPath.parent_path();
		SLM_FATAL_IF("Bundles directory can't be found", !exists(launcherPath/"Bundles"));
	}
	wxSetWorkingDirectory (wxString( launcherPath.string().c_str() , wxConvUTF8 ));

	// Retrieves the profile Path
	//	m_profilePath = "./profile.xml"; // default value
	m_profilePath = ::boost::filesystem::current_path().string()+"/profile.xml";
	value.Clear();
	parser.Found("p", &value);
	if(!value.IsEmpty())
	{
	    m_profilePath = wxConvertWX2MB(value);
	}
	else
	{
	    // old style parameters
        // 1 argument :
        // launcher path/profile.xml
        if ( parser.GetParamCount() == 1 )
        {
            m_profilePath = wxConvertWX2MB( parser.GetParam() );
        }
	}


	// Profile path is valid ?
	m_profilePathIsValid = ::boost::filesystem::exists( m_profilePath );

	// Print a message if the path is not valid
	if ( ! m_profilePathIsValid )
	{
		const wxString msgPart1 = wxT("A valid profile is required to start the application. The file ");
		const wxString msgPart3 = wxT(" does not exist.");

		std::stringstream mes;
		mes << std::string(msgPart1.mb_str());
		mes << m_profilePath;
		mes << std::string(msgPart3.mb_str());

		usage(mes.str());
		parsing = false;
	}
	return parsing;
}

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_OPTION, "p", "profile", "path to the profile to launch",wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "s", "substitute", "substitute parameters : name1@value1@name2@value2...", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM, NULL, NULL, "old style parameters", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};


void App::OnInitCmdLine(wxCmdLineParser & parser)
{
	wxApp::OnInitCmdLine(parser);
	parser.SetDesc(cmdLineDesc);
}

#ifdef __WXMAC__

void App::MacOpenFile (  const wxString & fileName)
{
    SLM_TRACE("MacOpenFile");
    eventMac(fileName);
}

void App::MacNewFile (  const wxString & fileName)
{
    SLM_TRACE("MacNewFile");
    eventMac(fileName);
}

void App::MacReopenApp (  const wxString & fileName)
{
    SLM_TRACE("MacReopenApp");
    eventMac(fileName);
}

void App::eventMac(const wxString & fileName)
{
    wxCommandEvent tEvent(wxEventFwOpen, wxIDEventFwOpen);
    tEvent.SetString( fileName );
    tEvent.SetEventObject( this );
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    if (frame != NULL)
        frame->GetEventHandler()->ProcessEvent( tEvent );
    else
        SLM_FATAL ("Window not found !")
}

#endif

} // namespace launcher
