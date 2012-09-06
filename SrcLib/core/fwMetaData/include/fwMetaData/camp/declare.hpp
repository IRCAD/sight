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
