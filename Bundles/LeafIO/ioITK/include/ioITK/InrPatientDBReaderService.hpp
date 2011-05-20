/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_INRPATIENTDBREADERSERVICE_HPP_
#define _IOITK_INRPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>

#include <io/IReader.hpp>
#include <itkIO/InrPatientDBReader.hpp>

#include "ioITK/export.hpp"


namespace itkIO
{
class InrPatientDBReader;
}

namespace ioITK
{

class IOITK_CLASS_API InrPatientDBReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (InrPatientDBReaderService)( ::io::IReader) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOITK_API InrPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOITK_API virtual ~InrPatientDBReaderService() throw();

    /// Override
    IOITK_API virtual void configuring() throw(::fwTools::Failed) ;

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOITK_API void updating() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Override
    IOITK_API virtual std::string getSelectorDialogTitle();

private :

    void notificationOfDBUpdate();

    ::fwData::PatientDB::sptr createPatientDB();

    bool m_bServiceIsConfigured;

    std::vector< ::boost::filesystem::path> m_files;
    ::boost::filesystem::path m_folder;
    bool m_isRecursive;
};

} // namespace ioITK

#endif //_IOITK_INRPATIENTDBREADERSERVICE_HPP_
