/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_REGISTRY_ADAPTOR_HPP__
#define __FWRENDEROGRE_REGISTRY_ADAPTOR_HPP__

#include <map>
#include <string>

namespace fwRenderOgre
{

class IAdaptor;

namespace registry
{

typedef std::map<std::string, std::string> AdaptorRegistryType;

AdaptorRegistryType& getAdaptorRegistry();

} // namespace registry

} // namespace fwRenderOgre

#endif /* __FWRENDEROGRE_REGISTRY_ADAPTOR_HPP__ */
