/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include <sight/data/config.hpp>

#include "core/macros.hpp"

#include "series.hpp"

namespace gdcm
{

class SequenceOfItems;
template<typename T>
class SmartPointer;

} // namespace gdcm

namespace sight::data
{

/**
 * Class which represents a Spatial Fiducials DICOM file.
 *
 * Spatial Fiducials IOD contains sequences of sequences. There are two ways to access or modify data in a sequence.
 * The first one is to directly use the method, providing the index of the element in the sequence. The indexes are
 * always 0-indexed. For example, to get the shape_t of the second fiducial in the first fiducial set, you can do
 * this:
 * @code{.cpp}
   fiducialsSeries->set_shape_type(0, 1, fiducials_series::shape_t::POINT);
 * @endcode
 * The second one is to use the methods that return struct that represents the DICOM data, such as FiducialSet and
 * Fiducial. For example:
 * @code{.cpp}
   auto fiducialSets = fiducialsSeries->get_fiducial_sets();
   fiducialsSets[0].fiducials[1].shapeType = fiducials_series::shape_t::POINT;
   fiducialsSeries->setFiducialSets(fiducialSets);
 * @endcode
 * or:
 * @code{.cpp}
   auto fiducials = fiducialsSeries->get_fiducials(0);
   fiducials[1].shapeType = fiducials_series::shape_t::POINT;
   fiducialsSeries->setFiducials(0, fiducials);
 * @endcode
 */
class SIGHT_DATA_CLASS_API fiducials_series : public series
{
public:

    /// Struct which represents an element in the GraphicData (0070,0022) data element.
    struct point2
    {
        double x {0};
        double y {0};

        SIGHT_DATA_API bool operator==(point2 _other) const;
    };

    /// Struct which represents an element in the ContourData (3006,0050) data element.
    struct point3
    {
        double x {0};
        double y {0};
        double z {0};

        SIGHT_DATA_API bool operator==(point3 _other) const;
    };

    /// Enum which represents the value contained in shape_t (0070,0306).
    enum class shape
    {
        invalid /*Not defined by DICOM*/,
        point,
        line,
        plane,
        surface,
        ruler,
        l_shape,
        t_shape,
        shape
    };

    enum class private_shape
    {
        sphere,
        cube
    };

    /// Struct which represents an element in the ReferencedImageSequence (0008,1140) data element.
    struct referenced_image
    {
        /// ReferencedSOPClassUID (0008,1150)
        std::string referenced_sop_class_uid {};

        /// ReferencedSOPInstanceUID (0008,1155)
        std::string referenced_sop_instance_uid {};

        /// ReferencedFrameNumber (0008,1160)
        std::vector<std::int32_t> referenced_frame_number {};

        /// ReferencedSegmentNumber (0062,000B)
        std::vector<std::uint16_t> referenced_segment_number {};

        SIGHT_DATA_API bool operator==(const referenced_image& _other) const;
        SIGHT_DATA_API bool operator!=(const referenced_image& _other) const;
    };

    /// Struct which represents an element in the GraphicCoordinatesDataSequence (0070,0318) data element.
    struct graphic_coordinates_data
    {
        /// ReferencedImageSequence (0008,1140)
        referenced_image referenced_image_sequence {};

        /// GraphicData (0070,0022)
        std::vector<point2> graphic_data {};

        SIGHT_DATA_API bool operator==(const graphic_coordinates_data& _other) const;
        SIGHT_DATA_API bool operator!=(const graphic_coordinates_data& _other) const;
    };

    /// Struct which represents an element in the FiducialSequence (0070,031E) data element.
    struct fiducial
    {
        /// ShapeType (0070,0306)
        shape shape_type {shape::invalid};

        /// FiducialDescription (0070,030F)
        std::string fiducial_description {};

        /// FiducialIdentifier (0070,0310)
        std::string fiducial_identifier {};

        /// GraphicCoordinatesDataSequence (0070,0318)
        // Shall always be 1, except if the fiducial spans over several frames
        // Should be matching the content of the contour data
        std::optional<std::vector<graphic_coordinates_data> > graphic_coordinates_data_sequence {std::nullopt};

        /// FiducialUID (0070,031A)
        std::optional<std::string> fiducial_uid {std::nullopt};

        /// ContourData (3006,0050) and NumberOfContourPoints (3006,0046)
        std::vector<point3> contour_data {};

        SIGHT_DATA_API bool operator==(const fiducial& _other) const;
        SIGHT_DATA_API bool operator!=(const fiducial& _other) const;

        [[nodiscard]] SIGHT_DATA_API std::tuple<point3, point3> contour_data_bounding_box() const;
        [[nodiscard]] SIGHT_DATA_API
        std::optional<std::tuple<point2, point2> > graphic_coordinates_data_bounding_box(
            std::size_t _index
        ) const;
    };

    /// Struct which represents an element in the FiducialSetSequence (0070,031C) data element.
    struct fiducial_set
    {
        /// ReferencedImageSequence (0008,1140)
        std::optional<std::vector<referenced_image> > referenced_image_sequence {std::nullopt};

        /// FrameOfReference (0020,0052)
        std::optional<std::string> frame_of_reference_uid {std::nullopt};

        /// FiducialSequence (0070,031E)
        std::vector<fiducial> fiducial_sequence {};

        /// Private tags
        std::optional<std::string> group_name {std::nullopt};

        // float RGBA colors in [0.0, 1.0]
        std::optional<std::array<float, 4> > color {std::nullopt};
        std::optional<float> size {std::nullopt};
        std::optional<private_shape> shape {std::nullopt};
        std::optional<bool> visibility {std::nullopt};

        SIGHT_DATA_API bool operator==(const fiducial_set& _other) const;
        SIGHT_DATA_API bool operator!=(const fiducial_set& _other) const;
    };

    SIGHT_DECLARE_CLASS(fiducials_series, series);

    SIGHT_DATA_API explicit fiducials_series();
    SIGHT_DATA_API ~fiducials_series() noexcept override = default;

    SIGHT_DATA_API bool operator==(const fiducials_series& _other) const;
    SIGHT_DATA_API bool operator!=(const fiducials_series& _other) const;

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /**
     * Getter/Setter for the ContentDate (0008,0023) data element.
     * @{
     */
    SIGHT_DATA_API std::string get_content_date() const noexcept;
    SIGHT_DATA_API void set_content_date(const std::string& _content_date);
    /// @}

    /**
     * Getter/Setter for the ContentLabel (0070,0080) data element.
     * @{
     */
    SIGHT_DATA_API std::string get_content_label() const noexcept;
    SIGHT_DATA_API void set_content_label(const std::string& _content_label);
    /// @}

    /**
     * Getter/Setter for the ContentDescription (0070,0081) data element.
     * @{
     */
    SIGHT_DATA_API std::string get_content_description() const noexcept;
    SIGHT_DATA_API void set_content_description(const std::string& _content_description);
    /// @}

    /**
     * Getter/Setter for the ContentCreatorName (0070,0084) data element.
     * @{
     */
    SIGHT_DATA_API std::string get_content_creator_name() const noexcept;
    SIGHT_DATA_API void set_content_creator_name(const std::string& _content_creator_name);
    /// @}

    /**
     * Getter for the FiducialSetSequence (0070,031C) data element.
     * @return The fiducial set sequence as a vector of FiducialSet struct.
     */
    SIGHT_DATA_API std::vector<fiducial_set> get_fiducial_sets() const noexcept;
    /**
     * Setter for the FiducialSetSequence (0070,031C) data element. Replaces the existing content with the provided
     * vector of fiducial sets.
     * @param _fiducial_sets The vector of fiducial sets which will replace the existing content.
     */
    SIGHT_DATA_API void set_fiducial_sets(const std::vector<fiducial_set>& _fiducial_sets);
    /**
     * Setter for the FiducialSetSequence (0070,031C) data element. Replaces the element at index _fiducial_set_number
     * with
     * the provided fiducial set. If the element or the previous elements don't exist, they are created first.
     * @param _fiducial_set_number  The 0-indexed index of the sequence to be replaced.
     * @param _fiducial_set         The fiducial set which will replace the existing one.
     */
    SIGHT_DATA_API void set_fiducial_set(std::size_t _fiducial_set_number, fiducial_set _fiducial_set);
    /**
     * Setter for the FiducialSetSequence (0070,031C) data element. Appends the provided fiducial set at the end of the
     * sequence.
     * @param _fiducial_set The fiducialSet to be appended to the sequence.
     */
    SIGHT_DATA_API void append_fiducial_set(fiducial_set _fiducial_set);

    /**
     * Getter for the ReferencedImageSequence (0008,1140) data element at fiducial set level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose referenced image sequence must be
     * fetched.
     * @return The referenced image sequence as a vector of ReferencedImage struct; or nullopt if it isn't defined.
     */
    SIGHT_DATA_API std::optional<std::vector<referenced_image> > get_referenced_images(std::size_t _fiducial_set_number)
    const
    noexcept;
    /**
     * Setter for the ReferencedImageSequence (0008,1140) data element at fiducial set level. Replaces the existing
     * content with the provided vector of fiducial sets.
     * @param _fiducial_set_number  The 0-indexed index of the fiducial set whose referenced image sequence must be
     * modified
     * @param _referenced_images    The referenced image sequence which will replace the existing one.
     */
    SIGHT_DATA_API void set_referenced_images(
        std::size_t _fiducial_set_number,
        const std::optional<std::vector<referenced_image> >& _referenced_images
    );
    /**
     * Setter for the ReferencedImageSequence (0008,1140) data element at fiducial set level. Replaces the element at
     * index _referenced_image_number with the provided referenced image. If the element or the previous elements don't
     * exist, they are created first.
     * @param _fiducial_set_number      The 0-indexed index of the fiducial set whose referenced image sequence must be
     * modified
     * @param _referenced_image_number  The 0-indexed index of the sequence to be replaced.
     * @param _referenced_image         The referenced image which will replace the existing one.
     */
    SIGHT_DATA_API void set_referenced_image(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        referenced_image _referenced_image
    );
    /**
     * Setter for the ReferencedImageSequence (0008,1140) data element at fiducial set level. Appends the provided
     * referenced image at the end of the sequence.
     * @param _fiducial_set_number  The 0-indexed index of the fiducial set whose referenced image sequence must be
     * modified
     * @param _referenced_image     The referenced image to be appended to the sequence.
     */
    SIGHT_DATA_API void append_referenced_image(std::size_t _fiducial_set_number, referenced_image _referenced_image);

    /**
     * Getter/Setter for the ReferencedSOPClassUID (0008,1150) data element at fiducial set level.
     * @param _fiducial_set_number      The 0-indexed index of the fiducial set whose referenced image sequence must be
     * fetched.
     * @param _referenced_image_number  The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<std::string> get_referenced_sop_class_uid(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_sop_class_uid(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        const std::string& _referenced_sop_class_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSOPInstanceUID (0008,1155) data element at fiducial set level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose referenced image sequence must be
     * fetched.
     * @param _referenced_image_number The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<std::string> get_referenced_sop_instance_uid(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_sop_instance_uid(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        const std::string& _referenced_sop_instance_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedFrameNumber (0008,1160) data element at fiducial set level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose referenced image sequence must be
     * fetched.
     * @param _referenced_image_number The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    SIGHT_DATA_API std::vector<std::int32_t> get_referenced_frame_number(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_frame_number(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        std::vector<std::int32_t> _referenced_frame_number
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSegmentNumber (0062,000B) data element at fiducial set level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose referenced image sequence must be
     * fetched.
     * @param _referenced_image_number The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    SIGHT_DATA_API std::vector<std::uint16_t> get_referenced_segment_number(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_segment_number(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        std::vector<std::uint16_t> _referenced_segment_number
    );
    /// @}

    /**
     * Getter/Setter for the FrameOfReferenceUID (0020,0052) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<std::string> get_frame_of_reference_uid(std::size_t _fiducial_set_number) const
    noexcept;
    SIGHT_DATA_API void set_frame_of_reference_uid(
        std::size_t _fiducial_set_number,
        const std::optional<std::string>& _frame_of_reference_uid
    );
    /// @}

    /**
     * Getter for the FiducialSequence (0070,031E) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose fiducial sequence must be fetched.
     * @return The fiducial sequence as a vector of Fiducial struct.
     */
    SIGHT_DATA_API std::vector<fiducial> get_fiducials(std::size_t _fiducial_set_number) const noexcept;

    /**
     * Setter for the FiducialSequence (0070,031E) data element. Replaces the existing content with the provided vector
     * of fiducials.
     * @param _fiducial_set_number  The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducials            The fiducial sequence which will replace the existing one.
     */
    SIGHT_DATA_API void set_fiducials(std::size_t _fiducial_set_number, const std::vector<fiducial>& _fiducials);

    /**
     * Setter for the FiducialSequence (0070,031E) data element. Replaces the element at index _fiducial_number with the
     * provided fiducial. If the element or the previous elements don't exist, they are created first.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducial_number The 0-indexed index of the sequence to be replaced.
     * @param _fiducial The fiducial which will replace the existing one.
     */
    SIGHT_DATA_API void set_fiducial(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        fiducial _fiducial
    );

    /**
     * Setter for the FiducialSequence (0070,031E) data element. Appends the provided fiducial at the end of the
     * sequence.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducial The fiducial to be appended to the sequence.
     */
    SIGHT_DATA_API void append_fiducial(std::size_t _fiducial_set_number, fiducial _fiducial);

    /**
     * Getter/Setter for the shape_t (0070,0306) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API shape get_shape_type(std::size_t _fiducial_set_number, std::size_t _fiducial_number) const noexcept;
    SIGHT_DATA_API void set_shape_type(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        shape _shape_type
    );
    /// @}

    /**
     * Getter/Setter for the FiducialDescription (0070,030F) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::string get_fiducial_description(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    SIGHT_DATA_API void set_fiducial_description(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::string& _fiducial_description
    );
    /// @}

    /**
     * Getter/Setter for the FiducialIdentifier (0070,0310) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::string get_fiducial_identifier(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    SIGHT_DATA_API void set_fiducial_identifier(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::string& _fiducial_identifier
    );
    /// @}

    /**
     * Getter for the GraphicCoordinatesDataSequence (0070,0318) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose graphic coordinates data must be fetched.
     * @return The graphic coordinates data sequence as a vector for GraphicCoordinatesData struct.
     */
    SIGHT_DATA_API std::optional<std::vector<graphic_coordinates_data> > get_graphic_coordinates_data_sequence(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    /**
     * Setter for the GraphicCoordinatesDataSequence (0070,0318) data element. Replaces the existing content with the
     * provided vector of graphic coordinates data.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducial_number The 0-indexed index of the fiducial whose graphic coordinate data sequence must be
     * modified
     * @param _graphic_coordinates_data_sequence The graphic coordinates data sequence which will replace the existing
     * one.
     */
    SIGHT_DATA_API void set_graphic_coordinates_data_sequence(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::optional<std::vector<graphic_coordinates_data> >& _graphic_coordinates_data_sequence
    );
    /**
     * Setter for the GraphicCoordinatesDataSequence (0070,0318) data element. Replaces the element at index
     * graphicCoordinatesDataNumber with the provided fiducial. If the element or the previous elements don't exist,
     * they are created first.
     * @param _fiducial_set_number  The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducial_number  The 0-indexed index of the fiducial whose graphic coordinate data sequence must be
     * modified
     * @param _graphic_coordinates_data_number  The 0-indexed index of the sequence to be replaced.
     * @param _graphic_coordinates_data         The graphic coordinates data which will replace the existing one.
     */
    SIGHT_DATA_API void set_graphic_coordinates_data(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        graphic_coordinates_data _graphic_coordinates_data
    );
    /**
     * Setter for the GraphicCoordinatesDataSequence (0070,0318) data element. Appends the provided graphic coordinates
     * data at the end of the sequence.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducial_number The 0-indexed index of the fiducial whose graphic coordinate data sequence must be
     * modified
     * @param _graphic_coordinates_data The graphic coordinates data to be appended to the sequence.
     */
    SIGHT_DATA_API void append_graphic_coordinates_data(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        graphic_coordinates_data _graphic_coordinates_data
    );

    /**
     * Getter/Setter for the ReferencedImage (0008,1140) data element at fiducial level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param _fiducial_number The 0-indexed index of the fiducial whose graphic coordinate data sequence must be
     * modified
     * @param _graphic_coordinates_data_number The graphic coordinates data to be appended to the sequence.
     * @{
     */
    SIGHT_DATA_API referenced_image get_referenced_image(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_image(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        referenced_image _referenced_image
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSOPClassUID (0008,1150) data element at graphic coordinates data level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @param _graphic_coordinates_data_number The 0-indexed index of the graphic coordinate data whose data must be
     * fetched
     * @{
     */
    SIGHT_DATA_API std::string get_referenced_sop_class_uid(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_sop_class_uid(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        const std::string& _referenced_sop_class_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSOPInstanceUID (0008,1155) data element at graphic coordinates data level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @param _graphic_coordinates_data_number The 0-indexed index of the graphic coordinate data whose data must be
     * fetched
     * @{
     */
    SIGHT_DATA_API std::string get_referenced_sop_instance_uid(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_sop_instance_uid(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        const std::string& _referenced_sop_instance_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedFrameNumber (0008,1160) data element at graphic coordinates data level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @param _graphic_coordinates_data_number The 0-indexed index of the graphic coordinate data whose data must be
     * fetched
     * @{
     */
    SIGHT_DATA_API std::vector<std::int32_t> get_referenced_frame_number(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_frame_number(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        std::vector<std::int32_t> _referenced_frame_number
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSegmentNumber (0062,000B) data element at graphic coordinates data level.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @param _graphic_coordinates_data_number The 0-indexed index of the graphic coordinate data whose data must be
     * fetched
     * @{
     */
    SIGHT_DATA_API std::vector<std::uint16_t> get_referenced_segment_number(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    SIGHT_DATA_API void set_referenced_segment_number(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        std::vector<std::uint16_t> _referenced_segment_number
    );
    /// @}

    /**
     * Getter/Setter for the GraphicData (0070,0022) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @param _graphic_coordinates_data_number The 0-indexed index of the graphic coordinate data whose data must be
     * fetched
     * @{
     */
    SIGHT_DATA_API std::vector<point2> get_graphic_data(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    SIGHT_DATA_API void set_graphic_data(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        const std::vector<point2>& _graphic_data
    );
    /// @}

    /**
     * Getter/Setter for the FiducialUID (0070,031A) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<std::string> get_fiducial_uid(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    SIGHT_DATA_API void set_fiducial_uid(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::optional<std::string>& _fiducial_uid
    );
    /// @}

    /**
     * Getter/Setter for the ContourData (3006,0050) data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @param _fiducial_number The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::vector<point3> get_contour_data(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    SIGHT_DATA_API void set_contour_data(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::vector<point3>& _contour_data
    );
    /// @}

    /** Getter/Setter for the GroupName private tag data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<std::string> get_group_name(std::size_t _fiducial_set_number) const noexcept;
    SIGHT_DATA_API void set_group_name(std::size_t _fiducial_set_number, const std::string& _group_name);
    /// @}

    /**
     * Getter/Setter for the Color private tag data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<std::array<float, 4> > get_color(std::size_t _fiducial_set_number) const noexcept;
    SIGHT_DATA_API void set_color(std::size_t _fiducial_set_number, const std::array<float, 4>& _color);
    /// @}

    /**
     * Getter/Setter for the Size private tag data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<float> get_size(std::size_t _fiducial_set_number) const noexcept;
    SIGHT_DATA_API void set_size(std::size_t _fiducial_set_number, float _size);
    /// @}

    /**
     * Getter/Setter for the Shape private tag data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<private_shape> get_shape(std::size_t _fiducial_set_number) const noexcept;
    SIGHT_DATA_API void set_shape(std::size_t _fiducial_set_number, private_shape _shape);
    /// @}

    /**
     * Getter/Setter for the Visibility private tag data element.
     * @param _fiducial_set_number The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    SIGHT_DATA_API std::optional<bool> get_visibility(std::size_t _fiducial_set_number) const noexcept;
    SIGHT_DATA_API void set_visibility(std::size_t _fiducial_set_number, bool _visibility);
    /// @}

    // Helper methods

    /**
     * Set group names for fiducial sets which contains point fiducials which doesn't have group names
     */
    SIGHT_DATA_API void set_group_names_for_point_fiducials();

    /**
     * Get the list of group names for fiducial sets which contains point fiducials
     * @return The list of group names
     */
    [[nodiscard]] SIGHT_DATA_API std::vector<std::string> get_point_fiducials_group_names() const;

    /**
     * Get the fiducial set which has the name in parameter and its index
     * @param _group_name The name of the group to fetch
     * @return A pair with the fiducial set and its index, or std::nullopt if the group name doesn't exist
     */
    [[nodiscard]] SIGHT_DATA_API std::optional<std::pair<fiducial_set, std::size_t> > get_fiducial_set_and_index(
        const std::string& _group_name
    ) const;

    struct SIGHT_DATA_CLASS_API query_result final
    {
        std::size_t m_fiducial_set_index {0};
        std::size_t m_fiducial_index {0};
        std::size_t m_shape_index {0};

        std::optional<std::string> m_frame_of_reference_uid {};
        std::optional<std::vector<std::int32_t> > m_referenced_frame_number {};

        std::optional<std::string> m_group_name {};
        std::optional<bool> m_visible {};
        std::optional<float> m_size {};
        std::optional<private_shape> m_private_shape {};
        std::optional<std::array<float, 4> > m_color {};

        std::optional<shape> m_shape {};
        std::optional<std::vector<double> > m_contour_data {};
        std::optional<std::vector<float> > m_graphic_data {};
        std::optional<std::string> m_fiducial_description {};
        std::optional<std::string> m_fiducial_identifier {};
        std::optional<std::string> m_fiducial_uid {};
    };

    /**
     * @brief Query fiducials based on the given parameters.
     *
     * @param _predicate function to filter fiducials (return true to keep the fiducial)
     * @param _shape filter by shape (point, ruler, ...)
     * @param _group_name filter by group name
     * @param _fiducial_index filter by index of the same shape type in the fiducial set
     * @return std::vector<query_result> the query result
     */
    [[nodiscard]] SIGHT_DATA_API std::vector<query_result> query_fiducials(
        const std::optional<std::function<bool(const query_result&)> >& _predicate = std::nullopt,
        const std::optional<shape>& _shape                                         = std::nullopt,
        const std::optional<std::string_view>& _group_name                         = std::nullopt,
        const std::optional<std::size_t>& _fiducial_index                          = std::nullopt
    ) const;

    /**
     * @brief Remove fiducials based on the given parameters. Empty fiducial set are also removed.
     *
     * @param _predicate function to filter fiducials (return true to remove the fiducial)
     * @param _shape filter by shape (point, ruler, ...)
     * @param _group_name filter by group name
     * @param _fiducial_index filter by index of the same shape type in the fiducial set
     * @return std::pair<std::vector<query_result>, std::set<std::string> > the removed fiducials and a set of removed
     *         group names
     */
    SIGHT_DATA_API std::pair<std::vector<query_result>, std::set<std::string> > remove_fiducials(
        const std::optional<std::function<bool(const query_result&)> >& _predicate = std::nullopt,
        const std::optional<shape>& _shape                                         = std::nullopt,
        const std::optional<std::string_view>& _group_name                         = std::nullopt,
        const std::optional<std::size_t>& _fiducial_index                          = std::nullopt
    );

    /**
     * @brief Modify fiducials based on the given parameters.
     *
     * @param _predicate function to filter fiducials and/or change fiducial values (return true to modify the
     *                   fiducial). Without predicate, the behavior is equivalent to query_fiducials().
     * @param _shape filter by shape (point, ruler, ...)
     * @param _group_name filter by group name
     * @param _fiducial_index filter by index of the same shape type in the fiducial set
     * @return std::vector<query_result> the modified fiducials
     */
    SIGHT_DATA_API std::vector<query_result> modify_fiducials(
        const std::optional<std::function<bool(query_result&)> >& _predicate = std::nullopt,
        const std::optional<shape>& _shape                                   = std::nullopt,
        const std::optional<std::string_view>& _group_name                   = std::nullopt,
        const std::optional<std::size_t>& _fiducial_index                    = std::nullopt
    );

    /**
     * @brief Append one fiducial based on the given parameters.
     *
     * @param _predicate function to set fiducial values (return true to add the fiducial)
     * @param _shape filter by shape (point, ruler, ...)
     * @param _group_name filter by group name
     * @param _fiducial_index filter by index of the same shape type in the fiducial set
     * @return std::pair<std::optional<query_result>, bool> the added fiducial, and a boolean indicating if a
     *         fiducial set was created
     */
    SIGHT_DATA_API std::pair<std::optional<query_result>, bool> add_fiducial(
        const std::function<bool(query_result&)>& _predicate,
        shape _shape,
        const std::string& _group_name
    );

    /**
     * Get the number of point fiducials in a group
     * @param _group_name The name of the group to fetch
     * @return The number of points in the group, or std::nullopt if the group name doesn't exist
     */
    [[nodiscard]] SIGHT_DATA_API std::optional<std::size_t> get_number_of_points_in_group(
        const std::string& _group_name
    )
    const;

    /**
     * Returns the 3D position of the point fiducial using Contour Data.
     * @param _fiducial The fiducial whose 3D position must be found
     * @return The 3D position of the fiducial, or std::nullopt if it has no Contour Data (getting 3D position using
     * Graphic Coordinates Data Sequence isn't supported) or if its shape type isn't point.
     */
    [[nodiscard]] SIGHT_DATA_API static std::optional<std::array<double, 3> > get_point(const fiducial& _fiducial);

    /**
     * Returns the 3D position of the point INDEX in group GROUP_NAME
     * @param _group_name The name of the group of the point
     * @param _index The index of the point in its group
     * @return The 3D position of the fiducial, or std::nullopt if the group doesn't exist or if it has no Contour Data
     * (getting 3D position using Graphic Coordinates Data Sequence isn't supported) or if its shape type isn't point.
     */
    [[nodiscard]] SIGHT_DATA_API std::optional<std::array<double, 3> > get_point(
        const std::string& _group_name,
        std::size_t _index
    ) const;

    /**
     * Remove the point INDEX in group GROUP_NAME
     * @param _group_name The name of the group of the point
     * @param _index The index of the point in its group
     */
    SIGHT_DATA_API void remove_point(const std::string& _group_name, std::size_t _index);

    /**
     * Remove the group GROUP_NAME
     * @param _group_name The name of the group to be removed
     */
    SIGHT_DATA_API void remove_group(const std::string& _group_name);

    /**
     * Add a new fiducial set with the given parameters
     * @param _group_name The name of the new fiducial set
     * @param _color The color of the new fiducial set
     * @param _size The size of the points in the new fiducial set
     */
    SIGHT_DATA_API void add_group(const std::string& _group_name, const std::array<float, 4>& _color, float _size);

    /**
     * Add a point fiducial in a fiducial set
     * @param _group_name   The name of the group of the point
     * @param _position     The 3D position of the new point
     * @return Whether the point has been added or not.
     */
    SIGHT_DATA_API bool add_point(const std::string& _group_name, const std::array<double, 3>& _position);

    SIGHT_DATA_API static std::string shape_to_string(fiducials_series::shape _shape);

private:

    template<typename T>
    T to(const gdcm::DataSet& _data_set) const;

    fiducial_set to_fiducial_set(const gdcm::DataSet& _data_set) const;
    referenced_image to_referenced_image(const gdcm::DataSet& _data_set) const;
    fiducial to_fiducial(const gdcm::DataSet& _data_set) const;
    graphic_coordinates_data to_graphic_coordinates_data(const gdcm::DataSet& _data_set) const;

    static std::vector<point2> to_point2(const std::vector<float>& _floats);

    static std::vector<point3> to_point3(const std::vector<double>& _floats);

    template<typename T>
    std::optional<std::vector<T> > to_vector(const gdcm::SmartPointer<gdcm::SequenceOfItems>& _sequence) const;

    template<typename T>
    gdcm::SmartPointer<gdcm::SequenceOfItems> to_sequence(const std::optional<std::vector<T> >& _vector) const;
    template<typename T>
    gdcm::SmartPointer<gdcm::SequenceOfItems> to_sequence(const std::vector<T>& _vector) const;
};

} // namespace sight::data
