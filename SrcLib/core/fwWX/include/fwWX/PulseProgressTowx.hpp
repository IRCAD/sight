/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_PulseProgressTowx
#define FWWX_PulseProgressTowx


#include <wx/thread.h>
#include <wx/event.h>

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <fwTools/Failed.hpp>

#include "fwWX/config.hpp"


#include <iostream>

class wxProgressDialog;

namespace fwWX {

wxDECLARE_EVENT(wxEVT_PULSARTHREAD_PULSE, wxThreadEvent);

/**
 * @brief   This class allows us to use a progress bar.
 * @class   PulseProgressTowx.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note Using PulseProgressTowx will start a threaded pulsed wxProgressBar. When PulseProgressTowx instance is destroy then wxProgressBar will automatically be stopped. This tool is used when the time can not be evaluated.
 * @todo PulseProgressTowx is not commented.
 */
class FWWX_CLASS_API PulseProgressTowx :  public wxEvtHandler
{

public :
    typedef ::boost::function0<void> Stuff;

class LocalThread : public wxThread
{
public :



    LocalThread( Stuff &stuff );
    ~LocalThread();
    virtual wxThread::ExitCode  Entry ();
    bool isFinished() const;
    std::string getErrorMessage() const;

protected :
    Stuff &m_stuff;
    bool m_isFinished;
     /** @brief
      * empty if no error else contain .what() value of exception
      * catched during execution of stuff
      */
    std::string m_errorMessage;


};



public:

    typedef unsigned long MilliSecond;

    // how to react on a finished thread event
    FWWX_API void onComplete( wxThreadEvent &event);

    FWWX_API PulseProgressTowx( std::string title, Stuff &stuff,
                                std::string message= std::string(256,' '),
                                MilliSecond frequenceRefresh = 100
                               ) throw(::fwTools::Failed);

    FWWX_API virtual ~PulseProgressTowx();


protected :

    // the thread and management
    LocalThread *m_thread;
    wxCriticalSection    m_threadCS;

    // the progress
    wxProgressDialog    *m_wxpd;

};


}
#endif /* FWWX_PulseProgressTowx */
