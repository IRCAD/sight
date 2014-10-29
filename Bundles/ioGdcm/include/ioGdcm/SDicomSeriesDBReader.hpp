/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SDICOMSERIESDBREADER_HPP__
#define __IOGDCM_SDICOMSERIESDBREADER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioGdcm/config.hpp"

namespace fwMedData
{
    class SeriesDB;
    class Patient;
}

namespace ioGdcm
{

class IOGDCM_CLASS_API SDicomSeriesDBReader : public ::io::IReader
{

public :
    typedef std::string                  ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesDBReader)( ::io::IReader) ) ;
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

    /// Override
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed){};

    /// Override
    IOGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOGDCM_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private :
    /// Notify the addition of a Series in the SeriesDB
    void notificationOfDBUpdate();

    /**
     * @brief Create a seriesDB and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir) const;

};

} // namespace ioGdcm

#endif //__IOGDCM_SDICOMSERIESDBREADER_HPP__
