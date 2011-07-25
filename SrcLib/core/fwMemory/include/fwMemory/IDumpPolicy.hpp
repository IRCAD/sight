/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_IDUMPPOLICY_HPP_
#define FWMEMORY_IDUMPPOLICY_HPP_

#include <boost/cstdint.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include "fwMemory/config.hpp"

namespace fwMemory
{

/**
 * @brief Define the API policy (to be specialised) to apply for memory management
 */
class FWMEMORY_CLASS_API IDumpPolicy
{
public :
    /// Ctor
    FWMEMORY_API IDumpPolicy() ;
    /// Dtor
    FWMEMORY_API virtual ~IDumpPolicy() ;
    /// Invoked by the memory monitor on changed : can reserve extraMemory
    FWMEMORY_API virtual bool apply( ::boost::uint64_t reverseMemory=0 ) = 0 ;

    FWMEMORY_API virtual void configure( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement >  cfgElt);
};

} // namespace fwMemory

#endif // FWMEMORY_IDUMPPOLICY_HPP_
