/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/dialog/MessageDialog.hpp"
#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/function.hpp>

namespace fwGui
{
namespace dialog
{

//-----------------------------------------------------------------------------

IMessageDialog::Buttons MessageDialog::showMessageDialog(
    const std::string& title, const std::string& message, ::fwGui::dialog::IMessageDialog::Icons icon)
{
    ::fwGui::dialog::MessageDialog messageBox(title, message, icon);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    return messageBox.show();
}

//-----------------------------------------------------------------------------

MessageDialog::MessageDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMessageDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IMessageDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

MessageDialog::MessageDialog(
    const std::string& title, const std::string& message, ::fwGui::dialog::IMessageDialog::Icons icon)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMessageDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IMessageDialog::dynamicCast(guiObj);

                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                    m_implementation->setMessage(message);
                    m_implementation->setIcon(icon);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
}

//-----------------------------------------------------------------------------

void MessageDialog::setTitle( const std::string &title )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setMessage( const std::string &msg )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setMessage(msg);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setIcon( ::fwGui::dialog::IMessageDialog::Icons icon )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setIcon(icon);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::addButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->addButton(button);
                }
            } )).wait();
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton( ::fwGui::dialog::IMessageDialog::Buttons button )
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( ::boost::function<void()>([&]
            {
                if(m_implementation)
                {
                    m_implementation->setDefaultButton(button);
                }
            })).wait();
}

//-----------------------------------------------------------------------------

::fwGui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    if(m_implementation)
    {
        typedef ::fwGui::dialog::IMessageDialog::Buttons R;

        ::boost::function<R()> func = ::boost::bind(&IMessageDialog::show, m_implementation);
        ::boost::shared_future<R> f = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<R>(func);
        f.wait();

        return f.get();
    }
    else
    {
        return ::fwGui::dialog::IMessageDialog::NOBUTTON;
    }
}

//-----------------------------------------------------------------------------

} //namespace dialog
} // namespace fwGui

