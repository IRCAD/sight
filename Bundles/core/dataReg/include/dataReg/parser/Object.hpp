/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DATAREG_PARSER_OBJECT_HPP__
#define __DATAREG_PARSER_OBJECT_HPP__

#include "dataReg/config.hpp"

#include <fwServices/IAppConfigManager.hpp>
#include <fwServices/IXMLParser.hpp>

#include <fwTools/Object.hpp>

namespace dataReg
{
namespace parser
{

/**
 * @brief Specific ::fwTools::Object parser. Does nothing as no IService method is specialized: IXMLParser base class
 *  methods are therefore considered.
 *
 * @note This implementation is only written to avoid the mix between the service type IXMLParser and the
 * ::fwTools::Object implementation (for homogeneity).
 *
 */
class DATAREG_CLASS_API Object : public ::fwServices::IXMLParser
{
public:
    fwCoreServiceClassDefinitionsMacro( (Object)(::fwServices::IXMLParser) );

    DATAREG_API Object( );
    DATAREG_API virtual ~Object();

    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj ) override;
    DATAREG_API void startConfig() override;
    DATAREG_API void updateConfig() override;
    DATAREG_API void stopConfig() override;
    DATAREG_API void destroyConfig() override;

protected:

    bool refObjectValidator( ::fwRuntime::ConfigurationElement::csptr _cfgElement );
    void updating() override;

    std::vector< ::fwServices::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace dataReg

#endif /* __DATAREG_PARSER_OBJECT_HPP__ */
