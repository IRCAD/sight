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
 * The root path of all temporary directories is accessible through the static method shared_directory().
 * This directory will only be deleted at application exit.
 */
class CORE_CLASS_API temp_path
{
public:

    /// Disable copy
    temp_path(const temp_path&)            = delete;
    temp_path(temp_path&&)                 = delete;
    temp_path& operator=(const temp_path&) = delete;
    temp_path& operator=(temp_path&&)      = delete;

    /// Conversion operators
    /// @{
    inline operator std::filesystem::path() const noexcept;
    inline operator std::string() const noexcept;
    /// @}

    /// Convenience operators
    /// @{
    inline std::filesystem::path operator/(const std::filesystem::path& _other) const noexcept;
    inline std::string operator+(const std::string& _other) const noexcept;
    inline bool operator==(const temp_path& _other) const noexcept;
    inline std::string string() const noexcept;
    /// @}

    /// Returns the temporary path
    /// The path is automatically deleted when the instance is destroyed.
    inline const std::filesystem::path& path() const noexcept;

    /// Returns the root path of all temporary directories.
    /// This directory will only be deleted at application exit.
    /// @param subdirectory_prefix if set, creates a subdirectory with the given prefix
    CORE_API static std::filesystem::path shared_directory(const std::string& _subdirectory_prefix = std::string());

protected:

    /// Constructor / Destructor
    /// @{
    CORE_API temp_path(const std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> >& _path_and_stream);
    CORE_API virtual ~temp_path() noexcept;
    /// @}

    /// Holds the ofstream if any. Will be closed at destruction.
    std::shared_ptr<std::ofstream> m_ofstream;

private:

    /// Holds the path to remove at destruction
    const std::filesystem::path M_PATH;
};

inline temp_path::operator std::filesystem::path() const noexcept
{
    return M_PATH;
}

inline temp_path::operator std::string() const noexcept
{
    return M_PATH.string();
}

//------------------------------------------------------------------------------

inline std::filesystem::path temp_path::operator/(const std::filesystem::path& _other) const noexcept
{
    return M_PATH / _other;
}

//------------------------------------------------------------------------------

inline std::string temp_path::operator+(const std::string& _other) const noexcept
{
    return M_PATH.string() + _other;
}

//------------------------------------------------------------------------------

inline bool temp_path::operator==(const temp_path& _other) const noexcept
{
    return M_PATH == _other.M_PATH;
}

//------------------------------------------------------------------------------

inline std::string temp_path::string() const noexcept
{
    return M_PATH.string();
}

//------------------------------------------------------------------------------

inline const std::filesystem::path& temp_path::path() const noexcept
{
    return M_PATH;
}

/**
 * @brief This class represents a temporary directory.
 *
 * The temporary directory is automatically deleted when the instance is destroyed.
 *
 */
class CORE_CLASS_API temp_dir final : public temp_path
{
public:

    /// Constructor
    /// @param[in] path Allows to specify a path, that will be deleted when the instance is destroyed.
    ///                 It is up to the user to ensure that the path is unique and not used elsewhere.
    CORE_API temp_dir(const std::optional<std::filesystem::path>& _path = std::nullopt);

    /// Destructor
    CORE_API ~temp_dir() noexcept override = default;

    /// Convenience operator
    inline bool operator==(const temp_dir& _other) const noexcept;
};

//------------------------------------------------------------------------------

inline bool temp_dir::operator==(const temp_dir& _other) const noexcept
{
    return temp_path::operator==(_other);
}

/**
 * @brief This class represents a temporary file.
 *
 * The temporary file is automatically deleted when the instance is destroyed.
 *
 */
class CORE_CLASS_API temp_file final : public temp_path
{
public:

    /// Constructor
    /// @param[in] openmode If provided, an ofstream will be opened with the given parameters.
    ///                     This is the safest way to avoid race conditions and possible security problems.
    /// @param[in] path Allows to specify a path, that will be deleted when the instance is destroyed.
    ///                 It is up to the user to ensure that the path is unique and not used elsewhere.
    CORE_API temp_file(
        const std::optional<std::ios_base::openmode>& _openmode = std::nullopt,
        const std::optional<std::filesystem::path>& _path       = std::nullopt
    );

    /// Destructor
    CORE_API ~temp_file() noexcept override = default;

    /// Conversion operator
    inline operator std::ostream& ();

    /// Convenience operators
    /// @{
    /// Equality operator
    inline bool operator==(const temp_file& _other) const noexcept;

    /// Stream operator
    template<typename T>
    inline friend std::ostream& operator<<(temp_file& _os, const T& _t);
    /// @}

    /// Returns an unique temporary file path. The file will be deleted when application exit.
    CORE_API static std::filesystem::path unique_path();

    /// Returns an unique temporary file path and an ofstream. The stream will be closed and the file deleted when
    /// application exit.
    /// @param openmode Open mode of the stream.
    CORE_API static std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> > unique_stream(
        const std::optional<std::ios_base::openmode>& _openmode = std::ios_base::out | std::ios_base::trunc
    );

    /// Returns the ofstream if any. Will be closed when instance is destructed.
    inline std::ofstream& stream();
};

inline temp_file::operator std::ostream& ()
{
    return *m_ofstream;
}

//------------------------------------------------------------------------------

inline bool temp_file::operator==(const temp_file& _other) const noexcept
{
    return temp_path::operator==(_other);
}

//------------------------------------------------------------------------------

template<typename T>
inline std::ostream& operator<<(temp_file& _os, const T& _t)
{
    return *_os.m_ofstream << _t;
}

//------------------------------------------------------------------------------

inline std::ofstream& temp_file::stream()
{
    return *m_ofstream;
}

} // namespace sight::core::os
