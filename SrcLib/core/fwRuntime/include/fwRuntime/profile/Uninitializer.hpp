/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_PROFILE_UNINITIALIZER_HPP_
#define _FWRUNTIME_PROFILE_UNINITIALIZER_HPP_

#include <string>
#include <boost/utility.hpp>

#include "fwRuntime/config.hpp"


namespace fwRuntime
{

namespace profile
{



/**
 * @brief   Starts a given bundle.
 * @class  Uninitializer
 * @date    2007-2009
 * @author  IRCAD (Research and Development Team).
 */
class Uninitializer : public boost::noncopyable
{
public:

    friend class Stopper;
    /**
     * @brief       Constructor
     *
     * @param[in]   identifier a string containing a bundle identifier
     */
    FWRUNTIME_API Uninitializer( const std::string & identifier );

    /**
     * @brief   Applies the uninitializer on the bundle.
     *
     * @remark  This method should be called directly.
     */
    void apply();


protected:

    const std::string   m_identifier;   ///< a bundle identifier

};



} // namespace profile

} // namespace fwRuntime



#endif /*_FWRUNTIME_PROFILE_UNINITIALIZER_HPP_*/
