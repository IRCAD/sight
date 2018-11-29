/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __DATAREG_PARSER_COMPOSITE_HPP__
#define __DATAREG_PARSER_COMPOSITE_HPP__

#include "dataReg/config.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IAppConfigManager.hpp>
#include <fwServices/IXMLParser.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

namespace dataReg
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a Composite and its associated services from an XML-based
 * description.
 * @see     ::fwServices::IXMLParser
 */
class DATAREG_CLASS_API Composite : public ::fwServices::IXMLParser
{
public:

    fwCoreServiceClassDefinitionsMacro( (Composite)(::fwServices::IXMLParser) );

    /**
     * @brief   Constructor : does nothing.
     */
    Composite( )
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Composite()
    {
    }

    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj ) override;

    DATAREG_API void startConfig() override;

    DATAREG_API void updateConfig() override;

    DATAREG_API void stopConfig() override;

    DATAREG_API void destroyConfig() override;

protected:

    /**
     * @brief   Updating method : create composite object.
     *
     * Parse the composite configuration element to configure and add its objects.
     */
    DATAREG_API virtual void updating( ) override;

private:

    /// To verify some conditions in xml file
    bool refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    std::vector< ::fwServices::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace dataReg

#endif /* __DATAREG_PARSER_COMPOSITE_HPP__ */
