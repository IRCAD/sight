/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/log.h>
#include <wx/progdlg.h>
#include <wx/utils.h>
#include <wx/evtloop.h>

#include <fwCore/base.hpp>

#include "fwWX/PulseProgressTowx.hpp"



#include <iostream>




namespace fwWX {

wxDEFINE_EVENT(wxEVT_LOCALTHREAD_COMPLETED, wxThreadEvent);



PulseProgressTowx::LocalThread::LocalThread( Stuff &stuff)
: 	wxThread(wxTHREAD_JOINABLE),
	m_stuff(stuff),
	m_isFinished(false),
	m_errorMessage()
{}

bool PulseProgressTowx::LocalThread::isFinished() const
{
	return m_isFinished;
}

std::string PulseProgressTowx::LocalThread::getErrorMessage() const
{
	return m_errorMessage;
}

PulseProgressTowx::LocalThread::~LocalThread()
{}


wxThread::ExitCode PulseProgressTowx::LocalThread::Entry()
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
		// wxQueueEvent(m_client, new wxThreadEvent(wxEVT_LOCALTHREAD_COMPLETED)); avec m_client un addr sur PulseProgressTowx qui a lanc� le thread
		OSLM_TRACE("UUU PulseProgressTowx::LocalThread::Entry() FINI !!!!");

	return (wxThread::ExitCode)0;     // success
}



PulseProgressTowx::PulseProgressTowx(std::string title,  Stuff &stuff , std::string message, PulseProgressTowx::MilliSecond msecond) throw(::fwTools::Failed)
{


#ifdef PULSEPROGRESS_USE_THREAD
	this->Bind( wxEVT_LOCALTHREAD_COMPLETED , &PulseProgressTowx::onComplete , this);

	m_wxpd = new wxProgressDialog(
									wxConvertMB2WX(title.c_str()),
									"                                             ", // sinon pas de place pour ecrire definit espace initial
									100 /*percent*/,
									NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH);


	m_wxpd->Pulse( message.c_str() );
	m_wxpd->Fit();
	m_wxpd->Show();
	m_wxpd->Update();
    m_thread = new LocalThread( stuff);

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
    //  m_isFinished while( m_thread->IsRunning() ) pas recommand� � g�rer par event !!! recommandation wx // marche car joignable
    while( m_thread->isFinished()== false ) // moins couteux que wxThread::isRunning
    {
    	OSLM_TRACE("PulseProgressTowx  in Loop m_thread->isRunning" << m_thread->IsRunning() << " " << ++i );
    	m_wxpd->Pulse();
    	m_wxpd->Update();
    	wxYield();
    	wxMilliSleep(msecond);
    }
    OSLM_TRACE("PulseProgressTowx AFTER Loop m_thread->isRunning " << m_thread->IsRunning() << " " << ++i );

    if ( m_thread->getErrorMessage().size() )
    {
    	m_wxpd->Update(100,"Failed !!!");

    	OSLM_WARN( "PulseProgressTowx::m_stuff an exception has occured " << m_thread->getErrorMessage() );
    	throw ::fwTools::Failed( m_thread->getErrorMessage() );
    }
    m_wxpd->Update(100,"Done");
#else
	m_thread = NULL;
	stuff();
#endif

}


PulseProgressTowx::~PulseProgressTowx()
{
	if ( m_thread )
	{
        if (m_thread->Delete() != wxTHREAD_NO_ERROR )
            wxLogError("Can't delete the thread!");
        // Delete attend le destruction de m_thread
        m_thread = NULL;
	}


}



void PulseProgressTowx::onComplete( wxThreadEvent &event )
{
	// TODO faire un arret plus propre
	OSLM_TRACE("UUUvoid PulseProgressTowx::onComplete( wxThreadEvent &event )" );

}


}
