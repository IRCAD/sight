/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_PROFILE_STOPPER_HPP__
#define __FWRUNTIME_PROFILE_STOPPER_HPP__

#include <string>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "fwRuntime/profile/Starter.hpp"
#include "fwRuntime/config.hpp"


namespace fwRuntime
{

namespace profile
{



/**
 * @brief   Stops a given bundle.
 * @class  Stopper
 * @date    2007-2009
 *
 */
class Stopper :  public boost::noncopyable
{

public:

    /**
     * @brief       Constructor
     *
     * @param[in]   _identifier  a string containing a bundle identifier
     */
    FWRUNTIME_API Stopper( const std::string   & _identifier );

    /**
     * @brief   Applies the Stopper on the system.
     *
     * @remark  This method should be called directly.
     */
    void apply();

private:

    const std::string m_identifier;     ///< a bundle identifier
};



} // namespace profile

} // namespace fwRuntime



#endif /*__FWRUNTIME_PROFILE_STOPPER_HPP__*/
