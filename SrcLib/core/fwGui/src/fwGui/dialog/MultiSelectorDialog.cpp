/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/dialog/MultiSelectorDialog.hpp"
#include "fwGui/registry/worker.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog()
{
    ::fwGui::registry::worker::get()->postTask< void >(::boost::function< void() >(
                                                           [this] {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IMultiSelectorDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IMultiSelectorDialog::dynamicCast(guiObj);
            })).wait();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string title)
{
    ::fwGui::registry::worker::get()->postTask<void>(
        [&] {
                m_implementation->setTitle(title);
            }).wait();
}

//-----------------------------------------------------------------------------

IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    typedef IMultiSelectorDialog::Selections R;
    ::boost::function< R() > func = ::boost::bind( &IMultiSelectorDialog::show, m_implementation);
    ::boost::shared_future< R > f = ::fwGui::registry::worker::get()->postTask< R  >(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    ::fwGui::registry::worker::get()->postTask<void>(
        [&] {
                m_implementation->setSelections( _selections );
            }).wait();
}

//-----------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string &msg)
{
    ::fwGui::registry::worker::get()->postTask<void>(
        [&] {
                m_implementation->setMessage( msg );
            }).wait();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
