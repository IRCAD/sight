/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_IXMLPARSER_HPP__
#define __FWSERVICES_IXMLPARSER_HPP__

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @class IXMLParser
 * @brief Service type for the construction of an object and associated services from an XML-based description
 *
 * This includes (data) object attributes initialization as well as XML declared service creation, attachment and configuration.
 * As each specific (data) object has a specific structure (attributes), it must be specialized for each one. Such a service is used by the factory
 * New(::fwRuntime::ConfigurationElement::sptr ) method.
 *
 * The updating() method of this base class parses the XML description: each object named XML children corresponds to a field
 * added to the object (see ::fwTools::Object). The New(::fwRuntime::ConfigurationElement::sptr ) method on the related child is invoked, therefore allowing to build
 * tree like composite object which services.
 *
 *
 */
class FWSERVICES_CLASS_API IXMLParser : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (IXMLParser)(::fwServices::IService) );

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
    FWSERVICES_API virtual void starting();

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void stopping();

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void configuring();

    /**
     * @brief Parse the XML configuration
     * @note Should invoked ( this->::IXMLParser::updating() ) from specific XMLParsers updating method to support both
     *       specific compositions and this generic one
     */
    FWSERVICES_API virtual void updating();
};




}

#endif /* __FWSERVICES_IXMLPARSER_HPP__ */
