/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwCore/base.hpp"
#include "fwWX/convert.hpp"
#include "fwWX/widgets/fwProgressDialog.hpp"

#include "fwWX/ProgressTowx.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

ProgressTowx::ProgressTowx( const std::string title, const std::string message)
{
    fwProgressDialog *wxpd = new fwProgressDialog(
                                    ::fwWX::std2wx(title),
                                    ::fwWX::std2wx(message),
                                    100 /*percent*/,
                                    NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL //| wxPD_REMAINING_TIME
                            );
    m_pdialog = ::boost::shared_ptr<fwProgressDialog>(wxpd);
}

//------------------------------------------------------------------------------

ProgressTowx::~ProgressTowx()
{
    // auto clean dialog
}

//------------------------------------------------------------------------------

FWWX_API void ProgressTowx::operator()(float percent,std::string msg)
{
    assert(m_pdialog);
    int value = (int)(percent*100);
    m_pdialog->Show(true); // can be hidden if previous load as finished
    m_pdialog->Update(value, ::fwWX::std2wx(msg) );
}

//------------------------------------------------------------------------------

}
