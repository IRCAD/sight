#ifndef __FWMETACONVERSION_AUTOLOAD_HPP__
#define __FWMETACONVERSION_AUTOLOAD_HPP__

#include "fwMetaConversion/custom/Factory.hpp"
#include "fwMetaConversion/custom/Graph.hpp"

namespace fwMetaConversion
{

struct runner
{
    runner()
    {
        fwMetaConversionRegisterMacro(::fwData::Graph,
                                      ::fwMetaConversion::custom::Graph);
    }

    static runner r;
};


}  // namespace fwMetaData


#endif /* __FWMETACONVERSION_AUTOLOAD_HPP__ */
