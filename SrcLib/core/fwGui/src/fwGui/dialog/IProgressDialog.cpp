/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/


#include <fwCore/base.hpp>
#include "fwGui/dialog/IProgressDialog.hpp"

namespace fwGui
{
namespace dialog
{
const IProgressDialog::FactoryRegistryKeyType IProgressDialog::REGISTRY_KEY = "::fwGui::dialog::ProgressDialog";

IProgressDialog::IProgressDialog()
    : m_canceled(false),
      m_raise(true),
      m_value(0),
      m_processUserEvents(true)
{
}

//-----------------------------------------------------------------------------

IProgressDialog::~IProgressDialog()
{
}

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



