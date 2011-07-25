/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_NEVERDUMPPOLICY_HPP_
#define FWMEMORY_NEVERDUMPPOLICY_HPP_
#include "fwMemory/IDumpPolicy.hpp"

namespace fwMemory
{
namespace policy
{

/**
 * @brief   Never dump objects, whatever memory consumption status
 * @class   NeverDumpPolicy.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2011
 */
class FWMEMORY_CLASS_API NeverDumpPolicy : public IDumpPolicy
{
public :
    NeverDumpPolicy() ;
    virtual ~NeverDumpPolicy() ;
    virtual bool apply( ::boost::uint64_t reserveMemory=0);
};

} //namespace policy
} //namespace fwMemory

#endif /*NEVERDUMPPOLICY_HPP_*/
