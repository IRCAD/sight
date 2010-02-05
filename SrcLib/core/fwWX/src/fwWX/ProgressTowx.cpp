/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwWX/ProgressTowx.hpp"
#include "wx/progdlg.h"

namespace fwWX {

ProgressTowx::ProgressTowx(std::string title, const std::string message)
{
        // TODO Auto-generated constructor stub
        wxProgressDialog *wxpd = new wxProgressDialog(
                                                                        wxConvertMB2WX(title.c_str()),
                                                                        wxConvertMB2WX(message.c_str()),
                                                                        100 /*percent*/,
                                                                        NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL //| wxPD_REMAINING_TIME
                                                        );
        m_pdialog = ::boost::shared_ptr<wxProgressDialog>(wxpd);

}

ProgressTowx::~ProgressTowx() {
        // auto clean dialog
}

FWWX_API void ProgressTowx::operator()(float percent,std::string msg)
{
        assert(m_pdialog);
        int value = (int)(percent*100);
        m_pdialog->Show(true); // can be hidden if repvious load as finished
        m_pdialog->Update(value,wxConvertMB2WX(msg.c_str()));
}

}
