/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTKGDCM_SSERIESDBREADER_HPP__
#define __IOVTKGDCM_SSERIESDBREADER_HPP__

#include "ioVtkGdcm/config.hpp"

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace fwJobs
{
class IJob;
}

namespace ioVtkGdcm
{

class IOVTKGDCM_CLASS_API SSeriesDBReader : public ::io::IReader
{

public:
    typedef std::string ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    fwCoreServiceClassDefinitionsMacro( (SSeriesDBReader)( ::io::IReader) );
    /**
     * @brief   constructor
     *
     */
    IOVTKGDCM_API SSeriesDBReader() noexcept;

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~SSeriesDBReader() noexcept;

protected:

    /// Override
    IOVTKGDCM_API virtual void starting() override;

    /// Override
    IOVTKGDCM_API virtual void stopping() override;

    /// Override
    IOVTKGDCM_API virtual void configuring() override;

    /// Override
    IOVTKGDCM_API void updating() override;

    /// Override
    IOVTKGDCM_API void info(std::ostream& _sstream ) override;

    /// Override
    IOVTKGDCM_API virtual ExtensionsType getSupportedExtensions() override;

    /// Override
    IOVTKGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOVTKGDCM_API virtual void configureWithIHM() override;

    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::io::IOPathType getIOPathType() const override;

private:

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVtkGdcm

#endif //__IOVTKGDCM_SSERIESDBREADER_HPP__
