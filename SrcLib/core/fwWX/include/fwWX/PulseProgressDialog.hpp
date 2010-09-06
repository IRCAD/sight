/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_PULSEPROGRESSDIALOG
#define FWWX_PULSEPROGRESSDIALOG

#include <wx/thread.h>
#include <wx/event.h>

#include <string>
#include <boost/shared_ptr.hpp>

#include <fwGui/PulseProgressDialog.hpp>

#include "fwWX/config.hpp"



namespace fwWX
{

/**
 * @brief   This class allows us to show a pulse progress dialog.
 * @class   PulseProgressDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo PulseProgressDialog is not commented.
 */
class FWWX_CLASS_API PulseProgressDialog : public ::fwGui::IPulseProgressDialog, public wxEvtHandler
{
public:


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


    fwCoreClassDefinitionsWithFactoryMacro( (PulseProgressDialog)(::fwGui::IPulseProgressDialog), (()), new PulseProgressDialog );

    FWWX_API PulseProgressDialog();

    FWWX_API virtual ~PulseProgressDialog();

    /// override
    FWWX_API void setTitle(const std::string &title);

    /// override
    FWWX_API void setMessage(const std::string &message);

    FWWX_API void show();

protected :

    std::string m_title;
    std::string m_message;
    // how to react on a finished thread event
    FWWX_API void onComplete( wxThreadEvent &event);

    // the thread and management
    LocalThread *m_thread;
    wxCriticalSection    m_threadCS;

    // the progress
    wxProgressDialog    *m_wxpd;

};

}

#endif /* FWWX_PULSEPROGRESSDIALOG */
