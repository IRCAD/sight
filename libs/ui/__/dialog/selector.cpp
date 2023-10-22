/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ui/__/dialog/selector.hpp"

#include <core/thread/worker.hpp>

namespace sight::ui::dialog
{

//-----------------------------------------------------------------------------

selector::selector(
    const std::string& _title,
    const std::string& _message,
    std::vector<std::string> _selections,
    bool _multiple
) :
    selector()
{
    core::thread::get_default_worker()->post_task<void>(
        [&]
        {
            m_implementation->setTitle(_title);
            m_implementation->setMessage(_message);
            m_implementation->set_choices(_selections);
            m_implementation->set_multiple(_multiple);
        }).wait();
}

//-----------------------------------------------------------------------------

selector::selector()
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            ui::object::sptr gui_obj = ui::factory::make(selector_base::REGISTRY_KEY);
            m_implementation         = std::dynamic_pointer_cast<ui::dialog::selector_base>(gui_obj);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void selector::setTitle(std::string _title)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->setTitle(_title);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void selector::set_multiple(bool _multiple)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->set_multiple(_multiple);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

selector::selections_t selector::show()
{
    auto f      = [this](auto&& ...){return m_implementation->show();};
    auto future = core::thread::get_default_worker()->post_task<selector::selections_t>(f);
    future.wait();
    return future.get();
}

//-----------------------------------------------------------------------------

void selector::set_choices_preset(choices_preset_t _selections)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->set_choices_preset(_selections);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void selector::setMessage(const std::string& _msg)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->setMessage(_msg);
        })
    ).wait();
}

//------------------------------------------------------------------------------

void selector::addCustomButton(const std::string& _label, std::function<void()> _clicked_fn)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_implementation->addCustomButton(_label, _clicked_fn);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::dialog
