/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "igtlProtocol/config.hpp"

#include <fwCore/macros.hpp>

#include <fwZip/exception/Read.hpp>
#include <fwZip/IReadArchive.hpp>

#include <archive.h>

#include <boost/iostreams/categories.hpp>

#include <map>

namespace igtlProtocol
{
namespace archiver
{

typedef SPTR (std::vector<char>)  BufferSPtr;
typedef CSPTR (std::vector<char>) BufferCSPtr;

/**
 *
 * @brief MemoryReadArchive is a memory archive reader
 */
class IGTLPROTOCOL_CLASS_API MemoryReadArchive : public ::fwZip::IReadArchive
{
public:

    typedef SPTR (MemoryReadArchive) sptr;

    /// constructor
    IGTLPROTOCOL_API MemoryReadArchive(const char* buffer, const std::size_t size);

    /// destructor
    IGTLPROTOCOL_API ~MemoryReadArchive();

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param[in] path file in archive.
     * @return input stream from request file.
     *
     * @throw ::fwZip::exception::Read if file doesn't exist in archive.
     * @throw ::fwZip::exception::Read if cannot retrieve file in archive.
     */
    IGTLPROTOCOL_API SPTR(std::istream) getFile(const ::boost::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     *
     * @return the archive path
     */
    IGTLPROTOCOL_API const ::boost::filesystem::path getArchivePath() const override;

    /**
     * @brief clone the instance
     *
     * @return cloned instance
     */
    IReadArchive::sptr clone() const override
    {
        return SPTR(MemoryReadArchive) (new MemoryReadArchive(m_BUFFER, m_SIZE));
    }

private:

    /**
     * @brief read file data from buffer
     *
     * @param[in] buffer buffer data to fill
     */
    void readEntry(BufferSPtr buffer);

private:

    /// size of archive
    const std::size_t m_SIZE;

    /// buffer contain the archive data
    const char* m_BUFFER;

    /// archive instance
    struct archive* m_archive;

    /// stream created when unarchive is done
    std::map< std::string, SPTR(std::istream)> m_streams;

    /// buffer read size in memory(huge because we have to read in memory, big buffer should be efficient)
    static const int s_BUFFER_READ_SIZE = 20000;
};

} // namespace archiver
} // namespace igtlProtocol
