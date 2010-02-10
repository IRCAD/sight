/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_READER_FWXMLOBJECTREADER_HPP_
#define _FWXML_READER_FWXMLOBJECTREADER_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwData/location/SingleFile.hpp>

#include "fwXML/config.hpp"
#include <fwDataIO/reader/IObjectReader.hpp>
#include <fwTools/ProgressAdviser.hpp>

namespace fwData {
    class Object;
}

namespace fwXML
{

namespace reader
{

class FWXML_CLASS_API FwXMLObjectReader :  public ::fwDataIO::reader::IObjectReader,
                                              public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                                              public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(     (FwXMLObjectReader)(::fwDataIO::reader::IObjectReader::Baseclass),
                                                 (()),
                                                 new  FwXMLObjectReader
                                            );

    FWXML_API FwXMLObjectReader();

    FWXML_API ~FwXMLObjectReader();

    FWXML_API void read();

    FWXML_API std::string  extension();

private :

    // purposely not exposeded FwXMLObjectReader can load any kind of object
    // so its impossible to grant that this->setObject and this->getObject are the same after reading
    void setObject( ::boost::shared_ptr< ::fwTools::Object > _pObject ) {}

    //  need to keep a reference on it else exipred // TO BE FIXED in #739
    ::boost::shared_ptr< ::fwTools::Object > m_pObject;

};

} // namespace reader

} // namespace fwXML


#endif // _FWXML_READER_FWXMLOBJECTREADER_HPP_
