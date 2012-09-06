/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETADATA_POLICY_HPP__
#define __FWMETADATA_POLICY_HPP__

#include <string>
#include <map>

#include "fwMetaData/Object.hpp"
#include "fwMetaData/config.hpp"

namespace fwMetaData {

/**
 * @brief Base class of MetaData policy.
 * This class can be specialized to managed
 * transformation between metaData to others type (fwData, XML, Json, ...).
 * The three methods are called in this order: processType, processVersion, processAttributes.
 */

class FWMETADATA_CLASS_API Policy {
public:
    typedef ::fwMetaData::Object::Attributes Attributes;

    /**
     * @brief process the attributes os current object
     * @param attributes the attributes of object.
     */
    FWMETADATA_API virtual void processAttributes(const Attributes& attributes)= 0;

    /**
     * @brief Process version of metaData.
     */
    FWMETADATA_API virtual void processMetaInfos(const ::fwMetaData::Object::MetaInfos& metaInfos) = 0;
private:

};



}  // namespace fwMetaData

#endif
