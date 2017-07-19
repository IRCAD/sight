/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_PROFILE_INITIALIZER_HPP__
#define __FWRUNTIME_PROFILE_INITIALIZER_HPP__

#include "fwRuntime/config.hpp"
#include "fwRuntime/Version.hpp"

#include <boost/utility.hpp>

#include <string>

namespace fwRuntime
{

namespace profile
{

/**
 * @brief   Starts a given bundle.
 */
class Initializer : public boost::noncopyable
{

public:

    /**
     * @brief       Constructor
     *
     * @param[in]   identifier  a string containing a bundle identifier
     */
    FWRUNTIME_API Initializer( const std::string& identifier, const Version& version = Version() );

    /**
     * @brief   Applies the initializer on the bundle.
     *
     * @remark  This method should be called directly.
     */
    void apply();

protected:

    const std::string m_identifier;     ///< the bundle identifier
    const Version m_version;            ///< the bundle version

};

} // namespace profile

} // namespace fwRuntime

#endif /*__FWRUNTIME_PROFILE_INITIALIZER_HPP__*/
