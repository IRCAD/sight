#ifndef __FWDATA_CAMP_DECLARE_HPP__
#define __FWDATA_CAMP_DECLARE_HPP__

#include <fwCamp/macros.hpp>
#include "fwTools/BufferObject.hpp"
#include "fwTools/Object.hpp"

namespace fwTools
{

namespace camp {

struct runner
{
    runner()
    {
        localDeclarefwToolsBufferObject();
        // localDeclarefwToolsObject();
    }

    static runner r;
};
}


}



#endif
