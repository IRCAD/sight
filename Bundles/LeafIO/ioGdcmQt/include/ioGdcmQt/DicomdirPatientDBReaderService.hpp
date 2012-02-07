/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCMQT_DICOMDIRPATIENTDBREADERSERVICE_HPP_
#define _IOGDCMQT_DICOMDIRPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <map>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include <ioGdcmQt/config.hpp>
#include <ioGdcmQt/Types.hpp>
#include <ioGdcmQt/ui/DicomdirEditor.hpp>

namespace fwData
{
    class PatientDB;
}



namespace ioGdcm
{

/**
 * @brief   This class read DICOMDIR file
 * @class   DicomdirPatientDBReaderService
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 *
 * \note First verison.
 */
class IOGDCMQT_CLASS_API DicomdirPatientDBReaderService : public ::io::IReader
{

public :

    fwCoreServiceClassDefinitionsMacro ( (DicomdirPatientDBReaderService)( ::io::IReader) ) ;
    /**
     * @brief   constructor
     *
     */
    IOGDCMQT_API DicomdirPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOGDCMQT_API virtual ~DicomdirPatientDBReaderService() throw();

protected:



    std::map < ::ioGdcm::DicomReader, std::string > readerList;

    /// Override
    IOGDCMQT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IOGDCMQT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCMQT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCMQT_API void updating() throw(::fwTools::Failed);

    /// Override
    IOGDCMQT_API virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Override
    IOGDCMQT_API void info(std::ostream &_sstream ) ;

    /// Override
    IOGDCMQT_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOGDCMQT_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOGDCMQT_API virtual void configureWithIHM();

private :

    void notificationOfDBUpdate();
    template < typename READER> ::fwData::PatientDB::sptr createPatientDB(const std::vector< ::boost::filesystem::path>& filenames);

    bool m_bServiceIsConfigured;

    /**
     * @brief   path of the selected dicomdir.
     */
    ::boost::filesystem::path m_dicomdirPath;
    /**
     * @brief   keep the selected reader and the list of serie's files.
     */
    std::pair< ioGdcm::DicomReader, std::vector< ::boost::filesystem::path> > m_readerAndFilenames;

    DicomReader selectedReader;
};

} // namespace ioGdcmQt

#endif // _IOGDCMQT_DICOMDIRPATIENTDBREADERSERVICE_HPP_
