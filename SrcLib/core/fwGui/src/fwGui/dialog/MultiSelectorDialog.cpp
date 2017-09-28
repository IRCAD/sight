/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/dialog/MultiSelectorDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< void >(std::function< void() >(
                                                                                    [this] {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMultiSelectorDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IMultiSelectorDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string title)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->setTitle(title);
            }).wait();
}

//-----------------------------------------------------------------------------

IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    typedef IMultiSelectorDialog::Selections R;
    std::function< R() > func = std::bind( &IMultiSelectorDialog::show, m_implementation);
    std::shared_future< R > f = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< R  >(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->setSelections( _selections );
            }).wait();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string& msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->setMessage( msg );
            }).wait();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
