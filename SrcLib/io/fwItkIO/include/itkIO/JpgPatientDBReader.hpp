/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_JPGPATIENTDBREADER_HPP_
#define _FWITKIO_JPGPATIENTDBREADER_HPP_

#include <vector>
#include <boost/filesystem/path.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/Folder.hpp>

#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Patient.hpp>

#include "itkIO/config.hpp"

namespace itkIO
{

class JpgPatientDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >,
                           public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                           public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((JpgPatientDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >),
                                           (()),
                                           new JpgPatientDBReader
                                          );
    fwCoreAllowSharedFromThis();

    FWITKIO_API void read();

protected:

    FWITKIO_API JpgPatientDBReader();

    FWITKIO_API ~JpgPatientDBReader();

private :

    ::fwData::Patient::sptr createPatient( const ::boost::filesystem::path inrFilePath, int commonPathSize);

    void searchRecursivelyInrFile( const ::boost::filesystem::path & path, ::std::vector< ::boost::filesystem::path > & inrFiles );

    std::string findCommonPath( ::std::vector< ::boost::filesystem::path > & inrFilePathes );

};

} // namespace itkIO

#endif /*_FWITKIO_JPGPATIENTDBREADER_HPP_*/
