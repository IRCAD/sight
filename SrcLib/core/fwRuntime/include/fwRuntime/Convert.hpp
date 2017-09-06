/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_CONVERT_HPP__
#define __FWRUNTIME_CONVERT_HPP__

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"

#include <boost/property_tree/ptree.hpp>

#include <libxml/tree.h>

namespace fwRuntime
{

/**
 * @brief   Defines the convert class.
 */
class FWRUNTIME_CLASS_API Convert
{
public:

    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~Convert();

    /**
     * @brief   Build an xmlNodePtr from a ConfigurationElement
     */
    FWRUNTIME_API static xmlNodePtr toXml( std::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement);

    /**
     * @brief   Build an std::string from a ConfigurationElement
     */
    FWRUNTIME_API std::string static toXmlString( ::fwRuntime::ConfigurationElement::sptr _cfgElement);

    /**
     * @brief   Build an xmlNodePtr with all running Bundles
     */
    FWRUNTIME_API static xmlNodePtr runningBundlesToXml( );

    FWRUNTIME_API static ::boost::property_tree::ptree toPropertyTree(
        ::fwRuntime::ConfigurationElement::csptr _cfgElement );

    FWRUNTIME_API static ::fwRuntime::ConfigurationElement::sptr fromPropertyTree( ::boost::property_tree::ptree pt );

private:

    static void fromConfigurationElementToXml(std::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement,
                                              xmlNodePtr _node);

    /**
     * @brief   Constructor : does nothing.
     */
    Convert();

};

} // namespace fwRuntime

#endif // __FWRUNTIME_CONVERT_HPP__
