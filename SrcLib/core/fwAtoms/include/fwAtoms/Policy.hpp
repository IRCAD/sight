/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_POLICY_HPP__
#define __FWATOMS_POLICY_HPP__

#include <string>
#include <map>

#include "fwAtoms/Object.hpp"
#include "fwAtoms/config.hpp"

namespace fwAtoms {

/**
 * @brief Base class of MetaData policy.
 * This class can be specialized to managed
 * transformation between metaData to others type (fwData, XML, Json, ...).
 * The three methods are called in this order: processType, processVersion, processAttributes.
 */

class FWATOMS_CLASS_API Policy {
public:
    typedef ::fwAtoms::Object::AttributesType AttributesType;

    /**
     * @brief process the attributes os current object
     * @param attributes the attributes of object.
     */
    FWATOMS_API virtual void processAttributes(const AttributesType& attributes)= 0;

    /**
     * @brief Process version of metaData.
     */
    FWATOMS_API virtual void processMetaInfos(const ::fwAtoms::Object::MetaInfosType& metaInfos) = 0;
private:

};



}  // namespace fwAtoms

#endif

