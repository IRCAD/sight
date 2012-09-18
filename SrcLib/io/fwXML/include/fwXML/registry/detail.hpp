/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWXML_REGISTRY_DETAIL_HPP__
#define __FWXML_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwXML/config.hpp"

namespace fwXML
{

class XMLTranslator;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwXML::XMLTranslator) (), KeyType > Type;

FWXML_API SPTR(Type) get();

} // namespace registry

} // namespace fwXML

#endif /* __FWXML_REGISTRY_DETAIL_HPP__ */


