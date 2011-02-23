/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_PARSER_COMPOSITE_HPP_
#define _FWCOMED_PARSER_COMPOSITE_HPP_

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include <fwServices/AppConfigManager.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a Composite and its associated services from an XML-based description.
 * @class   Composite
 * @author  IRCAD (Research and Development Team).

 * @date    2007-2009
 * @see     ::fwServices::IXMLParser
 */
class FWCOMED_CLASS_API Composite : public ::fwServices::IXMLParser
{
public :

    fwCoreServiceClassDefinitionsMacro ( (Composite)(::fwServices::IXMLParser) ) ;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API Composite( ) {};

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~Composite() {};

    FWCOMED_API void createConfig( ::fwTools::Object::sptr _obj );

    FWCOMED_API void startConfig();

    FWCOMED_API void updateConfig();

    FWCOMED_API void stopConfig();

    FWCOMED_API void destroyConfig();

protected:

    /**
     * @brief   Updating method : create composite object.
     *
     * Parse the composite configuration element to configure and add its objects.
     */
    FWCOMED_API virtual void updating( ) throw(fwTools::Failed) ;

private :

    /// To verify some conditions in xml file
    bool refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    std::vector< ::fwServices::AppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace fwcomEd

#endif /* _FWCOMED_PARSER_COMPOSITE_HPP_ */
