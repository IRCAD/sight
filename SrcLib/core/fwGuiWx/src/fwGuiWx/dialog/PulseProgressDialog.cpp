/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/log.h>
#include <wx/progdlg.h>
#include <wx/utils.h>
#include <wx/evtloop.h>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwWX/convert.hpp>
#include <fwWX/widgets/fwProgressDialog.hpp>

#include "fwGuiWx/dialog/PulseProgressDialog.hpp"

REGISTER_BINDING( ::fwGui::dialog::IPulseProgressDialog, ::fwGuiWx::dialog::PulseProgressDialog, ::fwGui::dialog::IPulseProgressDialog::FactoryRegistryKeyType , ::fwGui::dialog::IPulseProgressDialog::REGISTRY_KEY );


namespace fwGuiWx
{
namespace dialog
{
//------------------------------------------------------------------------------

wxDEFINE_EVENT(wxEVT_LOCALTHREAD_COMPLETED, wxThreadEvent);

//------------------------------------------------------------------------------


PulseProgressDialog::LocalThread::LocalThread( Stuff &stuff)
:   wxThread(wxTHREAD_JOINABLE),
    m_stuff(stuff),
    m_isFinished(false),
    m_errorMessage()
{}

//------------------------------------------------------------------------------

bool PulseProgressDialog::LocalThread::isFinished() const
{
    return m_isFinished;
}

//------------------------------------------------------------------------------

std::string PulseProgressDialog::LocalThread::getErrorMessage() const
{
    return m_errorMessage;
}

//------------------------------------------------------------------------------

PulseProgressDialog::LocalThread::~LocalThread()
{}

//------------------------------------------------------------------------------

wxThread::ExitCode PulseProgressDialog::LocalThread::Entry()
{
        try
        {
            m_stuff();
        }
        catch ( std::exception &e)
        {
            m_errorMessage = e.what();
        }
        m_isFinished=true;
        // TODO meilleur arret
        // Don't Poll a wxThread
        // A common problem users experience with wxThread is that in their main thread they will check the thread every now
        //and then to see if it has ended through IsRunning(), only to find that their application has run into problems because
        //the thread is using the default behavior (i.e. it's detached) and has already deleted itself. Naturally, they instead
        // attempt to use joinable threads in place of the previous behavior. However, polling a wxThread for when it has ended is
        // in general a bad idea - in fact calling a routine on any running wxThread should be avoided if possible. Instead, find a
        // way to notify yourself when the thread has ended.
        // wxQueueEvent(m_client, new wxThreadEvent(wxEVT_LOCALTHREAD_COMPLETED)); avec m_client un addr sur PulseProgressDialog qui a lanc� le thread
        OSLM_TRACE("UUU PulseProgressDialog::LocalThread::Entry() FINI !!!!");

    return (wxThread::ExitCode)0;     // success
}


//------------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog()
{
    m_wxpd = NULL;
}

//------------------------------------------------------------------------------

PulseProgressDialog::~PulseProgressDialog()
{
    if ( m_thread )
    {
        if (m_thread->Delete() != wxTHREAD_NO_ERROR )
            wxLogError("Can't delete the thread!");
        // Delete attend le destruction de m_thread
        m_thread = NULL;
    }
    if (m_wxpd)
    {
        m_wxpd->Destroy();
        m_wxpd = NULL;
    }

}

//------------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string &title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string &msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

void PulseProgressDialog::show()
{
#ifdef PULSEPROGRESS_USE_THREAD

    this->Bind( wxEVT_LOCALTHREAD_COMPLETED , &PulseProgressDialog::onComplete , this);

    m_wxpd = new wxProgressDialog(
                                    ::fwWX::std2wx(m_title),
                                    "                                             ", // sinon pas de place pour ecrire definit espace initial
                                    100 /*percent*/,
                                    NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH);

    m_wxpd->Pulse( m_message );
    m_wxpd->Fit();
    m_wxpd->Show();
    m_wxpd->Update();
    m_thread = new LocalThread( m_stuff);

    if ( m_thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create the thread!");
        delete m_thread;
        m_thread = NULL;
    }
    else
    {
        if (m_thread->Run() != wxTHREAD_NO_ERROR )
        {
            wxLogError("Can't create the thread!");
            delete m_thread;
            m_thread = NULL;
        }
    }

    int i=0;

    while( m_thread->isFinished()== false ) // moins couteux que wxThread::isRunning
    {
        OSLM_TRACE("PulseProgressDialog  in Loop m_thread->isRunning" << m_thread->IsRunning() << " " << ++i );
        m_wxpd->Pulse();
        m_wxpd->Update();
        wxYield();
        wxMilliSleep(m_frequence);
    }
    OSLM_TRACE("PulseProgressDialog AFTER Loop m_thread->isRunning " << m_thread->IsRunning() << " " << ++i );

    if ( m_thread->getErrorMessage().size() )
    {
        m_wxpd->Update(100,"Failed !!!");

        OSLM_WARN( "PulseProgressDialog::m_stuff an exception has occured " << m_thread->getErrorMessage() );
        throw ::fwTools::Failed( m_thread->getErrorMessage() );
    }
    m_wxpd->Update(100,"Done");
#else
    m_thread = NULL;
    m_stuff();
#endif
}

//------------------------------------------------------------------------------

void PulseProgressDialog::onComplete( wxThreadEvent &event )
{
    // TODO faire un arret plus propre
    SLM_TRACE_FUNC();

}

} // namespace dialog
} // namespace fwGuiWx
