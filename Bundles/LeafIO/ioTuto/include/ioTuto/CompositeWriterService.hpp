/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOTUTO_COMPOSITEWRITERSERVICE_HPP_
#define _IOTUTO_COMPOSITEWRITERSERVICE_HPP_

#include "ioTuto/config.hpp"
#include "io/IWriter.hpp"

namespace ioTuto
{
/**
 * @brief   Write a composite data structure from Xml file.
 * @class   CompositeWriteService.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 */
class IOTUTO_CLASS_API CompositeWriterService : public ::io::IWriter
{
public:
    fwCoreServiceClassDefinitionsMacro ( (CompositeWriterService)( ::io::IWriter) ) ;
    /// Constructor. Do nothing (Just initialize parameters).
    IOTUTO_API CompositeWriterService() throw() ;

    /// Destructor. Do nothing.
    IOTUTO_API virtual ~CompositeWriterService() throw() ;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     * @note    Call action base class configuring() method.
     */
    IOTUTO_API void configuring() throw( ::fwTools::Failed ) ;

    /**
     * @brief  Starting method. Call base class starting() method.
     */
    IOTUTO_API void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief  Stopping method. Call base class stopping() method.
     */
    IOTUTO_API void stopping() throw( ::fwTools::Failed ) ;

    /**
     * @brief  Write the composite to xml file.
     *
     * Use the ::fwXML::writer::FwXMLObjectWriter which propose xml writer services.
     */
    IOTUTO_API void updating() throw(::fwTools::Failed) ;

    /**
     * @brief React on modifications : does nothing.
     */
    IOTUTO_API void updating(::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg) throw(::fwTools::Failed) ;

    /**
     * @brief Give a short information about the action..
     */
    IOTUTO_API void info(std::ostream &_sstream ) ;

    /**
     * @brief Display a selector to choose the path and the name of file to write.
     */
    IOTUTO_API void configureWithIHM();
    /// @}

private:

    ::boost::filesystem::path m_fsExternalDataPath;

};

} // namespace ioTuto

#endif // _IOTUTO_COMPOSITEWRITERSERVICE_HPP_
