/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IXMLPARSER_HPP_
#define IXMLPARSER_HPP_

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
 * @author IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API IXMLParser : public ::fwServices::IService
{
public :

    fwCoreServiceClassDefinitionsMacro ( (IXMLParser)(::fwServices::IService) ) ;

public :

    FWSERVICES_API void setObjectConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem );

    FWSERVICES_API virtual void create( ::fwTools::Object::sptr _obj );

    FWSERVICES_API virtual void start();

    FWSERVICES_API virtual void update();

    FWSERVICES_API virtual void stop();

    FWSERVICES_API virtual void destroy();

    ::fwRuntime::ConfigurationElement::sptr m_cfg;

protected:

    /**
     * @brief Constructor. Does nothing.
     */
    FWSERVICES_API IXMLParser( ) ;

    /**
     * @brief Destructor. Does nothing.
     */
    FWSERVICES_API virtual ~IXMLParser() ;

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void starting() throw(::fwTools::Failed) ;

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void stopping() throw(::fwTools::Failed) ;

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void configuring() throw(::fwTools::Failed) ;

    /**
     * @brief Does nothing
     */
    FWSERVICES_API virtual void updating( fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) ;

    /**
     * @brief Parse the XML configuration
     * @note Should invoked ( this->::IXMLParser::updating() ) from specific XMLParsers updating method to support both specific compositions and this generic one
     */
    FWSERVICES_API virtual void updating() throw(::fwTools::Failed) ;
};




}

#endif /* IXMLPARSER_HPP_ */
