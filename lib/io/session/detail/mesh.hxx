/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <sight/io/session/config.hpp>

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/mesh.hpp>

#define SIGHT_PROFILING_DISABLED
#include <core/profiling.hpp>

//#define DEBUG_PLY
#ifdef DEBUG_PLY
#include <fstream>
#endif

namespace sight::io::session::detail::mesh
{

constexpr static auto UUID {"uuid"};

/**
 * @brief Get the file path for a given UUID
 */
inline static std::filesystem::path get_file_path(const std::string& _uuid)
{
    constexpr auto ext = ".ply";

    return _uuid + "/" + data::mesh::leaf_classname() + ext;
}

/**
 * @brief Write a value in binary little-endian format
 */
template<typename T>
inline static void write_binary(std::ostream& _os, const T& _value)
{
    _os.write(reinterpret_cast<const char*>(&_value), sizeof(T));
}

/**
 * @brief Read a value in binary little-endian format
 */
template<typename T>
inline static void read_binary(std::istream& _is, T& _value)
{
    _is.read(reinterpret_cast<char*>(&_value), sizeof(T));
}

/**
 * @brief Write a POD structure in binary little-endian format (optimized for block writes)
 */
template<typename T>
inline static void write_binary_block(std::ostream& _os, const T& _value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable for block write");
    _os.write(reinterpret_cast<const char*>(&_value), sizeof(T));
}

/**
 * @brief Read a POD structure in binary little-endian format (optimized for block reads)
 */
template<typename T>
inline static void read_binary_block(std::istream& _is, T& _value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable for block read");
    _is.read(reinterpret_cast<char*>(&_value), sizeof(T));
}

/**
 * @brief Safely extract a count value from the last space-delimited token in a line
 */
inline static std::size_t extract_count_from_line(const std::string& _line)
{
    const auto pos = _line.find_last_of(' ');
    if(pos == std::string::npos)
    {
        return 0;
    }

    char* end         = nullptr;
    std::size_t value = std::strtoul(_line.c_str() + pos + 1, &end, 10);

    if(*end != '\0' && *end != '\r')
    {
        return 0;
    }

    return value;
}

/**
 * @brief Write a PLY header
 */
inline static void write_ply_header(
    std::ostream& _os,
    std::size_t _num_points,
    std::size_t _num_cells,
    const data::mesh::csptr& _mesh
)
{
    const bool has_point_colors    = _mesh->has<data::mesh::attribute::point_colors>();
    const bool has_point_normals   = _mesh->has<data::mesh::attribute::point_normals>();
    const bool has_point_texcoords = _mesh->has<data::mesh::attribute::point_tex_coords>();
    const bool has_cell_colors     = _mesh->has<data::mesh::attribute::cell_colors>();
    const bool has_cell_normals    = _mesh->has<data::mesh::attribute::cell_normals>();
    const bool has_cell_texcoords  = _mesh->has<data::mesh::attribute::cell_tex_coords>();

    _os << "ply\n";
    _os << "format binary_little_endian 1.0\n";
    _os << "comment sight mesh v1\n";
    _os << "element vertex " << _num_points << "\n";
    _os << "property float x\n";
    _os << "property float y\n";
    _os << "property float z\n";

    if(has_point_normals)
    {
        _os << "property float nx\n";
        _os << "property float ny\n";
        _os << "property float nz\n";
    }

    if(has_point_colors)
    {
        _os << "property uchar red\n";
        _os << "property uchar green\n";
        _os << "property uchar blue\n";
        _os << "property uchar alpha\n";
    }

    if(has_point_texcoords)
    {
        _os << "property float texture_u\n";
        _os << "property float texture_v\n";
    }

    _os << "element face " << _num_cells << "\n";
    _os << "property list uchar int vertex_indices\n";

    if(has_cell_colors)
    {
        _os << "property uchar cell_red\n";
        _os << "property uchar cell_green\n";
        _os << "property uchar cell_blue\n";
        _os << "property uchar cell_alpha\n";
    }

    if(has_cell_normals)
    {
        _os << "property float cell_nx\n";
        _os << "property float cell_ny\n";
        _os << "property float cell_nz\n";
    }

    if(has_cell_texcoords)
    {
        _os << "property float cell_texture_u\n";
        _os << "property float cell_texture_v\n";
    }

    _os << "end_header\n";
}

//------------------------------------------------------------------------------

/**
 * @brief Write mesh data (vertices and cells) to a binary PLY stream
 */
inline static void write_mesh_data(std::ostream& _os, const data::mesh::csptr& _mesh)
{
    const std::size_t num_points = _mesh->num_points();
    const std::size_t num_cells  = _mesh->num_cells();

    // Write PLY header
    write_ply_header(_os, num_points, num_cells, _mesh);

    // Cache attribute availability
    const bool has_point_colors    = _mesh->template has<data::mesh::attribute::point_colors>();
    const bool has_point_normals   = _mesh->template has<data::mesh::attribute::point_normals>();
    const bool has_point_texcoords = _mesh->template has<data::mesh::attribute::point_tex_coords>();
    const bool has_cell_colors     = _mesh->template has<data::mesh::attribute::cell_colors>();
    const bool has_cell_normals    = _mesh->template has<data::mesh::attribute::cell_normals>();
    const bool has_cell_texcoords  = _mesh->template has<data::mesh::attribute::cell_tex_coords>();

    // Write vertices
    {
        // Handle all combinations of attributes
        if(has_point_normals && has_point_colors && has_point_texcoords)
        {
            for(const auto& [pos, normal, color, texcoord] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::nxyz,
                                  data::iterator::point::rgba,
                                  data::iterator::point::uv>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, normal);
                write_binary_block(_os, color);
                write_binary_block(_os, texcoord);
            }
        }
        else if(has_point_normals && has_point_colors)
        {
            for(const auto& [pos, normal, color] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::nxyz,
                                  data::iterator::point::rgba>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, normal);
                write_binary_block(_os, color);
            }
        }
        else if(has_point_normals && has_point_texcoords)
        {
            for(const auto& [pos, normal, texcoord] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::nxyz,
                                  data::iterator::point::uv>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, normal);
                write_binary_block(_os, texcoord);
            }
        }
        else if(has_point_colors && has_point_texcoords)
        {
            for(const auto& [pos, color, texcoord] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::rgba,
                                  data::iterator::point::uv>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, color);
                write_binary_block(_os, texcoord);
            }
        }
        else if(has_point_normals)
        {
            for(const auto& [pos, normal] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::nxyz>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, normal);
            }
        }
        else if(has_point_colors)
        {
            for(const auto& [pos, color] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::rgba>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, color);
            }
        }
        else if(has_point_texcoords)
        {
            for(const auto& [pos, texcoord] :
                _mesh->czip_range<data::iterator::point::xyz,
                                  data::iterator::point::uv>())
            {
                write_binary_block(_os, pos);
                write_binary_block(_os, texcoord);
            }
        }
        else
        {
            for(const auto& pos : _mesh->crange<data::iterator::point::xyz>())
            {
                write_binary_block(_os, pos);
            }
        }
    }

    // Write cells
    {
        auto write_cell =
            [&_os](const auto& _cell)
            {
                write_binary(_os, static_cast<std::uint8_t>(3));
                const std::array<int, 3> indices = {
                    static_cast<int>(_cell.pt[0]),
                    static_cast<int>(_cell.pt[1]),
                    static_cast<int>(_cell.pt[2])
                };
                _os.write(reinterpret_cast<const char*>(indices.data()), sizeof(indices));
            };

        // Handle all combinations of attributes
        if(has_cell_colors && has_cell_normals && has_cell_texcoords)
        {
            for(const auto& [cell, color, normal, texcoord] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::rgba,
                                  data::iterator::cell::nxyz,
                                  data::iterator::cell::uv>())
            {
                write_cell(cell);
                write_binary_block(_os, color);
                write_binary_block(_os, normal);
                write_binary_block(_os, texcoord);
            }
        }
        else if(has_cell_colors && has_cell_normals)
        {
            for(const auto& [cell, color, normal] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::rgba,
                                  data::iterator::cell::nxyz>())
            {
                write_cell(cell);
                write_binary_block(_os, color);
                write_binary_block(_os, normal);
            }
        }
        else if(has_cell_colors && has_cell_texcoords)
        {
            for(const auto& [cell, color, texcoord] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::rgba,
                                  data::iterator::cell::uv>())
            {
                write_cell(cell);
                write_binary_block(_os, color);
                write_binary_block(_os, texcoord);
            }
        }
        else if(has_cell_normals && has_cell_texcoords)
        {
            for(const auto& [cell, normal, texcoord] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::nxyz,
                                  data::iterator::cell::uv>())
            {
                write_cell(cell);
                write_binary_block(_os, normal);
                write_binary_block(_os, texcoord);
            }
        }
        else if(has_cell_colors)
        {
            for(const auto& [cell, color] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::rgba>())
            {
                write_cell(cell);
                write_binary_block(_os, color);
            }
        }
        else if(has_cell_normals)
        {
            for(const auto& [cell, normal] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::nxyz>())
            {
                write_cell(cell);
                write_binary_block(_os, normal);
            }
        }
        else if(has_cell_texcoords)
        {
            for(const auto& [cell, texcoord] :
                _mesh->czip_range<data::iterator::cell::triangle,
                                  data::iterator::cell::uv>())
            {
                write_cell(cell);
                write_binary_block(_os, texcoord);
            }
        }
        else
        {
            for(const auto& cell : _mesh->crange<data::iterator::cell::triangle>())
            {
                write_cell(cell);
            }
        }
    }
}

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& _password = ""
)
{
    SIGHT_PROFILE("write mesh (PLY)");

    // Get the object
    const auto mesh = helper::safe_cast<data::mesh>(_object);

    // Add a version number
    helper::write_version<data::mesh>(_tree, 2);

    // Get the mesh Lock
    const auto dump_lock = mesh->dump_lock();

    // Open output stream in archive (PLY format, compressed with ZSTD)
    const auto& ostream = _archive.open_file(
        get_file_path(mesh->get_uuid()),
        _password,
        sight::io::zip::method::zstd,
        sight::io::zip::level::fast
    );

    // Write mesh data to stream
    write_mesh_data(*ostream, mesh);

#ifdef DEBUG_PLY
    // Write the mesh data to a file
    {
        std::ofstream os(R"(D:\Dev\tmp\)" + mesh->get_uuid() + ".ply", std::ios::binary);
        write_mesh_data(os, mesh);
    }
#endif
}

//------------------------------------------------------------------------------

inline static data::mesh::sptr read(
    zip::archive_reader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    SIGHT_PROFILE("read mesh (PLY)");

    // Create or reuse the object
    auto mesh = helper::cast_or_create<data::mesh>(_object);

    // Check version number
    const auto version = helper::read_version<data::mesh>(_tree, 0, 2);

    // Get UUID and open archive file
    const auto& uuid = _tree.get<std::string>(UUID);

    if(version < 2)
    {
        SIGHT_THROW(
            "Cannot read mesh with version " << version
            << " because VTK support is disabled. Please recompile the serializer with VTK support enabled."
        );
    }
    else
    {
        const auto& istream = _archive.open_file(
            get_file_path(uuid),
            _password
        );

        // Parse PLY header
        std::string line;
        std::size_t num_points = 0;
        std::size_t num_cells  = 0;

        bool has_point_normals   = false;
        bool has_point_colors    = false;
        bool has_point_texcoords = false;
        bool has_cell_colors     = false;
        bool has_cell_normals    = false;
        bool has_cell_texcoords  = false;

        bool in_vertex_element = false;
        bool in_face_element   = false;

        // Parse header until "end_header"
        while(std::getline(*istream, line))
        {
            if(line.find("end_header") != std::string::npos)
            {
                break;
            }

            // Parse "element vertex N"
            if(line.find("element vertex") != std::string::npos)
            {
                in_vertex_element = true;
                in_face_element   = false;
                num_points        = extract_count_from_line(line);
            }
            // Parse "element face N"
            else if(line.find("element face") != std::string::npos)
            {
                in_vertex_element = false;
                in_face_element   = true;
                num_cells         = extract_count_from_line(line);
            }
            else if(in_vertex_element)
            {
                if(line.find("property float nx") != std::string::npos)
                {
                    has_point_normals = true;
                }
                else if(line.find("property uchar red") != std::string::npos)
                {
                    has_point_colors = true;
                }
                else if(line.find("property float texture_u") != std::string::npos)
                {
                    has_point_texcoords = true;
                }
            }
            else if(in_face_element)
            {
                if(line.find("property uchar cell_red") != std::string::npos)
                {
                    has_cell_colors = true;
                }
                else if(line.find("property float cell_nx") != std::string::npos)
                {
                    has_cell_normals = true;
                }
                else if(line.find("property float cell_texture_u") != std::string::npos)
                {
                    has_cell_texcoords = true;
                }
            }
        }

        if(num_cells == 0 || num_points == 0)
        {
            return mesh;
        }

        // Prepare mesh with appropriate attributes
        data::mesh::attribute attributes = data::mesh::attribute::none;
        if(has_point_normals)
        {
            attributes = attributes | data::mesh::attribute::point_normals;
        }

        if(has_point_colors)
        {
            attributes = attributes | data::mesh::attribute::point_colors;
        }

        if(has_point_texcoords)
        {
            attributes = attributes | data::mesh::attribute::point_tex_coords;
        }

        if(has_cell_colors)
        {
            attributes = attributes | data::mesh::attribute::cell_colors;
        }

        if(has_cell_normals)
        {
            attributes = attributes | data::mesh::attribute::cell_normals;
        }

        if(has_cell_texcoords)
        {
            attributes = attributes | data::mesh::attribute::cell_tex_coords;
        }

        mesh->clear();
        mesh->resize(
            static_cast<data::mesh::size_t>(num_points),
            static_cast<data::mesh::size_t>(num_cells),
            data::mesh::cell_type_t::triangle,
            attributes
        );

        const auto dump_lock = mesh->dump_lock();

        // Read vertices using zip_range for better performance
        {
            // Handle all combinations of attributes using zip_range
            if(has_point_normals && has_point_colors && has_point_texcoords)
            {
                for(auto&& [pos, normal, color, texcoord] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::nxyz,
                                    data::iterator::point::rgba,
                                    data::iterator::point::uv>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, normal);
                    read_binary_block(*istream, color);
                    read_binary_block(*istream, texcoord);
                }
            }
            else if(has_point_normals && has_point_colors)
            {
                for(auto&& [pos, normal, color] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::nxyz,
                                    data::iterator::point::rgba>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, normal);
                    read_binary_block(*istream, color);
                }
            }
            else if(has_point_normals && has_point_texcoords)
            {
                for(auto&& [pos, normal, texcoord] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::nxyz,
                                    data::iterator::point::uv>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, normal);
                    read_binary_block(*istream, texcoord);
                }
            }
            else if(has_point_colors && has_point_texcoords)
            {
                for(auto&& [pos, color, texcoord] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::rgba,
                                    data::iterator::point::uv>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, color);
                    read_binary_block(*istream, texcoord);
                }
            }
            else if(has_point_normals)
            {
                for(auto&& [pos, normal] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::nxyz>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, normal);
                }
            }
            else if(has_point_colors)
            {
                for(auto&& [pos, color] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::rgba>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, color);
                }
            }
            else if(has_point_texcoords)
            {
                for(auto&& [pos, texcoord] :
                    mesh->zip_range<data::iterator::point::xyz,
                                    data::iterator::point::uv>())
                {
                    read_binary_block(*istream, pos);
                    read_binary_block(*istream, texcoord);
                }
            }
            else
            {
                for(auto& pos : mesh->range<data::iterator::point::xyz>())
                {
                    read_binary_block(*istream, pos);
                }
            }
        }

        // Read cells using zip_range for better performance
        {
            auto read_cell_indices =
                [&istream](auto& _cell)
                {
                    std::uint8_t vertex_count = 0;
                    read_binary(*istream, vertex_count);

                    for(std::uint8_t j = 0 ; j < vertex_count ; ++j)
                    {
                        sight::data::mesh::point_t index = 0;
                        read_binary(*istream, index);
                        if(j < 3)
                        {
                            _cell.pt[j] = index;
                        }
                    }
                };

            // Handle all combinations of attributes using zip_range
            if(has_cell_colors && has_cell_normals && has_cell_texcoords)
            {
                for(auto&& [cell, color, normal, texcoord] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::rgba,
                                    data::iterator::cell::nxyz,
                                    data::iterator::cell::uv>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, color);
                    read_binary_block(*istream, normal);
                    read_binary_block(*istream, texcoord);
                }
            }
            else if(has_cell_colors && has_cell_normals)
            {
                for(auto&& [cell, color, normal] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::rgba,
                                    data::iterator::cell::nxyz>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, color);
                    read_binary_block(*istream, normal);
                }
            }
            else if(has_cell_colors && has_cell_texcoords)
            {
                for(auto&& [cell, color, texcoord] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::rgba,
                                    data::iterator::cell::uv>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, color);
                    read_binary_block(*istream, texcoord);
                }
            }
            else if(has_cell_normals && has_cell_texcoords)
            {
                for(auto&& [cell, normal, texcoord] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::nxyz,
                                    data::iterator::cell::uv>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, normal);
                    read_binary_block(*istream, texcoord);
                }
            }
            else if(has_cell_colors)
            {
                for(auto&& [cell, color] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::rgba>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, color);
                }
            }
            else if(has_cell_normals)
            {
                for(auto&& [cell, normal] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::nxyz>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, normal);
                }
            }
            else if(has_cell_texcoords)
            {
                for(auto&& [cell, texcoord] :
                    mesh->zip_range<data::iterator::cell::triangle,
                                    data::iterator::cell::uv>())
                {
                    read_cell_indices(cell);
                    read_binary_block(*istream, texcoord);
                }
            }
            else
            {
                for(auto& cell : mesh->range<data::iterator::cell::triangle>())
                {
                    read_cell_indices(cell);
                }
            }
        }
    }

    return mesh;
}

SIGHT_REGISTER_SERIALIZER(data::mesh, write, read);

} // namespace sight::io::session::detail::mesh
