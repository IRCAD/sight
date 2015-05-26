/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATACAMP_UT_DATACAMPHELPER_HPP__
#define __ARDATACAMP_UT_DATACAMPHELPER_HPP__

#include <fwData/Object.hpp>

namespace arDataCamp
{
namespace ut
{

namespace DataCampHelper
{
    typedef std::vector<std::string> PropertiesNameType;

    void visitProperties(const std::string& className, const PropertiesNameType& vecProp);

    void compareSimplePropertyValue(::fwData::Object::sptr obj,
                                           const std::string& propertyPath,
                                           const std::string& value);

    void compareObjectPropertyValue(::fwData::Object::sptr obj,
                                           const std::string& propertyPath,
                                           ::fwData::Object::sptr value);
} // namespace DataCampHelper
} // namespace ut
} // namespace arDataCamp

#endif // __ARDATACAMP_UT_DATACAMPHELPER_HPP__
