/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "dataReg/config.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

namespace dataReg
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a GenericField and its associated services from an XML-based
 * description.
 * @see     ::fwServices::IXMLParser
 */
class DATAREG_CLASS_API GenericField  : public ::fwServices::IXMLParser
{

public:

    fwCoreServiceMacro(GenericField, ::fwServices::IXMLParser);

    /// Constructor : does nothing.
    GenericField()
    {
    }

    /// Destructor : does nothing.
    virtual ~GenericField()
    {
    }

    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj ) override;
protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    DATAREG_API virtual void updating( ) override;

};

/**
 * @brief Specialisation of GenericField parser for boolean
 */
class BooleanParser : public GenericField
{
public:
    fwCoreServiceMacro(BooleanParser, ::fwServices::IXMLParser);
};

/**
 * @brief Specialisation of GenericField parser for integer
 */
class IntegerParser : public GenericField
{
public:
    fwCoreServiceMacro(IntegerParser, ::fwServices::IXMLParser);
};

/**
 * @brief Specialisation of GenericField parser for float
 */
class FloatParser : public GenericField
{
public:
    fwCoreServiceMacro(FloatParser, ::fwServices::IXMLParser);
};

/**
 * @brief Specialisation of GenericField parser for string
 */
class StringParser : public GenericField
{
public:
    fwCoreServiceMacro(StringParser, ::fwServices::IXMLParser);
};

} //namespace parser
} //namespace dataReg
