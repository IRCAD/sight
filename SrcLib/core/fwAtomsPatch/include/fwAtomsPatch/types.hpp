/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_TYPES_HPP__
#define __FWATOMSPATCH_TYPES_HPP__

#include <string>

#include <fwAtomConversion/DataVisitor.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
    /// Key used in object meta infos to define an object classname.
    static const std::string s_OBJ_CLASSNAME = ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO;

    /// Key used in object meta infos to define an object version.
    static const std::string s_OBJ_VERSION = "object_version";

    /// Key used in object meta infos to define an object ID.
    static const std::string s_OBJ_ID  = ::fwAtomConversion::DataVisitor::ID_METAINFO;

} // namespace fwAtomsPatch

#endif // __FWATOMSPATCH_TYPES_HPP__
