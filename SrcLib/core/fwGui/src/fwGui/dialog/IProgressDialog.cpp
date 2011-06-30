/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwCore/base.hpp>
#include "fwGui/dialog/IProgressDialog.hpp"

namespace fwGui
{
namespace dialog
{
const IProgressDialog::FactoryRegistryKeyType IProgressDialog::REGISTRY_KEY =  "::fwGui::dialog::ProgressDialog";

IProgressDialog::IProgressDialog() : m_canceled(false), m_raise(true), m_processUserEvents(true)
{}

//-----------------------------------------------------------------------------

IProgressDialog::~IProgressDialog()
{}

//-----------------------------------------------------------------------------

void IProgressDialog::setCancelCallback(CancelCallbackType callback)
{
    m_cancelCallback = callback;
}

//-----------------------------------------------------------------------------

void IProgressDialog::cancelPressed()
{
    m_canceled = true;
    if(m_cancelCallback)
    {
        m_cancelCallback();
    }
    else if (m_raise)
    {
        FW_RAISE("Operation canceled");
    }
}

//-----------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGui



