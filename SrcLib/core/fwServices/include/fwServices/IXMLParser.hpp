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

#include "fwServices/IService.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/Object.hpp>

namespace fwServices
{

/**
 * @brief Service type for the construction of an object and associated services from an XML-based description
 *
 * This includes (data) object attributes initialization as well as XML declared service creation, attachment and
 * configuration.
 * As each specific (data) object has a specific structure (attributes), it must be specialized for each one. Such a
 * service is used by the factory
 * New(::fwRuntime::ConfigurationElement::sptr ) method.
 *
 * The updating() method of this base class parses the XML description: each object named XML children corresponds to a
 * field
 * added to the object (see ::fwTools::Object). The New(::fwRuntime::ConfigurationElement::sptr ) method on the related
 * child is invoked, therefore allowing to build
 * tree like composite object which services.
 *
 *
 */
class FWSERVICES_CLASS_API IXMLParser : public ::fwServices::IService
{
public:

    fwCoreServiceMacro(IXMLParser, ::fwServices::IService);

public:

    FWSERVICES_API void setObjectConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem );

    FWSERVICES_API virtual void createConfig( ::fwTools::Object::sptr _obj );

    FWSERVICES_API virtual void startConfig();

    FWSERVICES_API virtual void updateConfig();

    FWSERVICES_API virtual void stopConfig();

    FWSERVICES_API virtual void destroyConfig();

    ::fwRuntime::ConfigurationElement::csptr m_cfg;

protected:

    /**
     * @brief Constructor. Does nothing.
     */
    FWSERVICES_API IXMLParser( );

    /**
     * @brief Destructor. Does nothing.
     */
    FWSERVICES_API virtual ~IXMLParser();

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void starting() override;

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void stopping() override;

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void configuring() override;

    /**
     * @brief Parse the XML configuration
     * @note Should invoked ( this->::IXMLParser::updating() ) from specific XMLParsers updating method to support both
     *       specific compositions and this generic one
     */
    FWSERVICES_API virtual void updating() override;
};

}
