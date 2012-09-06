/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWABSTRACTDATA_CAMP_DECLARE_HPP__
#define __FWABSTRACTDATA_CAMP_DECLARE_HPP__

#include "fwMetaData/Types.hpp"

#include "fwMetaData/config.hpp"

namespace fwMetaData {
namespace camp {

struct runner
{
    runner()
    {
        localDeclarefwMetaDataBase();
        localDeclarefwMetaDataString();
        localDeclarefwMetaDataNumeric();
        localDeclarefwMetaDataBoolean();
        localDeclarefwMetaDataObject();
        localDeclarefwMetaDataMap();
        localDeclarefwMetaDataSequence();
        localDeclarefwMetaDataBlob();
    }

    static runner r;
};


}  // namespace camp



}  // namespace fwMetaData


#endif /* __FWABSTRACTDATA_CAMP_DECLARE_HPP__ */
