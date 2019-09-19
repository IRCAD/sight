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

#include "ioData/config.hpp"

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{

/**
 * @brief   SAttachmentSeries reader service.
 * @deprecated This service will be removed in Sight 20.0.
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

    fwCoreServiceMacro(SAttachmentSeriesReader, ::fwIO::IReader);

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
