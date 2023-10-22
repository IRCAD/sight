/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ui/__/dialog/progress.hpp"

#include <core/thread/worker.hpp>

#include <functional>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

progress::progress(const std::string& _title, const std::string& _message)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(progress_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::progress_base>(gui_obj);
            if(m_implementation)
            {
                m_implementation->setTitle(_title);
                m_implementation->setMessage(_message);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

progress::~progress()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            m_implementation.reset();
        }).wait();
}

//-----------------------------------------------------------------------------

void progress::setTitle(const std::string& _title)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setTitle(_title);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void progress::setMessage(const std::string& _msg)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setMessage(_msg);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void progress::operator()(float _percent, std::string _msg)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                (*m_implementation)(_percent, _msg);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void progress::setCancelCallback(cancel_callback_t _callback)
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            if(m_implementation)
            {
                m_implementation->setCancelCallback(_callback);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void progress::cancelPressed()
{
    assert(0);
}

//-----------------------------------------------------------------------------

void progress::hideCancelButton()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            m_implementation->hideCancelButton();
        }).wait();
}

} // namespace sight::ui::dialog
