/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/assign/std/vector.hpp>

#include <fwData/GenericFieldBase.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <fwDataCamp/getObject.hpp>

#include "DataCampHelper.hpp"

namespace arDataCamp
{
namespace ut
{
namespace DataCampHelper
{

//------------------------------------------------------------------------------

void visitProperties(const std::string& className, const PropertiesNameType& vecProp)
{
    const ::camp::Class& metaClass = ::camp::classByName(className);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "wrong number of properties",  vecProp.size(), metaClass.propertyCount());
    for(const std::string& property : vecProp)
    {
        CPPUNIT_ASSERT_MESSAGE("Missing property "+ property+" in "+className,
                               metaClass.hasProperty(property));
    }
}

//------------------------------------------------------------------------------

void compareSimplePropertyValue(::fwData::Object::sptr obj,
                                const std::string& propertyPath,
                                const std::string& value)
{
    ::fwData::GenericFieldBase::sptr field;
    field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, propertyPath);
    CPPUNIT_ASSERT_MESSAGE("Retrieve failed for property "+propertyPath, field);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Retrieve property "+propertyPath+" not equal with value.", value, field->toString());
}

//------------------------------------------------------------------------------

void compareObjectPropertyValue(::fwData::Object::sptr obj,
                                const std::string& propertyPath,
                                ::fwData::Object::sptr value)
{
    ::fwData::Object::sptr subObj;
    subObj = ::fwDataCamp::getObject(obj, propertyPath);
    CPPUNIT_ASSERT_MESSAGE("Retrieve failed for property "+propertyPath, subObj);
    CPPUNIT_ASSERT_MESSAGE("Retrieve property "+propertyPath+" not equal with value.", value == subObj);
}

//------------------------------------------------------------------------------

} // namespace DataCampHelper
} // namespace ut
} // namespace arDataCamp

