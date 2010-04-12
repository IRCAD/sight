/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/log.h>

#include <fwCore/base.hpp>

#include "fwWX/convert.hpp"

namespace fwWX
{

class fwWXLog : public wxLog
{
public:
    fwWXLog(){};

protected:
    virtual void DoLog(wxLogLevel level, const wxString& szString, time_t t);

};


void fwWXLog::DoLog(wxLogLevel level, const wxString& szString, time_t t)
{

    wxString logLevel;

    switch ( level )
    {
        case wxLOG_FatalError:
            logLevel = wxT("FatalError: ");
            break;
        case wxLOG_Error:
            logLevel = wxT("Error: ");
            break;
        case wxLOG_Status:
            logLevel = wxT("Status: ");
            break;
        case wxLOG_Warning:
            logLevel = wxT("Warning: ");
            break;
        case wxLOG_Info:
            logLevel = wxT("Info: ");
            break;
        case wxLOG_Message:
            logLevel = wxT("Message: ");
            break;

        default:
            logLevel = wxT("Unknown log level: ");
            break;
#if wxUSE_LOG_TRACE
        case wxLOG_Trace:
            logLevel = wxT("Trace: ");
            break;
#endif
#if wxUSE_LOG_DEBUG
        case wxLOG_Debug:
            logLevel = wxT("Debug: ");
            break;
#endif
    }

    wxString timestamp;
    this->TimeStamp(&timestamp);
    std::string msg = "[WX]: " + wx2std(timestamp +": "+ logLevel + szString);
    SLM_WARN(msg);


    switch ( level )
    {

        case wxLOG_FatalError:
            wxLog::DoLog(level, szString, t);
            break;
        case wxLOG_Error:
            wxLog::DoLog(level, szString, t);
            break;
        case wxLOG_Status:
            wxLog::DoLog(level, szString, t);
            break;

    }
}



//------------------------------------------------------------------------------

class InitializeWxLogger
{

public:
    InitializeWxLogger()
    {
        wxLog * oldLog;
        oldLog = wxLog::SetActiveTarget(new fwWXLog);
        delete oldLog;
    }

};

WPTR(InitializeWxLogger) reg = SPTR(InitializeWxLogger)(new InitializeWxLogger) ;




} // namespace fwWX
