/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_OBJECTCONFIGURATIONPARSER_HPP_
#define _FWCOMED_OBJECTCONFIGURATIONPARSER_HPP_

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{
/**
 * @brief   Specific service for the construction of a ProcessObject and its associated services from an XML-based description.
 * @class   IProcessObjectParser
 * @author  IRCAD (Research and Development Team).

 * @date    2007-2009
 * @see     ::fwServices::IXMLParser
 */
class FWCOMED_CLASS_API IProcessObjectParser : public ::fwServices::IXMLParser
{
public :
    fwCoreServiceClassDefinitionsMacro ( (IProcessObjectParser)(::fwServices::IXMLParser) ) ;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API IProcessObjectParser( ) {};

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~IProcessObjectParser() {};

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    FWCOMED_API virtual void updating( ) throw(fwTools::Failed) ;

protected :

    /**
     * @brief       Configure inputs and outputs and add them in the process object.
     * @param[in]   io inputs/outputs
     * @param[in]   list inputs/outputs configuration
     */
    void configureIO( ::fwTools::Object::sptr io, const ::fwRuntime::ConfigurationElement::sptr list) ;
};

/**
 * @brief   Specific service for the construction of a Composite and its associated services from an XML-based description.
 * @class   ICompositeParser
 * @author  IRCAD (Research and Development Team).

 * @date    2007-2009
 * @see     ::fwServices::IXMLParser
 */
class FWCOMED_CLASS_API ICompositeParser : public ::fwServices::IXMLParser
{
public :

    fwCoreServiceClassDefinitionsMacro ( (ICompositeParser)(::fwServices::IXMLParser) ) ;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API ICompositeParser( ) {};

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~ICompositeParser() {};

    /**
     * @brief   Updating method : create composite object.
     *
     * Parse the composite configuratiob element to configure and add its objets.
     */
    FWCOMED_API virtual void updating( ) throw(fwTools::Failed) ;
};


}

#endif /* _FWCOMED_OBJECTCONFIGURATIONPARSER_HPP_ */
