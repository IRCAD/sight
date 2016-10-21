/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODCMTK_SDICOMTAGSERIESDBREADER_HPP__
#define __IODCMTK_SDICOMTAGSERIESDBREADER_HPP__

#include "ioDcmtk/config.hpp"

#include <io/IReader.hpp>

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
class IODCMTK_CLASS_API SDicomTagSeriesDBReader : public ::io::IReader
{

public:
    typedef std::string ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceClassDefinitionsMacro ( (SDicomTagSeriesDBReader)( ::io::IReader) );
    /**
     * @brief   constructor
     *
     */
    IODCMTK_API SDicomTagSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IODCMTK_API virtual ~SDicomTagSeriesDBReader() throw();

protected:

    /// Override
    IODCMTK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API void updating() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API void info(std::ostream& _sstream );

    /// Override
    IODCMTK_API virtual std::string getSelectorDialogTitle();

    /// Override
    IODCMTK_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IODCMTK_API ::io::IOPathType getIOPathType() const;

private:

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

};

} // namespace ioDcmtk

#endif //__IODCMTK_SDICOMTAGSERIESDBREADER_HPP__
