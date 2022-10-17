/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/dialog/IProgressDialog.hpp"

#include <core/base.hpp>

namespace sight::ui::base::dialog
{

const IProgressDialog::FactoryRegistryKeyType IProgressDialog::REGISTRY_KEY = "::ui::base::dialog::ProgressDialog";

IProgressDialog::IProgressDialog()
= default;

//-----------------------------------------------------------------------------

IProgressDialog::~IProgressDialog()
= default;

//-----------------------------------------------------------------------------

void IProgressDialog::setCancelCallback(CancelCallbackType callback)
{
    m_cancelCallback = callback;
}

//-----------------------------------------------------------------------------

void IProgressDialog::cancelPressed()
{
    m_canceled = true;
    if(m_cancelCallback != nullptr)
    {
        m_cancelCallback();
    }
    else if(m_raise)
    {
        SIGHT_THROW("Operation canceled");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::dialog
