/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#include "fwGui/dialog/ProgressDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

ProgressDialog::ProgressDialog(const std::string& title, const std::string& message)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IProgressDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IProgressDialog::dynamicCast(guiObj);
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                    m_implementation->setMessage(message);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( [&] { m_implementation.reset();
                                                                               } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string& title)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string& msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    m_implementation->setMessage(msg);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::operator()(float percent, std::string msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    (*m_implementation)(percent, msg);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::setCancelCallback(CancelCallbackType callback)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    m_implementation->setCancelCallback(callback);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    assert(0);
}

//-----------------------------------------------------------------------------

void ProgressDialog::hideCancelButton()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->hideCancelButton();
            } ).wait();
}

} //namespace dialog
} //namespace fwGui
