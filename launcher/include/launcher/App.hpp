/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _LAUNCHER_APP_HPP_
#define _LAUNCHER_APP_HPP_

//------------------------------------------------------------------------------

#ifdef _WIN32 && _DEBUG
#if _MSC_VER >= 1400 // visual studio >= 2005

// add a dependency on the retail crt even in debug

#ifdef _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' "           \
        "name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".CRT' "             \
        "version='" _CRT_ASSEMBLY_VERSION "' "                         \
        "processorArchitecture='x86' "                                 \
        "publicKeyToken='" _VC_ASSEMBLY_PUBLICKEYTOKEN "'\"")
#endif    /* _M_IX86 */

#ifdef _M_AMD64
#pragma comment(linker,"/manifestdependency:\"type='win32' "           \
        "name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".CRT' "             \
        "version='" _CRT_ASSEMBLY_VERSION "' "                         \
        "processorArchitecture='amd64' "                               \
        "publicKeyToken='" _VC_ASSEMBLY_PUBLICKEYTOKEN "'\"")
#endif    /* _M_AMD64 */

#ifdef _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' "           \
        "name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".CRT' "             \
        "version='" _CRT_ASSEMBLY_VERSION "' "                         \
        "processorArchitecture='ia64' "                                \
        "publicKeyToken='" _VC_ASSEMBLY_PUBLICKEYTOKEN "'\"")
#endif    /* _M_IA64 */

#endif    /* _MSC_VER >= 1400 */
#endif    /* _WIN32 && _DEBUG */

//------------------------------------------------------------------------------

#include <string>
#include <wx/app.h>
#include <wx/intl.h>


class WXDLLEXPORT wxLocale;

static void SaveUILanguage(wxLanguage lang);
wxLanguage GetUILanguage();
wxLanguage ChooseLanguage();
void ChangeUILanguage();

namespace launcher
{


/**
 * @brief	Defines the vpm application object.
 *
 * @author	IRCAD (Research and Development Team).
 */
struct App : public wxApp
{

	/**
	 * @brief	Constructor
	 */
	App();

	/**
	 * @name	Overrides
	 */
	/// @{

	bool OnInit();

	void usage(const std::string & mes) const;

	bool OnCmdLineParsed(wxCmdLineParser & parser);

	void OnInitCmdLine(wxCmdLineParser & parser);
	
	//wxLanguage GetUILanguage();

	/// @}


private:

	/**
	 * @brief	a string containing the path to the XML profile
	 *
	 * The path to the XML profile will be initialized once the
	 * command line is parsed.
	 */
	std::string m_profilePath;

	/**
	 * @brief	a bool to know if the profile path is valid
	 */
	bool m_profilePathIsValid;

	/**
	 * @brief	locale (internationalization) for app
	 */
	wxLocale * m_locale;
};


} // namespace vpm


#endif // #define _LAUNCHER_APP_HPP_
