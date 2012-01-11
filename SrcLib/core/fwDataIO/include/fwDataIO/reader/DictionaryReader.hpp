/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_READER_DICTIONARY_HPP_
#define _FWDATAIO_READER_DICTIONARY_HPP_

#include <boost/shared_ptr.hpp>

#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

namespace fwData {
    class StructureTraitsDictionary;
}

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Dictionary reader. Read file with .dic extension.
 * @class   DictionaryReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011
 *
 * Ircad reader to read a ::fwData::StructureTraitsDictionary on filesystem which the file format
 * is .dic. This is an ascii file the structure of each line is defined on the first line of this file.
 */
class FWDATAIO_CLASS_API DictionaryReader:
   public GenericObjectReader< ::fwData::StructureTraitsDictionary>,
   public ::fwData::location::enableSingleFile< IObjectReader >

{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (DictionaryReader)(GenericObjectReader< ::fwData::StructureTraitsDictionary >),
                                            (()),
                                            new  DictionaryReader
                                            );

    /// Constructor. Do nothing .
    FWDATAIO_API DictionaryReader();

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~DictionaryReader();

    /// Read the file with standard iostream API.
    FWDATAIO_API virtual void read();

    FWDATAIO_API std::string  extension();
};

} // namespace reader

} // namespace fwDataIO


#endif // _FWDATAIO_READER_DICTIONARY_HPP_
