/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_PARSER_LIST_HPP__
#define __FWCOMED_PARSER_LIST_HPP__

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include <fwServices/IAppConfigManager.hpp>

#include "fwComEd/config.hpp"

namespace fwComEd
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a List and its associated services from an XML-based description.
 * @see     ::fwServices::IXMLParser
 */
class FWCOMED_CLASS_API List : public ::fwServices::IXMLParser
{
public:

    fwCoreServiceClassDefinitionsMacro ( (List)(::fwServices::IXMLParser) );

    /**
     * @brief   Constructor : does nothing.
     */
    List( )
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~List()
    {
    }

    FWCOMED_API void createConfig( ::fwTools::Object::sptr _obj );

    FWCOMED_API void startConfig();

    FWCOMED_API void updateConfig();

    FWCOMED_API void stopConfig();

    FWCOMED_API void destroyConfig();

protected:

    /**
     * @brief   Updating method : create List object.
     *
     * Parse the List configuration element to configure and add its objects.
     */
    FWCOMED_API virtual void updating( ) throw(fwTools::Failed);

private:

    /// To verify some conditions in xml file
    bool refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    std::vector< ::fwServices::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace fwcomEd

#endif /* __FWCOMED_PARSER_LIST_HPP__ */

