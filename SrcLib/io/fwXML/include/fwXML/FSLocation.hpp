/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FSLOCATION_HPP_
#define FSLOCATION_HPP_

#include <boost/filesystem/path.hpp>

#include "fwXML/config.hpp"
#include "fwXML/ILocation.hpp"

namespace fwXML
{

/**
 * @brief the purpose of this class is to provide a file system location scheme
 * A file is represented via the path this->rootFolder() / this->localFolder() / this->filename().
 * the rootFolder is used as the common factor of all file typically the root of a patient Folder
 **/
class FWXML_CLASS_API FSLocation : public ILocation
{
public:
    FWXML_API FSLocation();

    FWXML_API virtual ~FSLocation();

    /// extension "Mandatory must begin with a dot !!! or be empty
    FWXML_API  std::string & extension();

    /// retreive the filename ( no directory no extension )
    FWXML_API  boost::filesystem::path & filename();

    /// retreive the local Folder ( the relative path from rootFolder to directory containing filename )
    FWXML_API  boost::filesystem::path & localFolder();

    /// return the root directory of the patient Folder;
    FWXML_API  boost::filesystem::path & rootFolder();

    /// return get the complete/fullAbsolute  path i.e  this->rootFolder() / this->localFolder() / getFullFilename().
    FWXML_API  boost::filesystem::path  getFullPath() const ;

    /// get filename + extension
    FWXML_API boost::filesystem::path  getFullFilename() const;

    /// return get the local path path i.e  / this->localFolder() / this->filename().
    FWXML_API  boost::filesystem::path  getLocalPath() const ;

protected :

    /// represention extension exple ".inr.gz" or ".raw.gz" *must* begin with a dot
    std::string m_extension;

    /// the representation of the filename (no directory and no extension)
    boost::filesystem::path m_filename;

    /// the local path
    boost::filesystem::path m_localFolder;

    /// the root path
    boost::filesystem::path m_rootFolder;
};

}

#endif /*FSLOCATION_HPP_*/
