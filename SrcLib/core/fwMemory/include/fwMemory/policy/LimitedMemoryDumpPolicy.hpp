/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_LIMITEDMEMORYDUMPPOLICY_HPP_
#define FWMEMORY_LIMITEDMEMORYDUMPPOLICY_HPP_

#include <boost/cstdint.hpp>
#include "fwMemory/IDumpPolicy.hpp"



namespace fwMemory
{
namespace policy
{


/**
 * @brief Dump objects according to their age if memory overflows a given threshold vs available memory
 */
class FWMEMORY_CLASS_API LimitedMemoryDumpPolicy : public IDumpPolicy
{
public :
    ///Ctor
    FWMEMORY_API LimitedMemoryDumpPolicy() ;
    ///Dtor
    FWMEMORY_API virtual ~LimitedMemoryDumpPolicy() ;
    /// Overrides
    FWMEMORY_API virtual bool apply( ::boost::uint64_t reserveMemory=0);

    /// set threshold (in Bytes) : dumps are performed until estimatedFreeMemory - reserveMemory < threshold
    FWMEMORY_API void setThreshold( ::boost::uint64_t threshold);

    FWMEMORY_API void configure( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement >  cfgElt);

protected :

    /// threshold unit in Bytes : dumps are performed until estimatedFreeMemory - reserveMemory < threshold
    /// default value 100 Mo
    ::boost::uint64_t m_threshold;
};


} //namespace policy
} //namespace fwMemory

#endif /*NEVERDUMPPOLICY_HPP_*/
