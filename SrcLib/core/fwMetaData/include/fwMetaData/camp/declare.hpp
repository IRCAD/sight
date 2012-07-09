#ifndef __FWABSTRACTDATA_CAMP_DECLARE_HPP__
#define __FWABSTRACTDATA_CAMP_DECLARE_HPP__

#include "fwMetaData/Object.hpp"
#include "fwMetaData/Blob.hpp"

#include "fwMetaData/config.hpp"

namespace fwMetaData {
namespace camp {

struct runner
{
    runner()
    {
        localDeclarefwMetaDataBase();
        localDeclarefwMetaDataObject();
    }

    static runner r;
};


}  // namespace camp



}  // namespace fwMetaData


#endif /* __FWABSTRACTDATA_CAMP_DECLARE_HPP__ */
