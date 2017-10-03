/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_READZIPARCHIVE_HPP__
#define __FWZIP_READZIPARCHIVE_HPP__

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <istream>

namespace fwZip
{

/**
 * @brief   This class defines functions to read a file in a zip archive.
 */
class FWZIP_CLASS_API ReadZipArchive : public IReadArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ReadZipArchive)(IReadArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           std::make_shared< ReadZipArchive >);

    /*
     * @brief Constructors. Initializes archive path, zip descriptor and zip stream.
     *
     * @throw ::fwZip::exception::Read if archive doesn't exist.
     * @throw ::fwZip::exception::Read if archive cannot be opened.
     */
    FWZIP_API ReadZipArchive( const ::boost::filesystem::path& archive );

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw ::fwZip::exception::Read if file doesn't exist in archive.
     * @throw ::fwZip::exception::Read if cannot retrieve file in archive.
     */
    FWZIP_API SPTR(std::istream) getFile(const ::boost::filesystem::path &path) override;

    /**
     * @brief Returns comment from the current archive (zip).
     * @param path file in archive.
     * @return comment
     */
    FWZIP_API std::string getComment();

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const override;

    //------------------------------------------------------------------------------

    IReadArchive::sptr clone() const override
    {
        return ReadZipArchive::New(m_archive);
    }

protected:

    ::boost::filesystem::path m_archive;
};

}

#endif /* __FWZIP_READZIPARCHIVE_HPP__ */

