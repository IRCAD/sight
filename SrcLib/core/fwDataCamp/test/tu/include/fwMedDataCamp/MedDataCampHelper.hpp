/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATACAMP_UT_MEDDATACAMPHELPER_HPP__
#define __FWMEDDATACAMP_UT_MEDDATACAMPHELPER_HPP__

#include <fwData/Object.hpp>

namespace fwMedDataCamp
{
namespace ut
{

class MedDataCampHelper
{
public:
    typedef std::vector<std::string> PropertiesNameType;

    static void visitProperties(const std::string& className, const PropertiesNameType& vecProp);

    static void compareSimplePropertyValue(::fwData::Object::sptr obj,
                                           const std::string& propertyPath,
                                           const std::string& value);

    static void compareObjectPropertyValue(::fwData::Object::sptr obj,
                                           const std::string& propertyPath,
                                           ::fwData::Object::sptr value);
};

} //namespace ut
} //namespace fwMedDataCamp

#endif // __FWMEDDATACAMP_UT_MEDDATACAMPHELPER_HPP__
