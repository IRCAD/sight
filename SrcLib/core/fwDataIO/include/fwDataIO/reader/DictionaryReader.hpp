/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_READER_DICTIONARYREADER_HPP__
#define __FWDATAIO_READER_DICTIONARYREADER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/StructureTraitsDictionary.hpp>

#include <boost/filesystem.hpp>

namespace fwData
{
class StructureTraitsDictionary;
}

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Dictionary reader. Read file with .dic extension.
 *
 *
 * Ircad reader to read a ::fwData::StructureTraitsDictionary on filesystem which the file format
 * is .dic. This is an ascii file the structure of each line is defined on the first line of this file.
 */
class FWDATAIO_CLASS_API DictionaryReader :
    public GenericObjectReader< ::fwData::StructureTraitsDictionary>,
    public ::fwData::location::enableSingleFile< IObjectReader >

{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (DictionaryReader)(GenericObjectReader< ::fwData::StructureTraitsDictionary >),
        (()),
        ::fwDataIO::reader::factory::New< DictionaryReader >
        );

    /// Constructor. Do nothing .
    FWDATAIO_API DictionaryReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~DictionaryReader();

    /// Read the file with standard iostream API.
    FWDATAIO_API virtual void read() override;

    FWDATAIO_API std::string extension() override;

    /// Returns the default dictionary path ( file in rc directory path of fwDataIO library )
    FWDATAIO_API static ::boost::filesystem::path  getDefaultDictionaryPath();
};

} // namespace reader

} // namespace fwDataIO

#endif // __FWDATAIO_READER_DICTIONARYREADER_HPP__
