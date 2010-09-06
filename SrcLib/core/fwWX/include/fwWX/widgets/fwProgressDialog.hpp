/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_FWPROGRESSDIALOG
#define FWWX_FWPROGRESSDIALOG

#include <string>


#include <wx/defs.h>
#include <wx/progdlg.h>

#if wxUSE_PROGRESSDLG

#include <wx/dialog.h>

#include "fwWX/config.hpp"

class wxButton;
class wxGauge;
class wxStaticText;

/* Progress dialog without YieldFor. Copied and modifed from wxWidget2.9 */

class  FWWX_CLASS_API fwProgressDialog : public wxDialog
{
DECLARE_DYNAMIC_CLASS(fwProgressDialog)
public:
    /* Creates and displays dialog, disables event handling for other
       frames or parent frame to avoid recursion problems.
       @param title title for window
       @param message message to display in window
       @param maximum value for status bar, if <= 0, no bar is shown
       @param parent window or NULL
       @param style is the bit mask of wxPD_XXX constants from wx/defs.h
    */
    FWWX_API fwProgressDialog(const wxString& title, const wxString& message,
                     int maximum = 100,
                     wxWindow *parent = NULL,
                     int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);
    /* Destructor.
        Re-enables event handling for other windows.
    */
    FWWX_API virtual ~fwProgressDialog();

    /* Update the status bar to the new value.
       @param value new value
       @param newmsg if used, new message to display
       @return true if ABORT button has not been pressed
    */
    FWWX_API virtual bool Update(int value, const wxString& newmsg = wxEmptyString, bool *skip = NULL);

    /* Switches the dialog to use a gauge in indeterminate mode and calls
       wxGauge::Pulse() to show to the user a bit of progress */
    FWWX_API virtual bool Pulse(const wxString& newmsg = wxEmptyString, bool *skip = NULL);

    // Must provide overload to avoid hiding it (and warnings about it)
    FWWX_API virtual void Update() { wxDialog::Update(); }

    FWWX_API virtual bool Show( bool show = true );

    /* Can be called to continue after the cancel button has been pressed, but
       the program decided to continue the operation (e.g., user didn't
       confirm it)
    */
    FWWX_API void Resume();

    FWWX_API int GetValue() const;
    FWWX_API int GetRange() const;
    FWWX_API wxString GetMessage() const;

    // updates the label message
    FWWX_API void UpdateMessage(const wxString &newmsg);

protected:
    // callback for optional abort button
    void OnCancel(wxCommandEvent&);

    // callback for optional skip button
    void OnSkip(wxCommandEvent&);

    // callback to disable "hard" window closing
    void OnClose(wxCloseEvent&);

    // must be called to reenable the other windows temporarily disabled while
    // the dialog was shown
    void ReenableOtherWindows();

private:
    // create the label with given text and another one to show the time nearby
    // as the next windows in the sizer, returns the created control
    wxStaticText *CreateLabel(const wxString& text, wxSizer *sizer);



    // common part of Update() and Pulse(), returns true if not cancelled
    bool DoAfterUpdate(bool *skip);

    // shortcuts for enabling buttons
    void EnableClose();
    void EnableSkip(bool enable = true);
    void EnableAbort(bool enable = true);
    void DisableSkip() { EnableSkip(false); }
    void DisableAbort() { EnableAbort(false); }

    // the widget displaying current status (may be NULL)
    wxGauge *m_gauge;
    // the message displayed
    wxStaticText *m_msg;
    // displayed elapsed, estimated, remaining time
    wxStaticText *m_elapsed,
                 *m_estimated,
                 *m_remaining;
    // time when the dialog was created
    unsigned long m_timeStart;
    // time when the dialog was closed or cancelled
    unsigned long m_timeStop;
    // time between the moment the dialog was closed/cancelled and resume
    unsigned long m_break;

    // parent top level window (may be NULL)
    wxWindow *m_parentTop;

    // continue processing or not (return value for Update())
    enum
    {
        Uncancelable = -1,   // dialog can't be canceled
        Canceled,            // can be cancelled and, in fact, was
        Continue,            // can be cancelled but wasn't
        Finished             // finished, waiting to be removed from screen
    } m_state;

    // skip some portion
    bool m_skip;

#if !defined(__SMARTPHONE__)
    // the abort and skip buttons (or NULL if none)
    wxButton *m_btnAbort;
    wxButton *m_btnSkip;
#endif

    // the maximum value
    int m_maximum;

    // saves the time when elapsed time was updated so there is only one
    // update per second
    unsigned long m_last_timeupdate;
    // tells how often a change of the estimated time has to be confirmed
    // before it is actually displayed - this reduces the frequence of updates
    // of estimated and remaining time
    const int m_delay;
    // counts the confirmations
    int m_ctdelay;
    unsigned long m_display_estimated;

    bool m_hasAbortButton,
         m_hasSkipButton;

#if defined(__WXMSW__ ) || defined(__WXPM__)
    // the factor we use to always keep the value in 16 bit range as the native
    // control only supports ranges from 0 to 65,535
    size_t m_factor;
#endif // __WXMSW__

    // for wxPD_APP_MODAL case
    class WXDLLIMPEXP_FWD_CORE wxWindowDisabler *m_winDisabler;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(fwProgressDialog);
};

#endif // wxUSE_PROGRESSDLG

#endif // FWWX_FWPROGRESSDIALOG
