/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioDcmtk/config.hpp"

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace ioDcmtk
{
/**
 * @brief   SDicomTagSeriesDBReader reads a folder that has DICOM images.
 *
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="::ioDcmtk::SDicomTagSeriesDBReader" >
        <inout key="data" uid="..."/>
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: SeriesDB
 */
class IODCMTK_CLASS_API SDicomTagSeriesDBReader : public ::fwIO::IReader
{

public:
    typedef std::string ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceClassDefinitionsMacro( (SDicomTagSeriesDBReader)( ::fwIO::IReader) );
    /**
     * @brief   constructor
     *
     */
    IODCMTK_API SDicomTagSeriesDBReader() noexcept;

    /**
     * @brief   destructor
     */
    IODCMTK_API virtual ~SDicomTagSeriesDBReader() noexcept;

protected:

    /// Override
    IODCMTK_API virtual void starting() override;

    /// Override
    IODCMTK_API virtual void stopping() override;

    /// Override
    IODCMTK_API virtual void configuring() override;

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

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

};

} // namespace ioDcmtk
