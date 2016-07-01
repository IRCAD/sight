/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwCore/base.hpp>

#include <fwData/registry/detail.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "monitor/action/fwMetrics.hpp"

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitor::action::fwMetrics, ::fwData::Object );

//------------------------------------------------------------------------------

fwMetrics::fwMetrics( ) throw()
{
}

//------------------------------------------------------------------------------

fwMetrics::~fwMetrics() throw()
{
}

//------------------------------------------------------------------------------

void fwMetrics::updating() throw(::fwTools::Failed)
{
    std::stringstream stream;

    stream << "Registered factory keys";
    stream << std::endl;

    stream << "fwData: ";
    stream << ::fwData::registry::get()->getFactoryKeys().size();
    stream << std::endl;

    stream << "IService: ";
    stream << ::fwServices::registry::ServiceFactory::getDefault()->getFactoryKeys().size();

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("FactoryRegistry Information");
    messageBox.setMessage( stream.str() );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

void fwMetrics::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void fwMetrics::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void fwMetrics::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------


} // namespace action
} // namespace monitor
