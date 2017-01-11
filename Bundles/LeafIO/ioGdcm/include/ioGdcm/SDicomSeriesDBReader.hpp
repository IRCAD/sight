/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SDICOMSERIESDBREADER_HPP__
#define __IOGDCM_SDICOMSERIESDBREADER_HPP__

#include "ioGdcm/config.hpp"

#include <fwCom/Signal.hpp>
#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwJobs
{
class IJob;
}

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace ioGdcm
{

/**
 * @brief Read DicomSeries from DICOM with gdcm reader
 **/
class IOGDCM_CLASS_API SDicomSeriesDBReader : public ::io::IReader
{

public:
    typedef std::string ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    typedef ::boost::filesystem::path PathType;
    typedef ::fwCom::Signal< void ( bool, std::vector< PathType > ) > FilesAddedSignal;

    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesDBReader)( ::io::IReader) );
    /**
     * @brief   constructor
     *
     */
    IOGDCM_API SDicomSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SDicomSeriesDBReader() throw();

protected:

    /// Enum for DicomDir support mode
    enum DicomDirSupport
    {
        ALWAYS = 0,     /*! Always use the DicomDir if present */
        NEVER,          /*! Never use the DicomDir */
        USER_SELECTION  /*! Let the user decide whether using the DicomDir or not */
    };

    /// Enum for reading mode
    enum ReaderMode
    {
        DIRECT = 0,         /*! Read directly from the source folder */
        COPY,               /*! Read after a copy of the source files */
        USER_SELECTION_MODE /*! Let the user decide whether read directly or copy before reading */
    };


    /// Override
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void info(std::ostream& _sstream );

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle();

    /**
     * The reader can be configured as a regular reader.
     * You can also specify how dicomdir support is handled
     * @code{.xml}
       <config>
       <dicomdirSupport>always|never|user_selection</dicomdirSupport> <!-- optional, default set to user_selection -->
       <mode>direct|copy|user_selection</mode> <!-- optional, default set to user_selection -->
       </config>
       @endcode
     */
    IOGDCM_API virtual void configuring() throw (fwTools::Failed);

    /// Override
    IOGDCM_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private:

    /**
     * @brief Create a seriesDB and fill it using the data of the DICOM files
     * @param[in] checkIsDicom If set to true, file must be checked for DICOM information
     * @param[in] dicomDir DICOM folder
     */
    SPTR(::fwMedData::SeriesDB) createSeriesDB(bool checkIsDicom, const ::boost::filesystem::path& dicomDir);

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Signal emitted when files have been added in the read DicomSeries
    SPTR(FilesAddedSignal) m_sigFilesAdded;

    /// Cancel information for jobs
    bool m_cancelled;

    /// Show log dialog
    bool m_showLogDialog;

    /// Specify how to use dicomdir files
    DicomDirSupport m_dicomDirSupport;

    /// Specify how to handle reading
    ReaderMode m_readerMode;


};

} // namespace ioGdcm

#endif //__IOGDCM_SDICOMSERIESDBREADER_HPP__
