/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "core/config.hpp"

#include <filesystem>
#include <fstream>
#include <optional>

namespace sight::core::os
{

/**
 * @brief This class is the base class for temporary directory or file.
 *
 * The temporary directory or file is automatically deleted when the instance is destroyed.
 * The root path of all temporary directories is accessible through the static method sharedDirectory().
 * This directory will only be deleted at application exit.
 */
class CORE_CLASS_API TempPath
{
public:

    /// Disable copy
    TempPath(const TempPath&)            = delete;
    TempPath(TempPath&&)                 = delete;
    TempPath& operator=(const TempPath&) = delete;
    TempPath& operator=(TempPath&&)      = delete;

    /// Conversion operators
    /// @{
    inline operator std::filesystem::path() const noexcept;
    inline operator std::string() const noexcept;
    /// @}

    /// Convenience operators
    /// @{
    inline std::filesystem::path operator/(const std::filesystem::path& other) const noexcept;
    inline std::string operator+(const std::string& other) const noexcept;
    inline bool operator==(const TempPath& other) const noexcept;
    inline std::string string() const noexcept;
    /// @}

    /// Returns the temporary path
    /// The path is automatically deleted when the instance is destroyed.
    inline const std::filesystem::path& path() const noexcept;

    /// Returns the root path of all temporary directories.
    /// This directory will only be deleted at application exit.
    /// @param subdirectory_prefix if set, creates a subdirectory with the given prefix
    CORE_API static std::filesystem::path sharedDirectory(const std::string& subdirectory_prefix = std::string());

protected:

    /// Constructor / Destructor
    /// @{
    CORE_API TempPath(const std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> >& path_and_stream);
    CORE_API virtual ~TempPath() noexcept;
    /// @}

    /// Holds the ofstream if any. Will be closed at destruction.
    std::shared_ptr<std::ofstream> m_ofstream;

private:

    /// Holds the path to remove at destruction
    const std::filesystem::path m_path;
};

inline TempPath::operator std::filesystem::path() const noexcept
{
    return m_path;
}

inline TempPath::operator std::string() const noexcept
{
    return m_path.string();
}

//------------------------------------------------------------------------------

inline std::filesystem::path TempPath::operator/(const std::filesystem::path& other) const noexcept
{
    return m_path / other;
}

//------------------------------------------------------------------------------

inline std::string TempPath::operator+(const std::string& other) const noexcept
{
    return m_path.string() + other;
}

//------------------------------------------------------------------------------

inline bool TempPath::operator==(const TempPath& other) const noexcept
{
    return m_path == other.m_path;
}

//------------------------------------------------------------------------------

inline std::string TempPath::string() const noexcept
{
    return m_path.string();
}

//------------------------------------------------------------------------------

inline const std::filesystem::path& TempPath::path() const noexcept
{
    return m_path;
}

/**
 * @brief This class represents a temporary directory.
 *
 * The temporary directory is automatically deleted when the instance is destroyed.
 *
 */
class CORE_CLASS_API TempDir final : public TempPath
{
public:

    /// Constructor
    /// @param[in] path Allows to specify a path, that will be deleted when the instance is destroyed.
    ///                 It is up to the user to ensure that the path is unique and not used elsewhere.
    CORE_API TempDir(const std::optional<std::filesystem::path>& path = std::nullopt);

    /// Destructor
    CORE_API ~TempDir() noexcept override = default;

    /// Convenience operator
    inline bool operator==(const TempDir& other) const noexcept;
};

//------------------------------------------------------------------------------

inline bool TempDir::operator==(const TempDir& other) const noexcept
{
    return TempPath::operator==(other);
}

/**
 * @brief This class represents a temporary file.
 *
 * The temporary file is automatically deleted when the instance is destroyed.
 *
 */
class CORE_CLASS_API TempFile final : public TempPath
{
public:

    /// Constructor
    /// @param[in] openmode If provided, an ofstream will be opened with the given parameters.
    ///                     This is the safest way to avoid race conditions and possible security problems.
    /// @param[in] path Allows to specify a path, that will be deleted when the instance is destroyed.
    ///                 It is up to the user to ensure that the path is unique and not used elsewhere.
    CORE_API TempFile(
        const std::optional<std::ios_base::openmode>& openmode = std::nullopt,
        const std::optional<std::filesystem::path>& path       = std::nullopt
    );

    /// Destructor
    CORE_API ~TempFile() noexcept override = default;

    /// Conversion operator
    inline operator std::ostream& ();

    /// Convenience operators
    /// @{
    /// Equality operator
    inline bool operator==(const TempFile& other) const noexcept;

    /// Stream operator
    template<typename T>
    inline friend std::ostream& operator<<(TempFile& os, const T& t);
    /// @}

    /// Returns an unique temporary file path. The file will be deleted when application exit.
    CORE_API static std::filesystem::path uniquePath();

    /// Returns an unique temporary file path and an ofstream. The stream will be closed and the file deleted when
    /// application exit.
    /// @param openmode Open mode of the stream.
    CORE_API static std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> > uniqueStream(
        const std::optional<std::ios_base::openmode>& openmode = std::ios_base::out | std::ios_base::trunc
    );

    /// Returns the ofstream if any. Will be closed when instance is destructed.
    inline std::ofstream& stream();
};

inline TempFile::operator std::ostream& ()
{
    return *m_ofstream;
}

//------------------------------------------------------------------------------

inline bool TempFile::operator==(const TempFile& other) const noexcept
{
    return TempPath::operator==(other);
}

//------------------------------------------------------------------------------

template<typename T>
inline std::ostream& operator<<(TempFile& os, const T& t)
{
    return *os.m_ofstream << t;
}

//------------------------------------------------------------------------------

inline std::ofstream& TempFile::stream()
{
    return *m_ofstream;
}

} // namespace sight::core::os
