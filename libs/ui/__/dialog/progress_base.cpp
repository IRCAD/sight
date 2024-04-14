/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/dialog/progress_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

const progress_base::factory_registry_key_t progress_base::REGISTRY_KEY = "::ui::dialog::progress";

progress_base::progress_base()
= default;

//-----------------------------------------------------------------------------

progress_base::~progress_base()
= default;

//-----------------------------------------------------------------------------

void progress_base::set_cancel_callback(cancel_callback_t _callback)
{
    m_cancel_callback = _callback;
}

//-----------------------------------------------------------------------------

void progress_base::cancel_pressed()
{
    m_canceled = true;
    if(m_cancel_callback != nullptr)
    {
        m_cancel_callback();
    }
    else if(m_raise)
    {
        SIGHT_THROW("Operation canceled");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
