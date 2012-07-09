#ifndef __FWMETACONVERSION_CUSTOM_MAPPER_HPP__
#define __FWMETACONVERSION_CUSTOM_MAPPER_HPP__

#include <iostream>
#include <map>
#include <string>

#include <camp/class.hpp>

#include <fwData/Object.hpp>
#include <fwMetaConversion/MetaHelper.hpp>
#include <fwMetaData/Object.hpp>


#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{
namespace custom
{

/**
 * Base class of custom mapper between fwData and MetaData
 */
class FWMETACONVERSION_CLASS_API Mapper
{
public:
    /**
     * Convert fwData to MetaData
     * @param object the origin fwData
     * @param metaHelper this metaHelper is used to compute internal
     * fwDataObject to metaData
     * @return the mapped metaData
     */
    FWMETACONVERSION_API virtual ::fwMetaData::Object::sptr
                                  toMeta(::fwData::Object::sptr object,
                                        ::fwMetaConversion::MetaHelper& metaHelper)
                                        { return ::fwMetaData::Object::New();}

    /**
     * Convert MetaData to fwData
     * @param meta the origin metaData
     * @param metaHelper this metaHelper is used to compute internal
     * metaData
     * @return the new fwData
     **/
    FWMETACONVERSION_API virtual ::fwData::Object::sptr
                                 fromMeta(::fwMetaData::Object::sptr meta,
                                        ::fwMetaConversion::MetaHelper& metaHelper)
                                        { return ::fwData::Object::New();}

protected:
    ::fwMetaData::Object::sptr computeMinimalInfos(::fwData::Object::sptr object,
                                                         ::fwMetaConversion::MetaHelper& metaHelper
                                                         );
    ::fwData::Object::sptr computeMinimalInfos(::fwMetaData::Object::sptr object,
                                                     ::fwMetaConversion::MetaHelper& metaHelper
                                                         );

}; // Value Object

}
}

#endif /* __FWMETACONVERSION_CUSTOM_MAPPER_HPP__ */
