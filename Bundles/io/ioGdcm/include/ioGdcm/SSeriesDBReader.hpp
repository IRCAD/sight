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

#include "ioGdcm/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace fwJobs
{
class IJob;
}

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{
/**
 * @brief GDCM Reader (ImageSeries/ModelSeries)
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(::fwJobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * The reader can be configured as a regular reader.
 * You can select the SOP Classes that can be read using this reader.
 * It is also possible to define a filter that must be applied prior to the reading process.
 *
 * @code{.xml}
       <service uid="..." type="::ioGdcm::SSeriesDBReader" >
           <inout key="data" uid="..." />
           <config>
               <filterType>::fwDicomIOFilter::custom::NoFilter<filterType/>
               <enableBufferRotation>yes</enableBufferRotation>
               <showLogDialog>yes</showLogDialog>
               <dicomdirSupport>user_selection</dicomdirSupport>
               <SOPClassSelection>
                   <SOPClass name="CT Image Storage" uid="1.2.840.10008.5.1.4.1.1.2" />
               </SOPClassSelection>
           </config>
       </service>
 * @endcode
 *
 * On the other hand, you can specify a service configuration using the filterConfig element:
 *
 * @code{.xml}
       <service uid="..." type="::ioGdcm::SSeriesDBReader" >
           <inout key="data" uid="..." />
           <config>
               <filterConfig>MyFilterSelectorConfig<filterConfig/>
               <enableBufferRotation>yes</enableBufferRotation>
               <showLogDialog>yes</showLogDialog>
               <dicomdirSupport>user_selection</dicomdirSupport>
               <SOPClassSelection>
                   <SOPClass name="CT Image Storage" uid="1.2.840.10008.5.1.4.1.1.2" />
               </SOPClassSelection>
           </config>
       </service>
   @endcode
 *
 * @code{.xml}
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>MyFilterSelectorConfig</id>
        <service>::ioDicom::SFilterSelectorDialog</service>
        <desc>"Open" action's filter selector config</desc>
        <config>
            <selection mode="include" />
            <addSelection filter="::fwDicomIOFilter::custom::DefaultDicomFilter" />
            <addSelection filter="::fwDicomIOFilter::custom::NoFilter" />
        </config>
    </extension>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: object to read
 * @subsection Configuration Configuration:
 * - \b filterType: Define a filter that must be applied prior to the reading process.
 * - \b enableBufferRotation (optional, default set to yes): Enable buffer rotation
 * - \b showLogDialog (optional, default set to yes): Show log dialog
 * - \b dicomdirSupport (optional, default set to user_selection ): DicomDir support mode.
 * dicomdirSupport available mode:
 *    - always (always use the DicomDir if present)
 *    - never (never use the DicomDir)
 *    - user_selection (let the user decide whether using the DicomDir or not)
 * - \b SOPClassSelection:  List of SOP Class selection managed by the reader
 */
class IOGDCM_CLASS_API SSeriesDBReader : public ::fwIO::IReader
{

public:
    fwCoreServiceMacro(SSeriesDBReader,  ::fwIO::IReader);

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    /**
     * @brief   constructor
     *
     */
    IOGDCM_API SSeriesDBReader() noexcept;

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SSeriesDBReader() noexcept override;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::vector< std::string > SupportedSOPClassContainerType;

    enum DicomDirSupport
    {
        ALWAYS = 0,
        NEVER,
        USER_SELECTION
    };

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Override
    IOGDCM_API virtual void starting() override;

    /// Override
    IOGDCM_API virtual void stopping() override;

    /// Override
    IOGDCM_API void updating() override;

    /// Override
    IOGDCM_API void info(std::ostream& _sstream ) override;

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle() override;

    /// Override
    IOGDCM_API virtual void configureWithIHM() override;

    /// Return managed file type, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Create a seriesDB and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

    /// Selector config used to select a filter to apply
    std::string m_filterConfig;

    /// Selected filter key
    std::string m_filterType;

    /// Supported SOP Class selection
    SupportedSOPClassContainerType m_supportedSOPClassSelection;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Show log dialog
    bool m_showLogDialog;

    /// Enable buffer rotation
    bool m_enableBufferRotation;

    /// Specify how to use dicomdir files
    DicomDirSupport m_dicomDirSupport;
};

} // namespace ioGdcm
