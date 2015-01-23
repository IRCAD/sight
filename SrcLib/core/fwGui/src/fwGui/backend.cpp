/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/registry/detail.hpp"
#include "fwGui/IApplication.hpp"
#include "fwGui/backend.hpp"

namespace fwGui
{

bool isBackendLoaded()
{
    return bool(::fwGui::registry::get()->getFactory(fwGui::IApplication::REGISTRY_KEY));
}

} // namespace fwGui



