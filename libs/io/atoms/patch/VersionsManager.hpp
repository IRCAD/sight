/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/atoms/config.hpp"
#include "io/atoms/patch/VersionsGraph.hpp"

#include <core/mt/types.hpp>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace sight::io::atoms::patch
{

/**
 * @brief Maintains a table of object versions and a table of links between objects.
 *
 * Version and link tables are built from .versions and .graphlink files.
 */
class IO_ATOMS_CLASS_API VersionsManager
{
public:

    /**
     * @name Typedefs
     * @{ */
    typedef std::vector<std::filesystem::path> ListPathType;
    typedef std::map<std::string, VersionsGraph::sptr> VersionsGraphMapType;
    /**  @} */

    IO_ATOMS_API VersionsManager();
    IO_ATOMS_API ~VersionsManager();

    /**
     * @brief Returns graph corresponding to given context.
     * @param context context of the requested VersionGraph.
     * @note This method is thread safe.
     */
    IO_ATOMS_API VersionsGraph::sptr getGraph(const std::string& context);

    /**
     * @brief Generates a .versions file with currently known data versions.
     * @param filePath path of .versions file to write.
     * @param context context of the VersionGraph.
     * @param versionName name of the version to write.
     * @todo Ignoring file in ignored library.
     */
    IO_ATOMS_API static void generateNewFile(
        const std::filesystem::path& filePath,
        const std::string& context,
        const std::string& versionName
    );

    /**
     * @brief Builds version table.
     * @param dirPath directory to search for .versions files
     * @note This method is thread safe.
     */
    IO_ATOMS_API void buildVersionTable(const std::string& dirPath);

    /**
     * @brief Builds link table.
     * @param dirPath directory to search for .graphlink files
     * @note This method is thread safe.
     */
    IO_ATOMS_API void buildLinkTable(const std::string& dirPath);

    /// Returns the default instance of VersionsManager
    static SPTR(VersionsManager) getDefault()
    {
        return s_default;
    }

private:

    /**
     * @brief Loads a file containing object versions.
     * @param filePath path to a .versions file
     */
    IO_ATOMS_API static io::atoms::patch::VersionDescriptor getVersion(
        const std::filesystem::path& filePath
    );

    /**
     * @brief Loads a file containing links between objects versions.
     * @param filePath path to a .graphlink file
     */
    IO_ATOMS_API static io::atoms::patch::LinkDescriptor getLink(
        const std::filesystem::path& filePath
    );

    /**
     * @brief Generates the versions graph.
     * @note This method is NOT thread safe.
     */
    IO_ATOMS_API void generateVersionsGraph();

    /// Mutex to protect m_versionTable access;
    core::mt::ReadWriteMutex m_versionMutex;

    /// Mutex to protect m_linkTable access;
    core::mt::ReadWriteMutex m_linkMutex;

    /// Mutex to protect m_versionsGraphMap access;
    core::mt::ReadWriteMutex m_graphMutex;

    /// Version table
    ListPathType m_versionTable;

    /// Link table
    ListPathType m_linkTable;

    /// Versions graphs
    VersionsGraphMapType m_versionsGraphMap;

    /// Default instance of Versions Manager
    IO_ATOMS_API static SPTR(VersionsManager) s_default;
};

} // fwAtomsPatch
