/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/dialog/SelectorDialog.hpp"
#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/bind.hpp>

#include <string>

namespace fwGui
{
namespace dialog
{


//-----------------------------------------------------------------------------

std::string SelectorDialog::showSelectorDialog(const std::string& title, const std::string& message,
                                               std::vector< std::string > _selections)
{
    ::fwGui::dialog::SelectorDialog selector(title, message, _selections);
    return selector.show();
}

//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog(const std::string& title, const std::string& message,
                               std::vector< std::string > _selections)
{
    create();
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
            {
                m_implementation->setTitle(title);
                m_implementation->setMessage( message );
                m_implementation->setSelections( _selections );
            }).wait();
}

//-----------------------------------------------------------------------------

SelectorDialog::SelectorDialog()
{
    create();
}

void SelectorDialog::create()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >([&]
            {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ISelectorDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::ISelectorDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setTitle(std::string title)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >( [&]
            {
                m_implementation->setTitle(title);
            })).wait();
}

//-----------------------------------------------------------------------------

std::string SelectorDialog::show()
{
    ::boost::function< std::string() > f         = ::boost::bind(&ISelectorDialog::show, m_implementation);
    ::boost::shared_future< std::string > future = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< std::string >(f);
    future.wait();
    return future.get();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setSelections(std::vector< std::string > _selections)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >([&]
            {
                m_implementation->setSelections( _selections );
            })).wait();
}

//-----------------------------------------------------------------------------

void SelectorDialog::setMessage(const std::string &msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >([&]
            {
                m_implementation->setMessage( msg );
            })).wait();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
