/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderQt/registry/Adaptor.hpp"

namespace fwRenderQt
{

class IAdaptor;

namespace registry
{

static AdaptorRegistryType s_REGISTRY;

//------------------------------------------------------------------------------

AdaptorRegistryType& getAdaptorRegistry()
{
    return s_REGISTRY;
}

} // namespace registry

} // namespace fwRenderQt
