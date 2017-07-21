/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_REGISTRY_ADAPTOR_HPP__
#define __FWRENDERQT_REGISTRY_ADAPTOR_HPP__

#include <map>
#include <string>

namespace fwRenderQt
{

class IAdaptor;

namespace registry
{

typedef std::map<std::string, std::string> AdaptorRegistryType;

/// Returns map containing Adaptor-SRender association
AdaptorRegistryType& getAdaptorRegistry();

} // namespace registry

} // namespace fwRenderQt

#endif /* __FWRENDERQT_REGISTRY_ADAPTOR_HPP__ */
