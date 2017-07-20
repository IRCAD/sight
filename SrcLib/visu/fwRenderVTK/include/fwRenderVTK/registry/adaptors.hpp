/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_REGISTRY_ADAPTORS_HPP__
#define __FWRENDERVTK_REGISTRY_ADAPTORS_HPP__

#include <map>
#include <string>

namespace fwRenderVTK
{

class IAdaptor;

namespace registry
{

/// map <adaptor Id, SRender Id>
typedef std::map<std::string, std::string> AdaptorRegistryType;

/// Get the association <adaptor, scene>
AdaptorRegistryType& getAdaptorRegistry();

} // namespace registry

} // namespace fwRenderVTK

#endif /* __FWRENDERVTK_REGISTRY_ADAPTORS_HPP__ */
