/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioData/config.hpp"

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{

/**
 * @brief   SAttachmentSeries reader service.
 * @deprecated This service will be removed in FW4SPL 20.0.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::ioData::SAttachmentSeriesReader" autoConnect="no">
        <inout key="data" uid="..."/>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: SeriesDB updated with AttachmentSeries.
 */
class IODATA_CLASS_API SAttachmentSeriesReader : public ::fwIO::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAttachmentSeriesReader)(::fwIO::IReader) );

    /// Super class of reader services
    typedef ::fwIO::IReader SuperClass;

    /// Constructor : does nothing
    IODATA_API SAttachmentSeriesReader();

    /** @name Specified reader service methods ( override from ::fwIO::IWriter )
     * @{
     */

    /// Returns  (filename) extension
    IODATA_API virtual std::vector< std::string > getSupportedExtensions() override;

    /**
     * @brief Configure the attachment path.
     *
     * This method is used to find the file path  using a file selector.
     */
    IODATA_API void configureWithIHM() override;
    /// @}

    /// Returns path type managed by the service, here FILE
    IODATA_API virtual ::fwIO::IOPathType getIOPathType() const override;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    virtual void starting() override
    {
    }

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    virtual void stopping() override
    {
    }

    /**
     * @brief Configuring method : calls implementation from `io::IReader`
     */
    IODATA_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     */
    IODATA_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IODATA_API virtual void info(std::ostream& _sstream ) override;
    /// @}

};

} //namespace ioData
