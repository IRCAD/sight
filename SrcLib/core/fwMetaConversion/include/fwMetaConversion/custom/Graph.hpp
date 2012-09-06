/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETACONVERSION_CUSTOM_GRAPH_HPP__
#define __FWMETACONVERSION_CUSTOM_GRAPH_HPP__



#include "fwMetaConversion/custom/Mapper.hpp"
#include "fwMetaConversion/custom/Factory.hpp"


#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{
namespace custom
{
namespace MetaData = ::fwMetaData;


class FWMETACONVERSION_CLASS_API Graph : public Mapper

{
    /**
     * Convert fwData to MetaData
     * @param object the origin fwData
     * @param metaHelper this metaHelper is used to compute internal
     * fwDataObject to metaData
     * @return the mapped metaData
     */
    FWMETACONVERSION_API virtual MetaData::Object::sptr
                                 toMeta( ::fwData::Object::sptr object,
                                        ::fwMetaConversion::MetaHelper& metaHelper);

    /**
     * Convert MetaData to fwData
     * @param meta the origin metaData
     * @param metaHelper this metaHelper is used to compute internal
     * metaData
     * @return the new fwData
     **/
    FWMETACONVERSION_API virtual ::fwData::Object::sptr
                                 fromMeta(MetaData::Object::sptr meta,
                                     ::fwMetaConversion::MetaHelper& metaHelper);

};

}

}

#endif /*  __FWMETACONVERSION_CUSTOM_GRAPH_HPP__*/
