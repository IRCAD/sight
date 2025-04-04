/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

//#define DEBUG_NIFTI

#include <sight/io/session/config.hpp>

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

//#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <data/image.hpp>

#include <io/vtk/vtk.hpp>

#include <nifti1.h>
#include <nifti1_io.h>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLImageDataWriter.h>

// cspell:ignore qoffset XFORM qform XYZT nvox srow sform

namespace sight::io::session::detail::image
{

constexpr static auto UUID {"uuid"};
constexpr static auto WINDOW_CENTERS {"WindowCenters"};
constexpr static auto WINDOW_WIDTHS {"WindowWidths"};
constexpr static auto WINDOW_CENTER {"WindowCenter"};
constexpr static auto WINDOW_WIDTH {"WindowWidth"};
constexpr static auto TYPE {"Type"};
constexpr static auto PIXEL_FORMAT {"PixelFormat"};
constexpr static auto SPACING {"Spacing"};
constexpr static auto ORIGIN {"Origin"};
constexpr static auto ORIENTATION {"Orientation"};
constexpr static auto SIZE {"Size"};
constexpr static auto X {"X"};
constexpr static auto Y {"Y"};
constexpr static auto Z {"Z"};
constexpr static auto X_X {"X_X"};
constexpr static auto X_Y {"X_Y"};
constexpr static auto X_Z {"X_Z"};
constexpr static auto Y_X {"Y_X"};
constexpr static auto Y_Y {"Y_Y"};
constexpr static auto Y_Z {"Y_Z"};
constexpr static auto Z_X {"Z_X"};
constexpr static auto Z_Y {"Z_Y"};
constexpr static auto Z_Z {"Z_Z"};
constexpr static auto WIDTH {"Width"};
constexpr static auto HEIGHT {"Height"};
constexpr static auto DEPTH {"Depth"};

#ifdef DEBUG_NIFTI
inline static struct nifti_debug
{
    nifti_debug()
    {
        nifti_set_debug_level(3);
    }
} nifti_debug;
#endif

//------------------------------------------------------------------------------

inline static std::filesystem::path get_file_path(const std::string& _uuid)
{
    constexpr auto ext = ".nii";

    return std::filesystem::path(_uuid + "/" + data::image::leaf_classname() + ext);
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
    auto image = helper::safe_cast<data::image>(_object);

    // Add a version number. Not mandatory, but could help for future release

    helper::write_version<data::image>(_tree, 2);

    // Serialize image
    const auto& size = image->size();
    boost::property_tree::ptree size_tree;
    size_tree.add(WIDTH, size[0]);
    size_tree.add(HEIGHT, size[1]);
    size_tree.add(DEPTH, size[2]);
    _tree.add_child(SIZE, size_tree);

    const auto& type = image->type();
    _tree.put(TYPE, type.name());

    const auto& format = image->pixel_format();
    _tree.put(PIXEL_FORMAT, format);

    // Write image metadata
    const auto& spacing = image->spacing();
    boost::property_tree::ptree spacing_tree;
    spacing_tree.add(X, spacing[0]);
    spacing_tree.add(Y, spacing[1]);
    spacing_tree.add(Z, spacing[2]);
    _tree.add_child(SPACING, spacing_tree);

    const auto& origin = image->origin();
    boost::property_tree::ptree origin_tree;
    origin_tree.add(X, origin[0]);
    origin_tree.add(Y, origin[1]);
    origin_tree.add(Z, origin[2]);
    _tree.add_child(ORIGIN, origin_tree);

    const auto& orientation = image->orientation();
    boost::property_tree::ptree orientation_tree;
    orientation_tree.add(X_X, orientation[0]);
    orientation_tree.add(Y_X, orientation[1]);
    orientation_tree.add(Z_X, orientation[2]);
    orientation_tree.add(X_Y, orientation[3]);
    orientation_tree.add(Y_Y, orientation[4]);
    orientation_tree.add(Z_Y, orientation[5]);
    orientation_tree.add(X_Z, orientation[6]);
    orientation_tree.add(Y_Z, orientation[7]);
    orientation_tree.add(Z_Z, orientation[8]);
    _tree.add_child(ORIENTATION, orientation_tree);

    boost::property_tree::ptree window_centers_tree;
    for(std::size_t index = 0 ; const auto& window_center : image->window_center())
    {
        window_centers_tree.put(WINDOW_CENTER + std::to_string(index++), window_center);
    }

    _tree.add_child(WINDOW_CENTERS, window_centers_tree);

    boost::property_tree::ptree window_widths_tree;
    for(std::size_t index = 0 ; const auto& window_width : image->window_width())
    {
        window_widths_tree.put(WINDOW_WIDTH + std::to_string(index++), window_width);
    }

    _tree.add_child(WINDOW_WIDTHS, window_widths_tree);

    {
        FW_PROFILE("write NIFTI");

        if(image->size_in_bytes() == 0)
        {
            // If the image is empty, we don't need to write anything
            return;
        }

        SIGHT_THROW_IF(
            "Image size is too large for NIFTI format. NIFTI only supports images with a maximum size of 32767",
            size[0] > std::numeric_limits<std::int16_t>::max()
            || size[1] > std::numeric_limits<std::int16_t>::max()
            || size[2] > std::numeric_limits<std::int16_t>::max()
        );

        // Dimensions. We always have 3D images, with one or more components.
        const int  dim[] = {
            // Number of "dimensions": 5 because of 3 for 3D images + 1 for time + 1 for components,
            5,
            // Width
            int(size[0]),
            // Height
            int(size[1]),
            // Depth
            int(size[2]),
            // Time
            1,
            // Number of components
            int(image->num_components()),
            // UNUSED
            1,
            1
        };

        // Type
        const int data_type =
            [&type]
            {
                switch(type)
                {
                    case sight::core::type::UINT8:
                        return DT_UINT8;

                    case sight::core::type::UINT16:
                        return DT_UINT16;

                    case sight::core::type::UINT32:
                        return DT_UINT32;

                    case sight::core::type::UINT64:
                        return DT_UINT64;

                    case sight::core::type::INT8:
                        return DT_INT8;

                    case sight::core::type::INT16:
                        return DT_INT16;

                    case sight::core::type::INT32:
                        return DT_INT32;

                    case sight::core::type::INT64:
                        return DT_INT64;

                    case sight::core::type::FLOAT:
                        return DT_FLOAT;

                    case sight::core::type::DOUBLE:
                        return DT_DOUBLE;

                    default:
                        return DT_UINT8;
                }
            }();

        auto* const nifti_header = nifti_make_new_header(dim, data_type);

        // We are in mm and second
        nifti_header->xyzt_units = SPACE_TIME_TO_XYZT(NIFTI_UNITS_MM, NIFTI_UNITS_SEC);

        // Intent, vector seems to be the best choice for images with multiple components
        nifti_header->intent_code = NIFTI_INTENT_VECTOR;

        // Spacing
        nifti_header->pixdim[1] = float(spacing[0]);
        nifti_header->pixdim[2] = float(spacing[1]);
        nifti_header->pixdim[3] = float(spacing[2]);

        // Origin + Orientation
        /// @note Look at the nifti1.h file for more information about the axes inversion
        auto qto_xyz = nifti_make_orthog_mat44(
            float(-orientation[0]),
            float(-orientation[1]),
            float(orientation[2]),
            float(-orientation[3]),
            float(-orientation[4]),
            float(orientation[5]),
            float(-orientation[6]),
            float(-orientation[7]),
            float(orientation[8])
        );

        // ITK transpose the matrix, do the same here
        qto_xyz =
            [&qto_xyz]
            {
                mat44 out;

                for(unsigned int i = 0 ; i < 4 ; ++i)
                {
                    for(unsigned int j = 0 ; j < 4 ; ++j)
                    {
                        out.m[i][j] = qto_xyz.m[j][i];
                    }
                }

                return out;
            }();

        // Set the origin
        qto_xyz.m[0][3] = float(-origin[0]);
        qto_xyz.m[1][3] = float(-origin[1]);
        qto_xyz.m[2][3] = float(origin[2]);

        nifti_mat44_to_quatern(
            qto_xyz,
            &nifti_header->quatern_b,
            &nifti_header->quatern_c,
            &nifti_header->quatern_d,
            &nifti_header->qoffset_x,
            &nifti_header->qoffset_y,
            &nifti_header->qoffset_z,
            nullptr,
            nullptr,
            nullptr,
            &nifti_header->pixdim[0]
        );

        nifti_header->srow_x[0] = qto_xyz.m[0][0] * nifti_header->pixdim[1];
        nifti_header->srow_x[1] = qto_xyz.m[0][1] * nifti_header->pixdim[1];
        nifti_header->srow_x[2] = qto_xyz.m[0][2] * nifti_header->pixdim[1];
        nifti_header->srow_x[3] = qto_xyz.m[0][3];

        nifti_header->srow_y[0] = qto_xyz.m[1][0] * nifti_header->pixdim[2];
        nifti_header->srow_y[1] = qto_xyz.m[1][1] * nifti_header->pixdim[2];
        nifti_header->srow_y[2] = qto_xyz.m[1][2] * nifti_header->pixdim[2];
        nifti_header->srow_y[3] = qto_xyz.m[1][3];

        nifti_header->srow_z[0] = qto_xyz.m[2][0] * nifti_header->pixdim[3];
        nifti_header->srow_z[1] = qto_xyz.m[2][1] * nifti_header->pixdim[3];
        nifti_header->srow_z[2] = qto_xyz.m[2][2] * nifti_header->pixdim[3];
        nifti_header->srow_y[3] = qto_xyz.m[2][3];

        // NIFTI_XFORM_SCANNER_ANAT is suited when coming from DICOM convention
        nifti_header->qform_code = NIFTI_XFORM_SCANNER_ANAT;
        nifti_header->sform_code = NIFTI_XFORM_SCANNER_ANAT;

        // Create the output file inside the archive
        const auto& ostream = _archive.open_file(
            get_file_path(image->get_uuid()),
            _password,
            sight::io::zip::method::DEFAULT,
            sight::io::zip::level::best
        );

        // From nifti1_io.c
        int offset = sizeof(nifti_1_header) + 4;
        if((offset % 16) != 0)
        {
            offset = ((offset + 0xf) & ~0xf);
        }

        // Yes nifti creator thinks that using a float for an offset is a good idea
        nifti_header->vox_offset = float(offset);

        ostream->write(reinterpret_cast<const char*>(nifti_header), sizeof(nifti_1_header));

        // Write padding
        if(const auto padding_size = std::streamsize(offset) - std::streamsize(sizeof(nifti_1_header));
           padding_size > 0)
        {
            std::string padding(std::size_t(padding_size), 0);
            ostream->write(padding.data(), padding_size);
        }

        // Write the image data as raw bytes
        ostream->write(reinterpret_cast<const char*>(image->buffer()), std::streamsize(image->size_in_bytes()));

#ifdef DEBUG_NIFTI
        // Write the image data to a file
        {
            std::ofstream os("C:\\TMP\\" + image->get_uuid() + ".nii", std::ios::binary);
            os.write(reinterpret_cast<const char*>(nifti_header), sizeof(nifti_1_header));

            if(const auto padding_size = std::streamsize(offset) - std::streamsize(sizeof(nifti_1_header));
               padding_size > 0)
            {
                std::string padding(std::size_t(padding_size), 0);
                os.write(padding.data(), padding_size);
            }

            os.write(reinterpret_cast<const char*>(image->buffer()), std::streamsize(image->size_in_bytes()));
        }

        // Try to read the file back
        {
            auto* nim = nifti_image_read(("C:\\TMP\\" + image->get_uuid() + ".nii").c_str(), 1);

            if(nim)
            {
                std::cout << "NIFTI image read successfully" << std::endl;
                std::cout << "Dimensions: " << nim->nx << " " << nim->ny << " " << nim->nz << std::endl;
                std::cout << "Data type: " << nim->datatype << std::endl;
                std::cout << "Data size: " << nim->nvox << std::endl;
                nifti_image_free(nim);
            }
            else
            {
                std::cerr << "Failed to read NIFTI image" << std::endl;
            }
        }
#endif

        free(nifti_header);
    }
}

//------------------------------------------------------------------------------

inline static data::image::sptr read(
    zip::archive_reader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto image           = helper::cast_or_create<data::image>(_object);
    const auto dump_lock = image->dump_lock();

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::image>(_tree, 2, 2);

    // Deserialize image
    const auto& size_tree          = _tree.get_child(SIZE);
    const data::image::size_t size = {
        size_tree.get<size_t>(WIDTH),
        size_tree.get<size_t>(HEIGHT),
        size_tree.get<size_t>(DEPTH)
    };

    core::type type(_tree.get<std::string>(TYPE));

    const auto format = static_cast<enum data::image::pixel_format_t>(_tree.get<int>(PIXEL_FORMAT));

    ///@note This is not saved in VTK files.
    std::vector<double> window_centers;
    for(const auto& value : _tree.get_child(WINDOW_CENTERS))
    {
        window_centers.push_back(value.second.get_value<double>());
    }

    image->set_window_center(window_centers);

    std::vector<double> window_widths;
    for(const auto& value : _tree.get_child(WINDOW_WIDTHS))
    {
        window_widths.push_back(value.second.get_value<double>());
    }

    image->set_window_width(window_widths);

    const auto& spacing_tree = _tree.get_child(SPACING);
    image->set_spacing(
        {
            spacing_tree.get<double>(X),
            spacing_tree.get<double>(Y),
            spacing_tree.get<double>(Z)
        });

    const auto& origin_tree = _tree.get_child(ORIGIN);
    image->set_origin(
        {
            origin_tree.get<double>(X),
            origin_tree.get<double>(Y),
            origin_tree.get<double>(Z)
        });

    const auto& orientation_tree = _tree.get_child(ORIENTATION);
    image->set_orientation(
        {
            orientation_tree.get<double>(X_X),
            orientation_tree.get<double>(Y_X),
            orientation_tree.get<double>(Z_X),
            orientation_tree.get<double>(X_Y),
            orientation_tree.get<double>(Y_Y),
            orientation_tree.get<double>(Z_Y),
            orientation_tree.get<double>(X_Z),
            orientation_tree.get<double>(Y_Z),
            orientation_tree.get<double>(Z_Z)
        });

    // If pixelFormart == UNDEFINED it is ALWAYS an empty image, so early return here.
    if(format == data::image::pixel_format_t::undefined)
    {
        return image;
    }

    const auto& serialized_uuid = _tree.get<std::string>(UUID);

    // Read the image data
    std::string content;

    {
        FW_PROFILE("read NIFTI");

        // Make room for the image
        image->resize(size, type, format);

        // No need to read an empty image
        if(image->size_in_bytes() != 0)
        {
            // Create the istream from the input file inside the archive
            const auto& istream = _archive.open_file(get_file_path(serialized_uuid), _password);

            // Read the header, just to have the offset, as we already have the information in the ptree
            nifti_1_header header;
            istream->read(reinterpret_cast<char*>(&header), sizeof(nifti_1_header));

            // Read the padding (we use a regular read, since seekg is not supported)
            if(const auto padding_size = std::streamsize(header.vox_offset) - std::streamsize(sizeof(nifti_1_header));
               padding_size > 0)
            {
                std::string padding(std::size_t(padding_size), 0);
                istream->read(padding.data(), padding_size);
            }

            // Read the image data as raw bytes
            istream->read(reinterpret_cast<char*>(image->buffer()), std::streamsize(image->size_in_bytes()));
        }
    }

    return image;
}

SIGHT_REGISTER_SERIALIZER(data::image, write, read);

} // namespace sight::io::session::detail::image
