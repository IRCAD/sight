/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/registry/adaptors.hpp"

namespace fwRenderVTK
{

class IAdaptor;

namespace registry
{

static AdaptorRegistryType s_registry;

//------------------------------------------------------------------------------

AdaptorRegistryType& getAdaptorRegistry()
{
    return s_registry;
}

} // namespace registry

} // namespace fwRenderVTK
