/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
 * @brief   Specific service for the construction of a Composite and its associated services from an XML-based description.
 * @see     ::fwServices::IXMLParser
 */
class DATAREG_CLASS_API Composite : public ::fwServices::IXMLParser
{
public:

    fwCoreServiceClassDefinitionsMacro ( (Composite)(::fwServices::IXMLParser) );

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

    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj );

    DATAREG_API void startConfig();

    DATAREG_API void updateConfig();

    DATAREG_API void stopConfig();

    DATAREG_API void destroyConfig();

protected:

    /**
     * @brief   Updating method : create composite object.
     *
     * Parse the composite configuration element to configure and add its objects.
     */
    DATAREG_API virtual void updating( );

private:

    /// To verify some conditions in xml file
    bool refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    std::vector< ::fwServices::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace dataReg

#endif /* __DATAREG_PARSER_COMPOSITE_HPP__ */
