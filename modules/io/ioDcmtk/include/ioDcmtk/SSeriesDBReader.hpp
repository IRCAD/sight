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

#include "ioDcmtk/config.hpp"

#include <fwIO/IReader.hpp>

#include <filesystem>

#include <string>

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace ioDcmtk
{

/**
 * @brief  Read ImageSeries from DICOM with dcmtk
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::ioDcmtk::SSeriesDBReader" autoConnect="no" worker="readerDicom">
        <inout key="data" uid="..."/>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: dicom seriesDB to read.
 */
class IODCMTK_CLASS_API SSeriesDBReader : public ::fwIO::IReader
{

public:
    fwCoreServiceMacro(SSeriesDBReader,  ::fwIO::IReader)
    /**
     * @brief   constructor
     *
     */
    IODCMTK_API SSeriesDBReader() noexcept;

    /**
     * @brief   destructor
     */
    IODCMTK_API virtual ~SSeriesDBReader() noexcept;

protected:

    typedef std::vector< std::string > SupportedSOPClassContainerType;

    /**
     * The reader can be configured as a regular reader.
     * You can select the SOP Classes that can be read using this reader.
     * It is also possible to define a filter that must be applied prior the reading process.
     * @code{.xml}
       <config filterType="::fwDicomIOFilter::custom::NoFilter" />
       <SOPClassSelection>
           <SOPClass name="CT Image Storage" uid="1.2.840.10008.5.1.4.1.1.2" />
       </SOPClassSelection>
       @endcode
     * On the other hand, you can specify a service configuration using the FilterSelectorSrvConfig element:
     * @code{.xml}
        <extension implements="::fwServices::registry::ServiceConfig">
            <id>FilterSelectorConfig</id>
            <service>::ioDicom::SFilterSelectorDialog</service>
            <desc>"Open" action's filter selector config</desc>
            <config>
                <selection mode="include" />
                <addSelection filter="::fwDicomIOFilter::custom::DefaultDicomFilter" />
                <addSelection filter="::fwDicomIOFilter::custom::NoFilter" />
            </config>
        </extension>
       @endcode
     */
    IODCMTK_API virtual void configuring() override;

    /// Override
    IODCMTK_API virtual void starting() override;

    /// Override
    IODCMTK_API virtual void stopping() override;

    /// Override
    IODCMTK_API void updating() override;

    /// Override
    IODCMTK_API void info(std::ostream& _sstream ) override;

    /// Override
    IODCMTK_API virtual std::string getSelectorDialogTitle() override;

    /// Override
    IODCMTK_API virtual void configureWithIHM() override;

    /// Return path type managed by the service, here FOLDER
    IODCMTK_API ::fwIO::IOPathType getIOPathType() const override;

private:

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const std::filesystem::path& dicomDir);

    /// Selector config used to select a filter to apply
    std::string m_filterSelectorSrvConfig;

    /// Selected filter key
    std::string m_filterType;

    /// Supported SOP Class selection
    SupportedSOPClassContainerType m_supportedSOPClassSelection;

};

} // namespace ioDcmtk
