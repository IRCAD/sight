/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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
