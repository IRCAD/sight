/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

//#ifdef __MACOSX__
//#include <ApplicationServices/ApplicationServices.h>
//#endif

#include <fwTools/Os.hpp>

#include <fwRuntime/io/XMLSubstitute.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/bundle/runtime.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "fwGuiQt/App.hpp"

namespace fwGuiQt
{

//-----------------------------------------------------------------------------

App::App(int & argc, char ** argv)
    : QApplication (argc, argv)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);

    //setlocale(LC_ALL,"C");

    std::string appName = profile->getName();

    this->setApplicationName( QString::fromStdString(appName) );

    //std::string checkerPath = ::fwTools::os::getUserDataDir("IRCAD", appName, true);

    //if (checkerPath.empty())
    //{
        //checkerPath = ::fwTools::os::getUserDataDir("IRCAD", "", true);
    //}
    //if (checkerPath.empty())
    //{
        //checkerPath = ::fwTools::os::getUserDataDir("", "", true);
    //}
    //SLM_ASSERT("Unable to find user's data dir.", !checkerPath.empty());

    //m_checker = new wxSingleInstanceChecker();
    //if (profile->getCheckSingleInstance())
    //{
        //m_checker->Create( ::fwWX::std2wx(appName) + _(".pid"), ::fwWX::std2wx(checkerPath));
        //if ( m_checker->IsAnotherRunning() )
        //{
            //wxLogError(_("Another " + ::fwWX::std2wx(appName) + _(" instance is already running, aborting.")));
            //return false;
        //}
    //}

    QObject::connect(this, SIGNAL(lastWindowClosed()), this, SLOT(onExit()));
    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(onExit()));

//#ifdef __MACOSX__
    //ProcessSerialNumber PSN;
    //GetCurrentProcess(&PSN);
    //TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
//#endif

    // Initialize root object : root object, views, ...
    ::fwServices::OSR::initializeRootObject();
}

//-----------------------------------------------------------------------------

void App::onExit()
{
    SLM_TRACE_FUNC();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ::fwServices::OSR::uninitializeRootObject();
    QApplication::restoreOverrideCursor();

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_TRACE("Stopping Profile");
    profile->stop();
    SLM_TRACE("Profile Stopped");

    qApp->flush();
    qApp->exit(0);
}

//-----------------------------------------------------------------------------

//void App::usage( const std::string & mes ) const
//{
    //::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
    //::fwGui::dialog::MessageDialog messageBox;
    //messageBox.setTitle("Exception Caught");
    //messageBox.setMessage( mes );
    //messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    //messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    //messageBox.show();
//}

//-----------------------------------------------------------------------------

//bool App::OnCmdLineParsed(wxCmdLineParser & parser)
//{
    //bool parsing;

    //parsing = wxApp::OnCmdLineParsed(parser);

    //// Retrieves the substitute parameters
    //wxString value;
    //parser.Found("s", &value);
    //if(!value.IsEmpty())
    //{
        //std::string str ( ::fwWX::wx2std(value) );
        //typedef ::boost::tokenizer< ::boost::char_separator<char> >   tokenizer;
        //::boost::char_separator<char> sep("@");
        //tokenizer tokens(str, sep);
        //assert ( std::distance (tokens.begin(),tokens.end())%2 == 0 );
        //for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        //{
            //std::string key = *tok_iter;
            //std::string val = (*++tok_iter);
            //::fwRuntime::io::XMLSubstitute::getDefault()->getRefDictionary()[key]= val ;
            //OSLM_TRACE("Token : "<< key << " - " << val );
        //}
    //}

    //return parsing;
//}

//-----------------------------------------------------------------------------

//static const wxCmdLineEntryDesc cmdLineDesc[] =
//{
        //{ wxCMD_LINE_OPTION, "p", "profile", "path to the profile to launch",wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
        //{ wxCMD_LINE_OPTION, "s", "substitute", "substitute parameters : name1@value1@name2@value2...", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
        //{ wxCMD_LINE_PARAM, NULL, NULL, "old style parameters", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
        //{ wxCMD_LINE_NONE }
//};

//-----------------------------------------------------------------------------

//void App::OnInitCmdLine(wxCmdLineParser & parser)
//{
    //wxApp::OnInitCmdLine(parser);
    //parser.SetDesc(cmdLineDesc);
//}

//-----------------------------------------------------------------------------

//#ifdef __WXMAC__
//void App::MacOpenFile (  const wxString & fileName)
//{
    //SLM_TRACE("MacOpenFile");
    //eventMac(fileName);
//}

////-----------------------------------------------------------------------------

//void App::MacNewFile (  const wxString & fileName)
//{
    //SLM_TRACE("MacNewFile");
    //eventMac(fileName);
//}

////-----------------------------------------------------------------------------

//void App::MacReopenApp (  const wxString & fileName)
//{
    //SLM_TRACE("MacReopenApp");
    //eventMac(fileName);
//}

////-----------------------------------------------------------------------------

//void App::eventMac(const wxString & fileName)
//{
    //wxCommandEvent tEvent(wxEventFwOpen, wxIDEventFwOpen);
    //tEvent.SetString( fileName );
    //tEvent.SetEventObject( this );
    //wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    //if (frame != NULL)
        //frame->GetEventHandler()->ProcessEvent( tEvent );
    //else
        //SLM_FATAL ("Window not found !")
//}

//#endif

} // namespace fwGuiQt


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

