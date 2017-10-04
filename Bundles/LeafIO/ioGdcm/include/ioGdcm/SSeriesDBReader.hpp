/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SSERIESDBREADER_HPP__
#define __IOGDCM_SSERIESDBREADER_HPP__

#include "ioGdcm/config.hpp"

#include <fwCom/Signal.hpp>

#include <io/IReader.hpp>

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
 * @brief Read  ImageSeries from DICOM with gdcm reader
 **/
class IOGDCM_CLASS_API SSeriesDBReader : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (SSeriesDBReader)( ::io::IReader) );

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    /**
     * @brief   constructor
     *
     */
    IOGDCM_API SSeriesDBReader() noexcept;

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SSeriesDBReader() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::vector< std::string > SupportedSOPClassContainerType;

    enum DicomDirSupport
    {
        ALWAYS = 0,
        NEVER,
        USER_SELECTION
    };

    /**
     * The reader can be configured as a regular reader.
     * You can select the SOP Classes that can be read using this reader.
     * It is also possible to define a filter that must be applied prior to the reading process.
     * @code{.xml}
       <config filterType="::fwDicomIOFilter::custom::NoFilter" />
       <enableBufferRotation>yes|no</enableBufferRotation> <!-- optional, default set to yes -->
       <showLogDialog>yes|no</showLogDialog> <!-- optional, default set to yes -->
       <dicomdirSupport>always|never|user_selection</dicomdirSupport> <!-- optional, default set to user_selection -->
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
    IOGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOGDCM_API virtual void configureWithIHM() override;

    /// Return managed file type, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Create a seriesDB and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path & dicomDir);

    /// Selector config used to select a filter to apply
    std::string m_filterSelectorSrvConfig;

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

#endif //__IOGDCM_SSERIESDBREADER_HPP__
