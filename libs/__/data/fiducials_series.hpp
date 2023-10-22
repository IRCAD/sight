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

#include "core/com/signals.hpp"
#include "core/macros.hpp"

#include "data/config.hpp"

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
   auto fiducialSets = fiducialsSeries->getFiducialSets();
   fiducialsSets[0].fiducials[1].shapeType = fiducials_series::shape_t::POINT;
   fiducialsSeries->setFiducialSets(fiducialSets);
 * @endcode
 * or:
 * @code{.cpp}
   auto fiducials = fiducialsSeries->getFiducials(0);
   fiducials[1].shapeType = fiducials_series::shape_t::POINT;
   fiducialsSeries->setFiducials(0, fiducials);
 * @endcode
 */
class DATA_CLASS_API fiducials_series : public series
{
public:

    /// Struct which represents an element in the GraphicData (0070,0022) data element.
    struct Point2
    {
        double x {0};
        double y {0};

        DATA_API bool operator==(Point2 _other) const;
    };

    /// Struct which represents an element in the ContourData (3006,0050) data element.
    struct Point3
    {
        double x {0};
        double y {0};
        double z {0};

        DATA_API bool operator==(Point3 _other) const;
    };

    /// Enum which represents the value contained in shape_t (0070,0306).
    enum class Shape
    {
        /// Not defined by DICOM; used if the file doesn't contain a valid enumeration value
        INVALID,
        POINT,
        LINE,
        PLANE,
        SURFACE,
        RULER,
        L_SHAPE,
        T_SHAPE,
        SHAPE
    };

    enum class PrivateShape
    {
        SPHERE,
        CUBE
    };

    /// Struct which represents an element in the ReferencedImageSequence (0008,1140) data element.
    struct ReferencedImage
    {
        std::string referencedSOPClassUID;                  /// ReferencedSOPClassUID (0008,1150)
        std::string referencedSOPInstanceUID;               /// ReferencedSOPInstanceUID (0008,1155)
        std::vector<std::int32_t> referencedFrameNumber;    /// ReferencedFrameNumber (0008,1160)
        std::vector<std::uint16_t> referencedSegmentNumber; /// ReferencedSegmentNumber (0062,000B)

        DATA_API bool operator==(const ReferencedImage& _other) const;
        DATA_API bool operator!=(const ReferencedImage& _other) const;
    };

    /// Struct which represents an element in the GraphicCoordinatesDataSequence (0070,0318) data element.
    struct GraphicCoordinatesData
    {
        ReferencedImage referencedImageSequence; /// ReferencedImageSequence (0008,1140)
        std::vector<Point2> graphicData;         /// GraphicData (0070,0022)

        DATA_API bool operator==(const GraphicCoordinatesData& _other) const;
        DATA_API bool operator!=(const GraphicCoordinatesData& _other) const;
    };

    /// Struct which represents an element in the FiducialSequence (0070,031E) data element.
    struct Fiducial
    {
        Shape shapeType;                                                                    /// shape_t (0070,0306)
        std::string fiducialDescription;                                                    /// FiducialDescription
                                                                                            /// (0070,030F)
        std::string fiducialIdentifier;                                                     /// FiducialIdentifier
                                                                                            /// (0070,0310)
        std::optional<std::vector<GraphicCoordinatesData> > graphicCoordinatesDataSequence; // GraphicCoordinatesDataSequence
                                                                                            // (0070,0318)
        std::optional<std::string> fiducialUID;                                             /// FiducialUID (0070,031A)
        std::vector<Point3> contourData;                                                    /// ContourData (3006,0050)
                                                                                            /// (NumberOfContourPoints
                                                                                            /// (3006,0046) included
                                                                                            /// inside)

        DATA_API bool operator==(const Fiducial& _other) const;
        DATA_API bool operator!=(const Fiducial& _other) const;
    };

    /// Struct which represents an element in the FiducialSetSequence (0070,031C) data element.
    struct FiducialSet
    {
        std::optional<std::vector<ReferencedImage> > referencedImageSequence; /// ReferencedImageSequence (0008,1140)
        std::optional<std::string> frameOfReferenceUID;                       /// FrameOfReference (0020,0052)
        std::vector<Fiducial> fiducialSequence;                               /// FiducialSequence (0070,031E)
        std::optional<std::string> groupName;                                 /// Private tag
        std::optional<std::array<float, 4> > color;                           /// Private tag
        std::optional<float> size;                                            /// Private tag
        std::optional<PrivateShape> shape;                                    /// Private tag
        std::optional<bool> visibility;                                       /// Private tag

        DATA_API bool operator==(const FiducialSet& _other) const;
        DATA_API bool operator!=(const FiducialSet& _other) const;
    };

    SIGHT_DECLARE_CLASS(fiducials_series, series);

    DATA_API explicit fiducials_series();
    DATA_API ~fiducials_series() noexcept override = default;

    DATA_API bool operator==(const fiducials_series& _other) const;
    DATA_API bool operator!=(const fiducials_series& _other) const;

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /**
     * Getter/Setter for the ContentDate (0008,0023) data element.
     * @{
     */
    DATA_API std::string getContentDate() const noexcept;
    DATA_API void setContentDate(const std::string& _content_date);
    /// @}

    /**
     * Getter/Setter for the ContentLabel (0070,0080) data element.
     * @{
     */
    DATA_API std::string getContentLabel() const noexcept;
    DATA_API void setContentLabel(const std::string& _content_label);
    /// @}

    /**
     * Getter/Setter for the ContentDescription (0070,0081) data element.
     * @{
     */
    DATA_API std::string getContentDescription() const noexcept;
    DATA_API void setContentDescription(const std::string& _content_description);
    /// @}

    /**
     * Getter/Setter for the ContentCreatorName (0070,0084) data element.
     * @{
     */
    DATA_API std::string getContentCreatorName() const noexcept;
    DATA_API void setContentCreatorName(const std::string& _content_creator_name);
    /// @}

    /**
     * Getter for the FiducialSetSequence (0070,031C) data element.
     * @return The fiducial set sequence as a vector of FiducialSet struct.
     */
    DATA_API std::vector<FiducialSet> getFiducialSets() const noexcept;
    /**
     * Setter for the FiducialSetSequence (0070,031C) data element. Replaces the existing content with the provided
     * vector of fiducial sets.
     * @param fiducialSets The vector of fiducial sets which will replace the existing content.
     */
    DATA_API void setFiducialSets(const std::vector<FiducialSet>& _fiducial_sets);
    /**
     * Setter for the FiducialSetSequence (0070,031C) data element. Replaces the element at index fiducialSetNumber with
     * the provided fiducial set. If the element or the previous elements don't exist, they are created first.
     * @param fiducialSetNumber The 0-indexed index of the sequence to be replaced.
     * @param fiducialSet The fiducial set which will replace the existing one.
     */
    DATA_API void setFiducialSet(std::size_t _fiducial_set_number, FiducialSet _fiducial_set);
    /**
     * Setter for the FiducialSetSequence (0070,031C) data element. Appends the provided fiducial set at the end of the
     * sequence.
     * @param fiducialSet The fiducialSet to be appended to the sequence.
     */
    DATA_API void appendFiducialSet(FiducialSet _fiducial_set);

    /**
     * Getter for the ReferencedImageSequence (0008,1140) data element at fiducial set level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be fetched.
     * @return The referenced image sequence as a vector of ReferencedImage struct; or nullopt if it isn't defined.
     */
    DATA_API std::optional<std::vector<ReferencedImage> > getReferencedImages(std::size_t _fiducial_set_number) const
    noexcept;
    /**
     * Setter for the ReferencedImageSequence (0008,1140) data element at fiducial set level. Replaces the existing
     * content with the provided vector of fiducial sets.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be modified
     * @param referencedImages The referenced image sequence which will replace the existing one.
     */
    DATA_API void setReferencedImages(
        std::size_t _fiducial_set_number,
        const std::optional<std::vector<ReferencedImage> >& _referenced_images
    );
    /**
     * Setter for the ReferencedImageSequence (0008,1140) data element at fiducial set level. Replaces the element at
     * index referencedImageNumber with the provided referenced image. If the element or the previous elements don't
     * exist, they are created first.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be modified
     * @param referencedImageNumber The 0-indexed index of the sequence to be replaced.
     * @param referencedImage The referenced image which will replace the existing one.
     */
    DATA_API void setReferencedImage(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        ReferencedImage _referenced_image
    );
    /**
     * Setter for the ReferencedImageSequence (0008,1140) data element at fiducial set level. Appends the provided
     * referenced image at the end of the sequence.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be modified
     * @param referencedImage The referenced image to be appended to the sequence.
     */
    DATA_API void appendReferencedImage(std::size_t _fiducial_set_number, ReferencedImage _referenced_image);

    /**
     * Getter/Setter for the ReferencedSOPClassUID (0008,1150) data element at fiducial set level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be fetched.
     * @param referencedImageNumber The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    DATA_API std::optional<std::string> getReferencedSOPClassUID(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    DATA_API void setReferencedSOPClassUID(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        const std::string& _referenced_sop_class_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSOPInstanceUID (0008,1155) data element at fiducial set level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be fetched.
     * @param referencedImageNumber The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    DATA_API std::optional<std::string> getReferencedSOPInstanceUID(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    DATA_API void setReferencedSOPInstanceUID(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        const std::string& _referenced_sop_instance_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedFrameNumber (0008,1160) data element at fiducial set level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be fetched.
     * @param referencedImageNumber The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    DATA_API std::vector<std::int32_t> getReferencedFrameNumber(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    DATA_API void setReferencedFrameNumber(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        std::vector<std::int32_t> _referenced_frame_number
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSegmentNumber (0062,000B) data element at fiducial set level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose referenced image sequence must be fetched.
     * @param referencedImageNumber The 0-indexed index of the referenced image to be fetched.
     * @{
     */
    DATA_API std::vector<std::uint16_t> getReferencedSegmentNumber(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number
    ) const noexcept;
    DATA_API void setReferencedSegmentNumber(
        std::size_t _fiducial_set_number,
        std::size_t _referenced_image_number,
        std::vector<std::uint16_t> _referenced_segment_number
    );
    /// @}

    /**
     * Getter/Setter for the FrameOfReferenceUID (0020,0052) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    DATA_API std::optional<std::string> getFrameOfReferenceUID(std::size_t _fiducial_set_number) const noexcept;
    DATA_API void setFrameOfReferenceUID(
        std::size_t _fiducial_set_number,
        const std::optional<std::string>& _frame_of_reference_uid
    );
    /// @}

    /**
     * Getter for the FiducialSequence (0070,031E) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be fetched.
     * @return The fiducial sequence as a vector of Fiducial struct.
     */
    DATA_API std::vector<Fiducial> getFiducials(std::size_t _fiducial_set_number) const noexcept;
    /**
     * Setter for the FiducialSequence (0070,031E) data element. Replaces the existing content with the provided vector
     * of fiducials.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducials The fiducial sequence which will replace the existing one.
     */
    DATA_API void setFiducials(std::size_t _fiducial_set_number, const std::vector<Fiducial>& _fiducials);
    /**
     * Setter for the FiducialSequence (0070,031E) data element. Replaces the element at index fiducialNumber with the
     * provided fiducial. If the element or the previous elements don't exist, they are created first.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducialNumber The 0-indexed index of the sequence to be replaced.
     * @param fiducial The fiducial which will replace the existing one.
     */
    DATA_API void setFiducial(std::size_t _fiducial_set_number, std::size_t _fiducial_number, Fiducial _fiducial);
    /**
     * Setter for the FiducialSequence (0070,031E) data element. Appends the provided fiducial at the end of the
     * sequence.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducial The fiducial to be appended to the sequence.
     */
    DATA_API void appendFiducial(std::size_t _fiducial_set_number, Fiducial _fiducial);

    /**
     * Getter/Setter for the shape_t (0070,0306) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    DATA_API Shape get_shape_type(std::size_t _fiducial_set_number, std::size_t _fiducial_number) const noexcept;
    DATA_API void set_shape_type(std::size_t _fiducial_set_number, std::size_t _fiducial_number, Shape _shape_type);
    /// @}

    /**
     * Getter/Setter for the FiducialDescription (0070,030F) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    DATA_API std::string getFiducialDescription(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    DATA_API void setFiducialDescription(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::string& _fiducial_description
    );
    /// @}

    /**
     * Getter/Setter for the FiducialIdentifier (0070,0310) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    DATA_API std::string getFiducialIdentifier(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    DATA_API void setFiducialIdentifier(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::string& _fiducial_identifier
    );
    /// @}

    /**
     * Getter for the GraphicCoordinatesDataSequence (0070,0318) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose graphic coordinates data must be fetched.
     * @return The graphic coordinates data sequence as a vector for GraphicCoordinatesData struct.
     */
    DATA_API std::optional<std::vector<GraphicCoordinatesData> > getGraphicCoordinatesDataSequence(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    /**
     * Setter for the GraphicCoordinatesDataSequence (0070,0318) data element. Replaces the existing content with the
     * provided vector of graphic coordinates data.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducialNumber The 0-indexed index of the fiducial whose graphic coordinate data sequence must be modified
     * @param graphicCoordinatesDataSequence The graphic coordinates data sequence which will replace the existing one.
     */
    DATA_API void setGraphicCoordinatesDataSequence(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::optional<std::vector<GraphicCoordinatesData> >& _graphic_coordinates_data_sequence
    );
    /**
     * Setter for the GraphicCoordinatesDataSequence (0070,0318) data element. Replaces the element at index
     * graphicCoordinatesDataNumber with the provided fiducial. If the element or the previous elements don't exist,
     * they are created first.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducialNumber The 0-indexed index of the fiducial whose graphic coordinate data sequence must be modified
     * @param graphicCoordinatesDataNumber The 0-indexed index of the sequence to be replaced.
     * @param graphicCoordinatesData The graphic coordinates data which will replace the existing one.
     */
    DATA_API void setGraphicCoordinatesData(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        GraphicCoordinatesData _graphic_coordinates_data
    );
    /**
     * Setter for the GraphicCoordinatesDataSequence (0070,0318) data element. Appends the provided graphic coordinates
     * data at the end of the sequence.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducialNumber The 0-indexed index of the fiducial whose graphic coordinate data sequence must be modified
     * @param graphicCoordinatesData The graphic coordinates data to be appended to the sequence.
     */
    DATA_API void appendGraphicCoordinatesData(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        GraphicCoordinatesData _graphic_coordinates_data
    );

    /**
     * Getter/Setter for the ReferencedImage (0008,1140) data element at fiducial level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose fiducial sequence must be modified.
     * @param fiducialNumber The 0-indexed index of the fiducial whose graphic coordinate data sequence must be modified
     * @param graphicCoordinatesData The graphic coordinates data to be appended to the sequence.
     * @{
     */
    DATA_API ReferencedImage getReferencedImage(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    DATA_API void setReferencedImage(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        ReferencedImage _referenced_image
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSOPClassUID (0008,1150) data element at graphic coordinates data level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @param graphicCoordinatesDataNumber The 0-indexed index of the graphic coordinate data whose data must be fetched
     * @{
     */
    DATA_API std::string getReferencedSOPClassUID(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    DATA_API void setReferencedSOPClassUID(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        const std::string& _referenced_sop_class_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSOPInstanceUID (0008,1155) data element at graphic coordinates data level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @param graphicCoordinatesDataNumber The 0-indexed index of the graphic coordinate data whose data must be fetched
     * @{
     */
    DATA_API std::string getReferencedSOPInstanceUID(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    DATA_API void setReferencedSOPInstanceUID(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        const std::string& _referenced_sop_instance_uid
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedFrameNumber (0008,1160) data element at graphic coordinates data level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @param graphicCoordinatesDataNumber The 0-indexed index of the graphic coordinate data whose data must be fetched
     * @{
     */
    DATA_API std::vector<std::int32_t> getReferencedFrameNumber(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    DATA_API void setReferencedFrameNumber(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        std::vector<std::int32_t> _referenced_frame_number
    );
    /// @}

    /**
     * Getter/Setter for the ReferencedSegmentNumber (0062,000B) data element at graphic coordinates data level.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @param graphicCoordinatesDataNumber The 0-indexed index of the graphic coordinate data whose data must be fetched
     * @{
     */
    DATA_API std::vector<std::uint16_t> getReferencedSegmentNumber(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    DATA_API void setReferencedSegmentNumber(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        std::vector<std::uint16_t> _referenced_segment_number
    );
    /// @}

    /**
     * Getter/Setter for the GraphicData (0070,0022) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @param graphicCoordinatesDataNumber The 0-indexed index of the graphic coordinate data whose data must be fetched
     * @{
     */
    DATA_API std::vector<Point2> getGraphicData(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number
    ) const noexcept;
    DATA_API void setGraphicData(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        std::size_t _graphic_coordinates_data_number,
        const std::vector<Point2>& _graphic_data
    );
    /// @}

    /**
     * Getter/Setter for the FiducialUID (0070,031A) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    DATA_API std::optional<std::string> getFiducialUID(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    DATA_API void setFiducialUID(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::optional<std::string>& _fiducial_uid
    );
    /// @}

    /**
     * Getter/Setter for the ContourData (3006,0050) data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @param fiducialNumber The 0-indexed index of the fiducial whose data must be fetched.
     * @{
     */
    DATA_API std::vector<Point3> getContourData(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number
    ) const noexcept;
    DATA_API void setContourData(
        std::size_t _fiducial_set_number,
        std::size_t _fiducial_number,
        const std::vector<Point3>& _contour_data
    );
    /// @}

    /** Getter/Setter for the GroupName private tag data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    DATA_API std::optional<std::string> getGroupName(std::size_t _fiducial_set_number) const noexcept;
    DATA_API void setGroupName(std::size_t _fiducial_set_number, const std::string& _group_name);
    /// @}

    /**
     * Getter/Setter for the Color private tag data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    DATA_API std::optional<std::array<float, 4> > getColor(std::size_t _fiducial_set_number) const noexcept;
    DATA_API void setColor(std::size_t _fiducial_set_number, const std::array<float, 4>& _color);
    /// @}

    /**
     * Getter/Setter for the Size private tag data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    DATA_API std::optional<float> getSize(std::size_t _fiducial_set_number) const noexcept;
    DATA_API void setSize(std::size_t _fiducial_set_number, float _size);
    /// @}

    /**
     * Getter/Setter for the Shape private tag data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    DATA_API std::optional<PrivateShape> getShape(std::size_t _fiducial_set_number) const noexcept;
    DATA_API void setShape(std::size_t _fiducial_set_number, PrivateShape _shape);
    /// @}

    /**
     * Getter/Setter for the Visibility private tag data element.
     * @param fiducialSetNumber The 0-indexed index of the fiducial set whose data must be fetched.
     * @{
     */
    DATA_API std::optional<bool> getVisibility(std::size_t _fiducial_set_number) const noexcept;
    DATA_API void setVisibility(std::size_t _fiducial_set_number, bool _visibility);
    /// @}

    // Helper methods

    /**
     * Set group names for fiducial sets which contains point fiducials which doesn't have group names
     */
    DATA_API void setGroupNamesForPointFiducials();

    /**
     * Get the list of group names for fiducial sets which contains point fiducials
     * @return The list of group names
     */
    [[nodiscard]] DATA_API std::vector<std::string> getPointFiducialsGroupNames() const;

    /**
     * Get the fiducial set which has the name in parameter and its index
     * @param groupName The name of the group to fetch
     * @return A pair with the fiducial set and its index, or std::nullopt if the group name doesn't exist
     */
    [[nodiscard]] DATA_API std::optional<std::pair<FiducialSet, std::size_t> > getFiducialSetAndIndex(
        const std::string& _group_name
    ) const;

    /**
     * Get the number of point fiducials in a group
     * @param groupName The name of the group to fetch
     * @return The number of points in the group, or std::nullopt if the group name doesn't exist
     */
    [[nodiscard]] DATA_API std::optional<std::size_t> getNumberOfPointsInGroup(const std::string& _group_name) const;

    /**
     * Returns the 3D position of the point fiducial using Contour Data.
     * @param fiducial The fiducial whose 3D position must be found
     * @return The 3D position of the fiducial, or std::nullopt if it has no Contour Data (getting 3D position using
     * Graphic Coordinates Data Sequence isn't supported) or if its shape type isn't point.
     */
    [[nodiscard]] DATA_API static std::optional<std::array<double, 3> > getPoint(const Fiducial& _fiducial);

    /**
     * Returns the 3D position of the point INDEX in group GROUP_NAME
     * @param groupName The name of the group of the point
     * @param index The index of the point in its group
     * @return The 3D position of the fiducial, or std::nullopt if the group doesn't exist or if it has no Contour Data
     * (getting 3D position using Graphic Coordinates Data Sequence isn't supported) or if its shape type isn't point.
     */
    [[nodiscard]] DATA_API std::optional<std::array<double, 3> > getPoint(
        const std::string& _group_name,
        std::size_t _index
    ) const;

    /**
     * Get all the fiducials whose shape type is point in the given fiducial set
     * @param fiducialSet The fiducial set whose fiducials must be filtered
     * @return The list of fiducial whose shape type is point
     */
    [[nodiscard]] DATA_API static std::vector<Fiducial> getPointFiducials(const FiducialSet& _fiducial_set);

    /**
     * Get a fiducial set as a structure compatible with data::landmarks
     * @param groupName The name of the group to fetch
     * @return The fiducial set as a structure compatible with data::landmarks
     */
    [[nodiscard]] DATA_API std::optional<landmarks::LandmarksGroup> getGroup(const std::string& _group_name) const;

    /**
     * Remove the point INDEX in group GROUP_NAME
     * @param groupName The name of the group of the point
     * @param index The index of the point in its group
     */
    DATA_API void removePoint(const std::string& _group_name, std::size_t _index);

    /**
     * Remove the group GROUP_NAME
     * @param groupName The name of the group to be removed
     */
    DATA_API void removeGroup(const std::string& _group);

    /**
     * Add a new fiducial set with the given parameters
     * @param groupName The name of the new fiducial set
     * @param color The color of the new fiducial set
     * @param size The size of the points in the new fiducial set
     */
    DATA_API void addGroup(const std::string& _group_name, const std::array<float, 4>& _color, float _size);

    /**
     * Add a point fiducial in a fiducial set
     * @param groupName The name of the group of the point
     * @param point The 3D position of the new point
     */
    DATA_API void addPoint(const std::string& _group_name, const std::array<double, 3>& _pos);

private:

    static Shape stringToShape(const std::optional<std::string>& _string);
    static std::optional<std::array<float, 4> > stringToColor(const std::optional<std::string>& _string);
    static std::optional<PrivateShape> stringToPrivateShape(const std::optional<std::string>& _string);

    template<typename T>
    T to(const gdcm::DataSet& _data_set) const;

    FiducialSet toFiducialSet(const gdcm::DataSet& _data_set) const;
    ReferencedImage toReferencedImage(const gdcm::DataSet& _data_set) const;
    Fiducial toFiducial(const gdcm::DataSet& _data_set) const;
    GraphicCoordinatesData toGraphicCoordinatesData(const gdcm::DataSet& _data_set) const;

    static std::vector<Point2> toPoint2(const std::vector<float>& _floats);

    static std::vector<Point3> toPoint3(const std::vector<double>& _floats);

    template<typename T>
    std::optional<std::vector<T> > toVector(gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence) const;

    template<typename T>
    gdcm::SmartPointer<gdcm::SequenceOfItems> toSequence(const std::optional<std::vector<T> >& _vector) const;
    template<typename T>
    gdcm::SmartPointer<gdcm::SequenceOfItems> toSequence(const std::vector<T>& _vector) const;
};

} // namespace sight::data
