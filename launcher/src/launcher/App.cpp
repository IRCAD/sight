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

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/wxprec.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/config.h>

#ifdef __MACOSX__
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/io/ProfileReader.hpp>
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
				::boost::shared_ptr< ::fwRuntime::profile::Profile > profile = ::fwRuntime::io::ProfileReader::createProfile(m_profilePath);
#ifndef TDVPM_COMPLIANT
				m_locale->AddCatalog(wxConvertMB2WX(profile->getName().c_str()), wxLANGUAGE_FRENCH, _T("utf-8"));
				//m_locale->AddCatalog(wxConvertMB2WX(profile->getName().c_str()), wxLANGUAGE_ENGLISH, _T("utf-8"));
#endif
				SetAppName( wxConvertMB2WX(profile->getName().c_str()) );
				profile->start();
			}
			catch( const std::exception & exception )
			{
				throw ::fwRuntime::RuntimeException( std::string(m_profilePath/*.native_file_string()*/ + ": invalid profile file. ") + exception.what() );
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



bool App::OnCmdLineParsed(wxCmdLineParser & parser)
{

	bool parsing;

	parsing = wxApp::OnCmdLineParsed(parser);

	// Retrieves the profile Path
	// 1 argument :
	// launcher path/profile.xml
	if ( parser.GetParamCount() == 1 )
	{
		m_profilePath = wxConvertWX2MB( parser.GetParam() );
	}
	// 3 arguments :
	// launcher path/profile.xml ::fwData::RootObject ConfigName
	else if(parser.GetParamCount() == 3)
	{
		m_profilePath = wxConvertWX2MB( parser.GetParam(0) ) ;
		std::string objectClassName(  wxConvertWX2MB( parser.GetParam(1)) ) ;
		std::string objectConfigurationName( wxConvertWX2MB( parser.GetParam(2)) ) ;

		::fwServices::OSR::setRootObjectClassName( objectClassName ) ;
		::fwServices::OSR::setRootObjectConfigurationName(objectConfigurationName) ;

		// Bundle initialization
		assert( boost::filesystem::exists( m_profilePath ) ) ;
	}
	// 0 argument (or > 3)
	// default profile.xml in current folder
	else
	{
		m_profilePath = "./profile.xml";
	}

	// Profile path is valid ?
	m_profilePathIsValid = boost::filesystem::exists( m_profilePath );

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
	}
	return parsing;
}



void App::OnInitCmdLine(wxCmdLineParser & parser)
{
	wxApp::OnInitCmdLine(parser);
	parser.AddParam(_T("profile.xml"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddParam(_T("object"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddParam(_T("config"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}

} // namespace launcher
