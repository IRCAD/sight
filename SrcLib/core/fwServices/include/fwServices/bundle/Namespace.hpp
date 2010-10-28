/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWFWSERVICESBUNDLENAMESPACE_HPP_
#define FWFWSERVICESBUNDLENAMESPACE_HPP_

namespace fwServices
{
/**
 * @brief The namespace fwServices::bundle concerns component based object service basic manipulations
 *
 * \li Mainly consists in the support and add methods (support( ::fwTools::Object::sptr , std::string ) and add( ::fwTools::Object::sptr , std::string , std::string )).
 * \li Helpers have been developped for internal use (should not be used from outside) to analyze the XML based graph provided by component descriptions
 * @note It focuses on find the appropriate component to start (and dynamic libraries to load) before invoking class registry
 * @author IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
namespace bundle
{

}
}
#endif /* FWFWSERVICESBUNDLENAMESPACE_HPP_ */
