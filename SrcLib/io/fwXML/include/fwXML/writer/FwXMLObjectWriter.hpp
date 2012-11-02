/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_WRITER_FWXMLOBJECTWRITER_HPP_
#define _FWXML_WRITER_FWXMLOBJECTWRITER_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/writer/IObjectWriter.hpp>
#include <fwDataIO/writer/factory/new.hpp>

#include "fwXML/config.hpp"
#include <fwData/location/SingleFile.hpp>


namespace fwData {
    class Object;
}

namespace fwXML
{

namespace writer
{

class FWXML_CLASS_API FwXMLObjectWriter : public ::fwDataIO::writer::IObjectWriter,
                                            public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                                            public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithNFactoriesMacro( (FwXMLObjectWriter)(::fwDataIO::writer::IObjectWriter),
                                              ((::fwDataIO::writer::factory::New< FwXMLObjectWriter > ,() ))
                                              (( ObjectWriterFactory, ((bool)) ))
                                             );

    FWXML_API FwXMLObjectWriter(::fwDataIO::writer::IObjectWriter::Key key);

    FWXML_API ~FwXMLObjectWriter();

    FWXML_API void write();

    FWXML_API std::string  extension();
protected:

    FWXML_API static sptr ObjectWriterFactory(bool saveSchema);

    bool m_saveSchema;

};

} // namespace writer

} // namespace fwXML


#endif // _FWXML_WRITER_FWXMLOBJECTWRITER_HPP_
