/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/dialog/InputDialog.hpp"

#include "fwGui/registry/worker.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

std::string InputDialog::showInputDialog(const std::string& title, const std::string& message, const std::string &text)
{
    ::fwGui::dialog::InputDialog inputBox(title, message, text);
    return inputBox.getInput();
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog()
{
    ::fwGui::registry::worker::get()->postTask<void>(::boost::function< void() >([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IInputDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IInputDialog::dynamicCast(guiObj);
            }));
}

//-----------------------------------------------------------------------------

InputDialog::InputDialog(const std::string& title, const std::string& message, const std::string &text)
{
    ::fwGui::registry::worker::get()->postTask<void>(::boost::function< void() >( [&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IInputDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IInputDialog::dynamicCast(guiObj);
                m_implementation->setTitle(title);
                m_implementation->setMessage(message);
                m_implementation->setInput(text);
            }));
}

//-----------------------------------------------------------------------------

InputDialog::~InputDialog()
{
}

//-----------------------------------------------------------------------------

void InputDialog::setTitle( const std::string &title )
{
    ::fwGui::registry::worker::get()->postTask<void>(::boost::function< void() >([&]
            {
                m_implementation->setTitle(title);
            })).wait();
}

//-----------------------------------------------------------------------------

void InputDialog::setMessage( const std::string &msg )
{
    ::fwGui::registry::worker::get()->postTask<void>(::boost::function< void() >( [&]
            {
                m_implementation->setMessage(msg);
            })).wait();
}

//-----------------------------------------------------------------------------

void InputDialog::setInput(const std::string &text)
{
    ::fwGui::registry::worker::get()->postTask<void>(::boost::function< void() >([&]
            {
                m_implementation->setInput(text);
            })).wait();
}

//-----------------------------------------------------------------------------

std::string InputDialog::getInput()
{
    ::boost::function< std::string() > func = ::boost::bind(&IInputDialog::getInput, m_implementation);
    ::boost::shared_future< std::string > f = ::fwGui::registry::worker::get()->postTask<std::string>(func);
    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui



