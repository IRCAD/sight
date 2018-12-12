/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWRUNTIME_IO_PROFILEREADER_HPP__
#define __FWRUNTIME_IO_PROFILEREADER_HPP__

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
#include <string>
#include <boost/filesystem/path.hpp>
#include <libxml/tree.h>
#if defined(__GNUC__)
#pragma GCC visibility pop
#endif

#include "fwRuntime/config.hpp"

namespace fwRuntime
{
namespace profile
{
class Activater;
class Profile;
class Starter;
}
}



namespace fwRuntime
{

namespace io
{

/**
 * @brief   Reads plugin set profiles from xml files.
 * @struct  ProfileReader
 * @date    2004-2009
 *
 */
struct ProfileReader
{

    /**
     * @brief       Creates a profile from an xml file located at the given path.
     *
     * @param[in]   path    a path to an xml profile file
     */
    FWRUNTIME_API static std::shared_ptr< ::fwRuntime::profile::Profile > createProfile(
        const boost::filesystem::path & path );



    private:

        static std::string ID;          ///< defines the name of the id xml element or attribute name
        static std::string NAME;        ///< defines the name of the xml profile
        static std::string VALUE;       ///< defines the name of the value xml element or attribute name
        static std::string VERSION;     ///< defines the name of the version xml element or attribute name
        static std::string CHECK_SINGLE_INSTANCE;///< defines the name of the check-single-instance xml element or attribute name
        static std::string ACTIVATE;    ///< defines the name of the activate xml element or attribute name
        static std::string START;       ///< defines the name of the start xml element or attribute name
        static std::string PARAM;       ///< defines the name of the parameter xml element or attribute name
        static std::string DIS_EXT_PT;  ///< defines the name of the disable extension point xml element or attribute name
        static std::string DIS_EXT;     ///< defines the name of the disable extension xml element or attribute name

        /**
         * @brief       Processes the given xml node as a profile.
         *
         * @param[in]   node    a pointer to the xml node to process
         *
         * @return      a shared pointer to the created profile
         */
        static std::shared_ptr< ::fwRuntime::profile::Profile > processProfile(xmlNodePtr node);

        /**
         * @brief       Processes the given xml node as an activater.
         *
         * @param[in]   node    a pointer to the xml node to process
         *
         * @return      a shared pointer to the created activater
         */
        static std::shared_ptr< ::fwRuntime::profile::Activater > processActivater(xmlNodePtr node);

        /**
         * @brief       Processes the given xml node as an activater parameter and
         *              stores it in the given activater.
         *
         * @param[in]   node        a pointer to the xml node to process
         * @param[out]  activater   a shared pointer to an activater
         */
        static void processActivaterParam(xmlNodePtr node,
                                          std::shared_ptr< ::fwRuntime::profile::Activater > activater);

        /**
         * @brief       Processes the given xml node as an activater disable extension point and
         *              stores it in the given activater.
         *
         * @param[in]   node        a pointer to the xml node to process
         * @param[out]  activater   a shared pointer to an activater
         */
        static void processActivaterDisableExtensionPoint(xmlNodePtr node,
                                                          std::shared_ptr< ::fwRuntime::profile::Activater > activater);

        /**
         * @brief       Processes the given xml node as an activater disable extension and
         *              stores it in the given activater.
         *
         * @param[in]   node        a pointer to the xml node to process
         * @param[out]  activater   a shared pointer to an activater
         */
        static void processActivaterDisableExtension(xmlNodePtr node,
                                                     std::shared_ptr< ::fwRuntime::profile::Activater > activater);

        /**
         * @brief       Processes the given xml node as a starter.
         *
         * @param[in]   node    a pointer to the xml node to process
         *
         * @return      a shared pointer to the created starter
         */
        static std::shared_ptr< ::fwRuntime::profile::Starter > processStarter(xmlNodePtr node);

};



} // namespace io

} // namespace fwRuntime



#endif /*__FWRUNTIME_IO_PROFILEREADER_HPP__*/
