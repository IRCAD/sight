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

#include "fiducials_series.hpp"

#include "data/dicom/Sop.hpp"

#include "detail/SeriesImpl.hxx"

#include <core/tools/uuid.hpp>

#include <gdcmSequenceOfItems.h>
#include <gdcmSmartPointer.h>
#include <gdcmTagKeywords.h>

#include <array>

namespace sight::data
{

namespace kw = gdcm::Keywords;

//------------------------------------------------------------------------------

namespace
{

//------------------------------------------------------------------------------

std::string shape_to_string(fiducials_series::Shape _shape)
{
    switch(_shape)
    {
        case fiducials_series::Shape::POINT:
            return "POINT";

        case fiducials_series::Shape::LINE:
            return "LINE";

        case fiducials_series::Shape::PLANE:
            return "PLANE";

        case fiducials_series::Shape::SURFACE:
            return "SURFACE";

        case fiducials_series::Shape::RULER:
            return "RULER";

        case fiducials_series::Shape::L_SHAPE:
            return "L_SHAPE";

        case fiducials_series::Shape::T_SHAPE:
            return "T_SHAPE";

        case fiducials_series::Shape::SHAPE:
            return "SHAPE";

        default:
            SIGHT_WARN("Unknown shape " << static_cast<int>(_shape));
            return "";
    }
}

//------------------------------------------------------------------------------

std::optional<std::string> color_to_string(const std::optional<std::array<float, 4> >& _color)
{
    if(!_color.has_value())
    {
        return std::nullopt;
    }

    return std::to_string((*_color)[0]) + ',' + std::to_string((*_color)[1]) + ',' + std::to_string((*_color)[2]) + ','
           + std::to_string((*_color)[3]);
}

//------------------------------------------------------------------------------

std::optional<std::string> private_shape_to_string(const std::optional<fiducials_series::PrivateShape>& _private_shape)
{
    if(!_private_shape.has_value())
    {
        return std::nullopt;
    }

    switch(*_private_shape)
    {
        case fiducials_series::PrivateShape::SPHERE:
            return "SPHERE";

        case fiducials_series::PrivateShape::CUBE:
            return "CUBE";

        default:
            return std::nullopt;
    }
}

//------------------------------------------------------------------------------

std::vector<float> to_floats(const std::vector<fiducials_series::Point2>& _points)
{
    std::vector<float> res;
    res.reserve(_points.size() * 2);
    for(const fiducials_series::Point2& point : _points)
    {
        res.push_back(static_cast<float>(point.x));
        res.push_back(static_cast<float>(point.y));
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<double> to_floats(const std::vector<fiducials_series::Point3>& _points)
{
    std::vector<double> res;
    res.reserve(_points.size() * 3);
    for(const fiducials_series::Point3& point : _points)
    {
        res.push_back(point.x);
        res.push_back(point.y);
        res.push_back(point.z);
    }

    return res;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::SeriesImpl& _pimpl, fiducials_series::ReferencedImage _referenced_image)
{
    gdcm::DataSet data_set;
    _pimpl.setStringValue<kw::ReferencedSOPClassUID>(_referenced_image.referencedSOPClassUID, data_set);
    _pimpl.setStringValue<kw::ReferencedSOPInstanceUID>(_referenced_image.referencedSOPInstanceUID, data_set);
    _pimpl.setValues<kw::ReferencedFrameNumber>(_referenced_image.referencedFrameNumber, data_set);
    _pimpl.setValues<kw::ReferencedSegmentNumber>(_referenced_image.referencedSegmentNumber, data_set);
    return data_set;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::SeriesImpl& _pimpl, fiducials_series::GraphicCoordinatesData _graphic_coordinates_data)
{
    gdcm::DataSet data_set;
    auto referenced_image_sequence = gdcm::SequenceOfItems::New();
    gdcm::Item item;
    item.SetNestedDataSet(to_gdcm(_pimpl, _graphic_coordinates_data.referencedImageSequence));
    referenced_image_sequence->AddItem(item);
    detail::SeriesImpl::setSequence(kw::ReferencedImageSequence::GetTag(), referenced_image_sequence, data_set);
    _pimpl.setValues<kw::GraphicData>(to_floats(_graphic_coordinates_data.graphicData), data_set);
    return data_set;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::SeriesImpl& _pimpl, fiducials_series::Fiducial _fiducial)
{
    gdcm::DataSet data_set;
    _pimpl.setStringValue<kw::ShapeType>(shape_to_string(_fiducial.shapeType), data_set);
    _pimpl.setStringValue<kw::FiducialDescription>(_fiducial.fiducialDescription, data_set);
    _pimpl.setStringValue<kw::FiducialIdentifier>(_fiducial.fiducialIdentifier, data_set);
    if(_fiducial.graphicCoordinatesDataSequence)
    {
        auto gcds = gdcm::SequenceOfItems::New();
        for(const fiducials_series::GraphicCoordinatesData& gcd : *_fiducial.graphicCoordinatesDataSequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(to_gdcm(_pimpl, gcd));
            gcds->AddItem(item);
        }

        detail::SeriesImpl::setSequence(kw::GraphicCoordinatesDataSequence::GetTag(), gcds, data_set);
    }

    _pimpl.setStringValue<kw::FiducialUID>(_fiducial.fiducialUID.value_or(""), data_set);
    if(!_fiducial.contourData.empty())
    {
        _pimpl.setValue<kw::NumberOfContourPoints>(static_cast<int>(_fiducial.contourData.size()), data_set);
        _pimpl.setValues<kw::ContourData>(to_floats(_fiducial.contourData), data_set);
    }

    return data_set;
}

//------------------------------------------------------------------------------

gdcm::DataSet to_gdcm(detail::SeriesImpl& _pimpl, fiducials_series::FiducialSet _fiducial_set)
{
    gdcm::DataSet data_set;
    if(_fiducial_set.referencedImageSequence)
    {
        auto referenced_image_sequence = gdcm::SequenceOfItems::New();
        for(const fiducials_series::ReferencedImage& referenced_image : *_fiducial_set.referencedImageSequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(to_gdcm(_pimpl, referenced_image));
            referenced_image_sequence->AddItem(item);
        }

        detail::SeriesImpl::setSequence(kw::ReferencedImageSequence::GetTag(), referenced_image_sequence, data_set);
    }

    if(_fiducial_set.frameOfReferenceUID)
    {
        _pimpl.setStringValue<kw::FrameOfReferenceUID>(*_fiducial_set.frameOfReferenceUID, data_set);
    }

    auto fiducial_sequence = gdcm::SequenceOfItems::New();
    for(const fiducials_series::Fiducial& fiducial : _fiducial_set.fiducialSequence)
    {
        gdcm::Item item;
        item.SetNestedDataSet(to_gdcm(_pimpl, fiducial));
        fiducial_sequence->AddItem(item);
    }

    detail::SeriesImpl::setSequence(kw::FiducialSequence::GetTag(), fiducial_sequence, data_set);
    detail::SeriesImpl::setPrivateValue(0, _fiducial_set.groupName, data_set);
    detail::SeriesImpl::setPrivateValue(1, color_to_string(_fiducial_set.color), data_set);
    std::optional<std::string> size;
    if(_fiducial_set.size.has_value())
    {
        size = std::to_string(*_fiducial_set.size);
    }

    detail::SeriesImpl::setPrivateValue(2, size, data_set);
    detail::SeriesImpl::setPrivateValue(3, private_shape_to_string(_fiducial_set.shape), data_set);
    if(_fiducial_set.visibility.has_value())
    {
        detail::SeriesImpl::setPrivateValue(4, *_fiducial_set.visibility ? "true" : "false", data_set);
    }

    return data_set;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> append_in_sequence(
    detail::SeriesImpl& _pimpl,
    gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence,
    T _element
)
{
    auto res = _sequence == nullptr ? gdcm::SequenceOfItems::New() : _sequence;
    gdcm::Item item;
    item.SetNestedDataSet(to_gdcm(_pimpl, _element));
    res->AddItem(item);
    return res;
}

//------------------------------------------------------------------------------

template<typename T>
void append_in_sequence(
    detail::SeriesImpl& _pimpl,
    gdcm::Tag _tag,
    T _element,
    const std::vector<std::pair<gdcm::Tag, std::size_t> >& _indices = {})
{
    _pimpl.setSequence(_tag, append_in_sequence(_pimpl, _pimpl.getSequence(_tag, 0, _indices), _element));
}

} // namespace

//------------------------------------------------------------------------------

void fiducials_series::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const fiducials_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void fiducials_series::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const fiducials_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool fiducials_series::Point2::operator==(Point2 _other) const
{
    return x == _other.x && y == _other.y;
}

//------------------------------------------------------------------------------

bool fiducials_series::Point3::operator==(Point3 _other) const
{
    return x == _other.x && y == _other.y && z == _other.z;
}

//------------------------------------------------------------------------------

bool fiducials_series::ReferencedImage::operator==(const ReferencedImage& _other) const
{
    return referencedSOPClassUID == _other.referencedSOPClassUID
           && referencedSOPInstanceUID == _other.referencedSOPInstanceUID
           && referencedFrameNumber == _other.referencedFrameNumber
           && referencedSegmentNumber == _other.referencedSegmentNumber;
}

//------------------------------------------------------------------------------

bool fiducials_series::ReferencedImage::operator!=(const ReferencedImage& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool fiducials_series::GraphicCoordinatesData::operator==(const GraphicCoordinatesData& _other) const
{
    return referencedImageSequence == _other.referencedImageSequence && graphicData == _other.graphicData;
}

//------------------------------------------------------------------------------

bool fiducials_series::GraphicCoordinatesData::operator!=(const GraphicCoordinatesData& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool fiducials_series::Fiducial::operator==(const Fiducial& _other) const
{
    return shapeType == _other.shapeType && fiducialDescription == _other.fiducialDescription
           && fiducialIdentifier == _other.fiducialIdentifier
           && graphicCoordinatesDataSequence == _other.graphicCoordinatesDataSequence
           && fiducialUID == _other.fiducialUID
           && contourData == _other.contourData;
}

//------------------------------------------------------------------------------

bool fiducials_series::Fiducial::operator!=(const Fiducial& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool fiducials_series::FiducialSet::operator==(const FiducialSet& _other) const
{
    return referencedImageSequence == _other.referencedImageSequence
           && frameOfReferenceUID == _other.frameOfReferenceUID
           && fiducialSequence == _other.fiducialSequence && groupName == _other.groupName;
}

fiducials_series::fiducials_series()
{
    setSOPKeyword(dicom::sop::Keyword::SpatialFiducialsStorage);
}

//------------------------------------------------------------------------------

bool fiducials_series::operator==(const fiducials_series& _other) const
{
    return getContentDate() == _other.getContentDate() && getContentLabel() == _other.getContentLabel()
           && getContentDescription() == _other.getContentDescription()
           && getContentCreatorName() == _other.getContentCreatorName() && getFiducialSets() == _other.getFiducialSets()
           && base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool fiducials_series::operator!=(const fiducials_series& _other) const
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentDate() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentDate>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentDate(const std::string& _content_date)
{
    m_pimpl->setStringValue<kw::ContentDate>(_content_date);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentLabel() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentLabel>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentLabel(const std::string& _content_label)
{
    m_pimpl->setStringValue<kw::ContentLabel>(_content_label);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentDescription() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentDescription>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentDescription(const std::string& _content_description)
{
    m_pimpl->setStringValue<kw::ContentDescription>(_content_description);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentCreatorName() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentCreatorName>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentCreatorName(const std::string& _content_creator_name)
{
    m_pimpl->setStringValue<kw::ContentCreatorName>(_content_creator_name);
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::FiducialSet> fiducials_series::getFiducialSets() const noexcept
{
    return toVector<FiducialSet>(m_pimpl->getSequence<kw::FiducialSetSequence>()).value_or(std::vector<FiducialSet> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialSets(const std::vector<FiducialSet>& _fiducial_sets)
{
    m_pimpl->setSequence(kw::FiducialSetSequence::GetTag(), toSequence(_fiducial_sets));
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialSet(std::size_t _fiducial_set_number, FiducialSet _fiducial_set)
{
    m_pimpl->getOrCreateDataSet(0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}}) = to_gdcm(
        *m_pimpl,
        _fiducial_set
    );
}

//------------------------------------------------------------------------------

void fiducials_series::appendFiducialSet(FiducialSet _fiducial_set)
{
    append_in_sequence(*m_pimpl, kw::FiducialSetSequence::GetTag(), _fiducial_set);
}

//------------------------------------------------------------------------------

std::optional<std::vector<fiducials_series::ReferencedImage> > fiducials_series::getReferencedImages(
    std::size_t _fiducial_set_number
) const noexcept
{
    return toVector<ReferencedImage>(
        m_pimpl->getSequence(
            kw::ReferencedImageSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedImages(
    std::size_t _fiducial_set_number,
    const std::optional<std::vector<ReferencedImage> >& _referenced_images
)
{
    m_pimpl->setSequence(
        kw::ReferencedImageSequence::GetTag(),
        toSequence(
            _referenced_images
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedImage(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    ReferencedImage _referenced_image
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        }) = to_gdcm(*m_pimpl, _referenced_image);
}

//------------------------------------------------------------------------------

void fiducials_series::appendReferencedImage(std::size_t _fiducial_set_number, ReferencedImage _referenced_image)
{
    append_in_sequence(
        *m_pimpl,
        kw::ReferencedImageSequence::GetTag(),
        _referenced_image,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getReferencedSOPClassUID(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPClassUID(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    const std::string& _referenced_sop_class_uid
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPClassUID>(
        _referenced_sop_class_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getReferencedSOPInstanceUID(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPInstanceUID(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    const std::string& _referenced_sop_instance_uid
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPInstanceUID>(
        _referenced_sop_instance_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> fiducials_series::getReferencedFrameNumber(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedFrameNumber(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    std::vector<std::int32_t> _referenced_frame_number
)
{
    m_pimpl->setValues<kw::ReferencedFrameNumber>(
        _referenced_frame_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> fiducials_series::getReferencedSegmentNumber(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSegmentNumber(
    std::size_t _fiducial_set_number,
    std::size_t _referenced_image_number,
    std::vector<std::uint16_t> _referenced_segment_number
)
{
    m_pimpl->setValues<kw::ReferencedSegmentNumber>(
        _referenced_segment_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::ReferencedImageSequence::GetTag(), _referenced_image_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getFrameOfReferenceUID(std::size_t _fiducial_set_number) const noexcept
{
    return m_pimpl->getValue<kw::FrameOfReferenceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setFrameOfReferenceUID(
    std::size_t _fiducial_set_number,
    const std::optional<std::string>& _frame_of_reference_uid
)
{
    m_pimpl->setStringValue<kw::FrameOfReferenceUID>(
        _frame_of_reference_uid.value_or(
            ""
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Fiducial> fiducials_series::getFiducials(std::size_t _fiducial_set_number) const noexcept
{
    return toVector<Fiducial>(
        m_pimpl->getSequence(
            kw::FiducialSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }
            })
    ).value_or(std::vector<Fiducial> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducials(std::size_t _fiducial_set_number, const std::vector<Fiducial>& _fiducials)
{
    m_pimpl->setSequence(
        kw::FiducialSequence::GetTag(),
        toSequence(_fiducials),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducial(std::size_t _fiducial_set_number, std::size_t _fiducial_number, Fiducial _fiducial)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        }) = to_gdcm(*m_pimpl, _fiducial);
}

//------------------------------------------------------------------------------

void fiducials_series::appendFiducial(std::size_t _fiducial_set_number, Fiducial _fiducial)
{
    append_in_sequence(
        *m_pimpl,
        kw::FiducialSequence::GetTag(),
        _fiducial,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

fiducials_series::Shape fiducials_series::get_shape_type(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return stringToShape(
        m_pimpl->getValue<kw::ShapeType>(
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }, {kw::FiducialSequence::GetTag(), _fiducial_number
                }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::set_shape_type(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    fiducials_series::Shape _shape_type
)
{
    m_pimpl->setValue<kw::ShapeType>(
        shape_to_string(_shape_type),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::getFiducialDescription(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialDescription>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialDescription(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::string& _fiducial_description
)
{
    m_pimpl->setStringValue<kw::FiducialDescription>(
        _fiducial_description,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::getFiducialIdentifier(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialIdentifier>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialIdentifier(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::string& _fiducial_identifier
)
{
    m_pimpl->setStringValue<kw::FiducialIdentifier>(
        _fiducial_identifier,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::vector<fiducials_series::GraphicCoordinatesData> > fiducials_series::
getGraphicCoordinatesDataSequence(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return toVector<GraphicCoordinatesData>(
        m_pimpl->getSequence(
            kw::GraphicCoordinatesDataSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
            }, {kw::FiducialSequence::GetTag(), _fiducial_number
                }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setGraphicCoordinatesDataSequence(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::optional<std::vector<GraphicCoordinatesData> >& _graphic_coordinates_data_sequence
)
{
    m_pimpl->setSequence(
        kw::GraphicCoordinatesDataSequence::GetTag(),
        toSequence(
            _graphic_coordinates_data_sequence
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setGraphicCoordinatesData(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    GraphicCoordinatesData _graphic_coordinates_data
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }
        }) = to_gdcm(*m_pimpl, _graphic_coordinates_data);
}

//------------------------------------------------------------------------------

void fiducials_series::appendGraphicCoordinatesData(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    GraphicCoordinatesData _graphic_coordinates_data
)
{
    append_in_sequence(
        *m_pimpl,
        kw::GraphicCoordinatesDataSequence::GetTag(),
        _graphic_coordinates_data,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

fiducials_series::ReferencedImage fiducials_series::getReferencedImage(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    std::optional<gdcm::DataSet> data_set = m_pimpl->getDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
    if(!data_set)
    {
        return {};
    }

    return toReferencedImage(*data_set);
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedImage(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    ReferencedImage _referenced_image
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }) = to_gdcm(*m_pimpl, _referenced_image);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getReferencedSOPClassUID(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPClassUID(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    const std::string& _referenced_sop_class_uid
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPClassUID>(
        _referenced_sop_class_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::getReferencedSOPInstanceUID(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPInstanceUID(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    const std::string& _referenced_sop_instance_uid
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPInstanceUID>(
        _referenced_sop_instance_uid,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> fiducials_series::getReferencedFrameNumber(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedFrameNumber(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    std::vector<std::int32_t> _referenced_frame_number
)
{
    m_pimpl->setValues<kw::ReferencedFrameNumber>(
        _referenced_frame_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> fiducials_series::getReferencedSegmentNumber(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSegmentNumber(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    std::vector<std::uint16_t> _referenced_segment_number
)
{
    m_pimpl->setValues<kw::ReferencedSegmentNumber>(
        _referenced_segment_number,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point2> fiducials_series::getGraphicData(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number
) const noexcept
{
    std::optional<std::vector<float> > graphic_data = m_pimpl->getValues<kw::GraphicData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }
        });
    if(!graphic_data)
    {
        return {};
    }

    return toPoint2(*graphic_data);
}

//------------------------------------------------------------------------------

void fiducials_series::setGraphicData(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    std::size_t _graphic_coordinates_data_number,
    const std::vector<fiducials_series::Point2>& _graphic_data
)
{
    m_pimpl->setValues<kw::GraphicData>(
        to_floats(_graphic_data),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), _graphic_coordinates_data_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getFiducialUID(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialUID(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::optional<std::string>& _fiducial_uid
)
{
    m_pimpl->setStringValue<kw::FiducialUID>(
        _fiducial_uid.value_or(""),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point3> fiducials_series::getContourData(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number
) const noexcept
{
    std::optional<std::vector<double> > contour_data = m_pimpl->getValues<kw::ContourData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        },
            {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
    if(!contour_data)
    {
        return {};
    }

    return toPoint3(*contour_data);
}

//------------------------------------------------------------------------------

void fiducials_series::setContourData(
    std::size_t _fiducial_set_number,
    std::size_t _fiducial_number,
    const std::vector<fiducials_series::Point3>& _contour_data
)
{
    m_pimpl->setValues<kw::ContourData>(
        to_floats(_contour_data),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }, {kw::FiducialSequence::GetTag(), _fiducial_number
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getGroupName(std::size_t _fiducial_set_number) const noexcept
{
    return m_pimpl->getPrivateValue(0, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}});
}

//------------------------------------------------------------------------------

void fiducials_series::setGroupName(std::size_t _fiducial_set_number, const std::string& _group_name)
{
    m_pimpl->setPrivateValue(0, _group_name, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}});
}

//------------------------------------------------------------------------------

std::optional<std::array<float, 4> > fiducials_series::getColor(std::size_t _fiducial_set_number) const noexcept
{
    return stringToColor(m_pimpl->getPrivateValue(1, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}}));
}

//------------------------------------------------------------------------------

void fiducials_series::setColor(std::size_t _fiducial_set_number, const std::array<float, 4>& _color)
{
    m_pimpl->setPrivateValue(
        1,
        color_to_string(_color),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<float> fiducials_series::getSize(std::size_t _fiducial_set_number) const noexcept
{
    std::optional<std::string> private_value = m_pimpl->getPrivateValue(
        2,
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
    if(!private_value.has_value())
    {
        return std::nullopt;
    }

    return std::stof(*private_value);
}

//------------------------------------------------------------------------------

void fiducials_series::setSize(std::size_t _fiducial_set_number, float _size)
{
    m_pimpl->setPrivateValue(2, std::to_string(_size), 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}});
}

//------------------------------------------------------------------------------

std::optional<fiducials_series::PrivateShape> fiducials_series::getShape(std::size_t _fiducial_set_number) const
noexcept
{
    return stringToPrivateShape(
        m_pimpl->getPrivateValue(3, 0, {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number}})
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setShape(std::size_t _fiducial_set_number, PrivateShape _shape)
{
    m_pimpl->setPrivateValue(
        3,
        private_shape_to_string(_shape),
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

std::optional<bool> fiducials_series::getVisibility(std::size_t _fiducial_set_number) const noexcept
{
    if(std::optional<std::string> visibility = m_pimpl->getPrivateValue(
           4,
           0,
           {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
           }
           }))
    {
        return *visibility == "true";
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void fiducials_series::setVisibility(std::size_t _fiducial_set_number, bool _visibility)
{
    m_pimpl->setPrivateValue(
        4,
        _visibility ? "true" : "false",
        0,
        {{kw::FiducialSetSequence::GetTag(), _fiducial_set_number
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setGroupNamesForPointFiducials()
{
    std::vector<FiducialSet> fiducial_sets = getFiducialSets();
    for(FiducialSet& fiducial_set : fiducial_sets)
    {
        if(fiducial_set.groupName.has_value() && !fiducial_set.groupName->empty())
        {
            continue;
        }

        bool contains_a_point_fiducial = false;
        for(const Fiducial& fiducial : fiducial_set.fiducialSequence)
        {
            if(fiducial.shapeType == Shape::POINT)
            {
                contains_a_point_fiducial = true;
                break;
            }
        }

        if(contains_a_point_fiducial)
        {
            std::size_t i          = 0;
            std::string group_name = "Group_" + std::to_string(i);
            while(std::ranges::find_if(
                      fiducial_sets,
                      [&group_name](const FiducialSet& _fiducial_set)
                {
                    return _fiducial_set.groupName.value_or("") == group_name;
                }) != fiducial_sets.end())
            {
                i++;
                group_name = "Group_" + std::to_string(i);
            }

            fiducial_set.groupName = group_name;
        }
    }

    setFiducialSets(fiducial_sets);
}

//------------------------------------------------------------------------------

std::vector<std::string> fiducials_series::getPointFiducialsGroupNames() const
{
    std::vector<std::string> res;
    std::vector<FiducialSet> fiducial_sets = getFiducialSets();
    res.reserve(fiducial_sets.size());
    // Ignore fiducial sets which doesn't contain fiducials with shape_t == POINT (= Landmarks)
    std::vector<FiducialSet> point_fiducial_sets;
    std::ranges::copy_if(
        fiducial_sets,
        std::back_inserter(point_fiducial_sets),
        [](const FiducialSet& _fiducial_set)
        {
            return _fiducial_set.fiducialSequence.empty() || std::ranges::find_if(
                _fiducial_set.fiducialSequence,
                [](
                    const Fiducial& _fiducial){return _fiducial.shapeType == Shape::POINT;}) != _fiducial_set.fiducialSequence.end();
        });
    std::ranges::for_each(
        point_fiducial_sets,
        [&res](const FiducialSet& _fs)
        {
            if(_fs.groupName.has_value())
            {
                res.push_back(*_fs.groupName);
            }
        });
    return res;
}

//------------------------------------------------------------------------------

std::optional<std::pair<fiducials_series::FiducialSet, std::size_t> > fiducials_series::getFiducialSetAndIndex(
    const std::string& _group_name
) const
{
    for(std::size_t index = 0 ;
        const FiducialSet& fiducial_set : getFiducialSets())
    {
        if(fiducial_set.groupName == _group_name)
        {
            return {{fiducial_set, index}};
        }

        index++;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

std::optional<std::size_t> fiducials_series::getNumberOfPointsInGroup(const std::string& _group_name) const
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducial_set = getFiducialSetAndIndex(_group_name);
    if(!fiducial_set.has_value())
    {
        SIGHT_ASSERT("The group name '" << _group_name << "' doesn't exist", false);
        return {};
    }

    return std::size_t(std::ranges::count_if(
                           fiducial_set->first.fiducialSequence,
                           [](const Fiducial& _fiducial)
        {
            return _fiducial.shapeType == Shape::POINT;
        }));
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<fiducials_series::Fiducial> fiducials_series::getPointFiducials(
    const fiducials_series::FiducialSet& _fiducial_set
)
{
    std::vector<Fiducial> point_fiducials;
    std::ranges::copy_if(
        _fiducial_set.fiducialSequence,
        std::back_inserter(point_fiducials),
        [](const auto _f)
        {
            return _f.shapeType == Shape::POINT;
        });
    return point_fiducials;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > fiducials_series::getPoint(
    const fiducials_series::Fiducial& _fiducial
)
{
    if(_fiducial.shapeType != Shape::POINT)
    {
        // Only point fiducials are supported
        return std::nullopt;
    }

    if(!_fiducial.contourData.empty())
    {
        data::fiducials_series::Point3 point = _fiducial.contourData[0];
        return {{point.x, point.y, point.z}};
    }

    // Position with Graphic Coordinates Data Sequence isn't supported
    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > fiducials_series::getPoint(
    const std::string& _group_name,
    std::size_t _index
) const
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducial_set = getFiducialSetAndIndex(_group_name);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    for(std::size_t i = 0 ; const Fiducial& fiducial : fiducial_set->first.fiducialSequence)
    {
        if(fiducial.shapeType == Shape::POINT)
        {
            if(i == _index)
            {
                return getPoint(fiducial);
            }

            i++;
        }
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<landmarks::LandmarksGroup> fiducials_series::getGroup(const std::string& _group_name) const
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducial_set = getFiducialSetAndIndex(_group_name);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    landmarks::color_t color = fiducial_set->first.color.value_or(
        std::array {1.F, 1.F, 1.F, 1.F
        });
    landmarks::size_t size                                = fiducial_set->first.size.value_or(10);
    fiducials_series::PrivateShape fiducial_private_shape = fiducial_set->first.shape.value_or(PrivateShape::SPHERE);
    landmarks::Shape shape                                = landmarks::Shape::SPHERE;
    switch(fiducial_private_shape)
    {
        case PrivateShape::SPHERE:
            shape = landmarks::Shape::SPHERE;
            break;

        case PrivateShape::CUBE:
            shape = landmarks::Shape::CUBE;

        default:
            break;
    }

    bool visibility = fiducial_set->first.visibility.value_or(true);

    landmarks::LandmarksGroup group(color, size, shape, visibility);
    std::ranges::for_each(
        getPointFiducials(fiducial_set->first),
        [&group](const data::fiducials_series::Fiducial& _fiducial)
        {
            if(auto point = getPoint(_fiducial))
            {
                group.m_points.push_back(*point);
            }
        });
    return group;
}

//------------------------------------------------------------------------------

void fiducials_series::removePoint(const std::string& _group_name, std::size_t _index)
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducial_set = getFiducialSetAndIndex(_group_name);
    if(!fiducial_set.has_value())
    {
        return;
    }

    std::size_t point_index = 0;
    for(auto it = fiducial_set->first.fiducialSequence.begin() ;
        it < fiducial_set->first.fiducialSequence.end() ;
        ++it)
    {
        if(it->shapeType == data::fiducials_series::Shape::POINT)
        {
            if(point_index == _index)
            {
                fiducial_set->first.fiducialSequence.erase(it);
                break;
            }

            point_index++;
        }
    }

    setFiducialSet(fiducial_set->second, fiducial_set->first);
}

//------------------------------------------------------------------------------

void fiducials_series::removeGroup(const std::string& _group_name)
{
    std::vector<FiducialSet> fiducial_sets = getFiducialSets();
    std::erase_if(fiducial_sets, [&_group_name](const FiducialSet& _fs){return _fs.groupName == _group_name;});
    setFiducialSets(fiducial_sets);
}

//------------------------------------------------------------------------------

void fiducials_series::addGroup(const std::string& _group_name, const std::array<float, 4>& _color, float _size)
{
    FiducialSet fiducial_set;
    fiducial_set.groupName = _group_name;
    fiducial_set.color     = _color;
    fiducial_set.size      = _size;
    fiducial_set.shape     = PrivateShape::SPHERE;
    appendFiducialSet(fiducial_set);
}

//------------------------------------------------------------------------------

void fiducials_series::addPoint(const std::string& _group_name, const std::array<double, 3>& _pos)
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducial_set = getFiducialSetAndIndex(_group_name);
    if(!fiducial_set.has_value())
    {
        SIGHT_WARN("Couldn't add point in fiducial set '" << _group_name << "', the group doesn't exist.");
        return;
    }

    std::string fiducial_name = _group_name + '_' + std::to_string(getPointFiducials(fiducial_set->first).size());
    Fiducial fiducial;
    fiducial.shapeType           = Shape::POINT;
    fiducial.fiducialDescription = fiducial_name;
    fiducial.fiducialIdentifier  = fiducial_name;
    fiducial.fiducialUID         = core::tools::UUID::generate();
    fiducial.contourData         = {{.x = _pos[0], .y = _pos[1], .z = _pos[2]}};
    appendFiducial(fiducial_set->second, fiducial);
}

//------------------------------------------------------------------------------

fiducials_series::Shape fiducials_series::stringToShape(const std::optional<std::string>& _string)
{
    static const std::map<std::string, Shape> stringToShapeMap {{"POINT", Shape::POINT
    }, {"LINE", Shape::LINE
        }, {"PLANE", Shape::POINT
        }, {"SURFACE", Shape::SURFACE
        }, {"RULER", Shape::RULER
        }, {"L_SHAPE", Shape::L_SHAPE
        }, {"T_SHAPE", Shape::T_SHAPE
        }, {"SHAPE", Shape::SHAPE
        }
    };
    if(!_string || !stringToShapeMap.contains(*_string))
    {
        return Shape::INVALID;
    }

    return stringToShapeMap.at(*_string);
}

//------------------------------------------------------------------------------

std::optional<std::array<float, 4> > fiducials_series::stringToColor(const std::optional<std::string>& _string)
{
    if(!_string.has_value())
    {
        return std::nullopt;
    }

    std::vector<std::string> splits;
    boost::split(splits, *_string, boost::is_any_of(","));
    if(splits.size() != 4)
    {
        return std::nullopt;
    }

    std::array<float, 4> res {};
    std::ranges::transform(splits, res.begin(), [](const std::string& _value){return std::stof(_value);});
    return res;
}

//------------------------------------------------------------------------------

std::optional<fiducials_series::PrivateShape> fiducials_series::stringToPrivateShape(
    const std::optional<std::string>& _string
)
{
    static const std::map<std::string, PrivateShape> stringToPrivateShapeMap {
        {"SPHERE", PrivateShape::SPHERE},
        {"CUBE", PrivateShape::CUBE}
    };
    if(!_string.has_value())
    {
        return std::nullopt;
    }

    if(!stringToPrivateShapeMap.contains(*_string))
    {
        return std::nullopt;
    }

    return stringToPrivateShapeMap.at(*_string);
}

//------------------------------------------------------------------------------

fiducials_series::FiducialSet fiducials_series::toFiducialSet(const gdcm::DataSet& _data_set) const
{
    FiducialSet fiducial_set;
    if(auto ris = m_pimpl->getSequence(kw::ReferencedImageSequence::GetTag(), _data_set))
    {
        fiducial_set.referencedImageSequence = std::vector<ReferencedImage> {};
        for(std::size_t i = 1 ; i <= ris->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial_set.referencedImageSequence->push_back(toReferencedImage(ris->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducial_set.frameOfReferenceUID = m_pimpl->getValue<kw::FrameOfReferenceUID>(_data_set);
    if(auto fiducial_sequence = m_pimpl->getSequence(kw::FiducialSequence::GetTag(), _data_set))
    {
        for(std::size_t i = 1 ; i <= fiducial_sequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial_set.fiducialSequence.push_back(toFiducial(fiducial_sequence->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducial_set.groupName = m_pimpl->getPrivateValue(0, _data_set);
    fiducial_set.color     = stringToColor(m_pimpl->getPrivateValue(1, _data_set));
    if(std::optional<std::string> size = m_pimpl->getPrivateValue(2, _data_set))
    {
        fiducial_set.size = std::stof(*size);
    }

    fiducial_set.shape = stringToPrivateShape(m_pimpl->getPrivateValue(3, _data_set));
    if(std::optional<std::string> visibility = m_pimpl->getPrivateValue(4, _data_set))
    {
        fiducial_set.visibility = (*visibility == "true");
    }

    return fiducial_set;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::FiducialSet fiducials_series::to<fiducials_series::FiducialSet>(const gdcm::DataSet& _data_set) const
{
    return toFiducialSet(_data_set);
}

//------------------------------------------------------------------------------

fiducials_series::ReferencedImage fiducials_series::toReferencedImage(const gdcm::DataSet& _data_set) const
{
    return ReferencedImage {
        .referencedSOPClassUID    = m_pimpl->getValue<kw::ReferencedSOPClassUID>(_data_set).value_or(""),
        .referencedSOPInstanceUID = m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(_data_set).value_or(""),
        .referencedFrameNumber    = m_pimpl->getValues<kw::ReferencedFrameNumber>(_data_set).value_or(
            std::vector<std::int32_t> {
            }),
        .referencedSegmentNumber = m_pimpl->getValues<kw::ReferencedSegmentNumber>(_data_set).value_or(
            std::vector<std::uint16_t> {
            })
    };
}

//------------------------------------------------------------------------------

template<>
fiducials_series::ReferencedImage fiducials_series::to<fiducials_series::ReferencedImage>(
    const gdcm::DataSet& _data_set
)
const
{
    return toReferencedImage(_data_set);
}

//------------------------------------------------------------------------------

fiducials_series::Fiducial fiducials_series::toFiducial(const gdcm::DataSet& _data_set) const
{
    Fiducial fiducial;
    fiducial.shapeType           = stringToShape(m_pimpl->getValue<kw::ShapeType>(_data_set));
    fiducial.fiducialDescription = m_pimpl->getValue<kw::FiducialDescription>(_data_set).value_or("");
    fiducial.fiducialIdentifier  = m_pimpl->getValue<kw::FiducialIdentifier>(_data_set).value_or("");
    if(auto gcds = m_pimpl->getSequence(kw::GraphicCoordinatesDataSequence::GetTag(), _data_set))
    {
        fiducial.graphicCoordinatesDataSequence = std::vector<GraphicCoordinatesData> {};
        for(std::size_t i = 1 ; i <= gcds->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducial.graphicCoordinatesDataSequence->push_back(
                toGraphicCoordinatesData(
                    gcds->GetItem(i).
                    GetNestedDataSet()
                )
            );
        }
    }

    fiducial.fiducialUID = m_pimpl->getValue<kw::FiducialUID>(_data_set);
    if(auto contour_data = m_pimpl->getValues<kw::ContourData>(_data_set))
    {
        fiducial.contourData = toPoint3(*contour_data);
    }

    return fiducial;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::Fiducial fiducials_series::to<fiducials_series::Fiducial>(const gdcm::DataSet& _data_set) const
{
    return toFiducial(_data_set);
}

//------------------------------------------------------------------------------

fiducials_series::GraphicCoordinatesData fiducials_series::toGraphicCoordinatesData(const gdcm::DataSet& _data_set)
const
{
    GraphicCoordinatesData gcd;
    if(auto ris = m_pimpl->getSequence(kw::ReferencedImageSequence::GetTag(), _data_set);
       ris != nullptr && ris->GetNumberOfItems() > 0)
    {
        // GDCM Sequence of Items is 1-indexed
        gcd.referencedImageSequence = toReferencedImage(ris->GetItem(1).GetNestedDataSet());
    }

    if(auto graphic_data = m_pimpl->getValues<kw::GraphicData>(_data_set))
    {
        gcd.graphicData = toPoint2(*graphic_data);
    }

    return gcd;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::GraphicCoordinatesData fiducials_series::to<fiducials_series::GraphicCoordinatesData>(
    const gdcm::DataSet& _data_set
) const
{
    return toGraphicCoordinatesData(_data_set);
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point2> fiducials_series::toPoint2(const std::vector<float>& _floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 2", (_floats.size() % 2) == 0);
    std::vector<Point2> res;
    res.reserve(_floats.size() / 2);
    for(std::size_t i = 0 ; i < _floats.size() ; i += 2)
    {
        res.push_back({.x = _floats[i], .y = _floats[i + 1]});
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point3> fiducials_series::toPoint3(const std::vector<double>& _floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 3", (_floats.size() % 3) == 0);
    std::vector<Point3> res;
    res.reserve(_floats.size() / 3);
    for(std::size_t i = 0 ; i < _floats.size() ; i += 3)
    {
        res.push_back({.x = _floats[i], .y = _floats[i + 1], .z = _floats[i + 2]});
    }

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
std::optional<std::vector<T> > fiducials_series::toVector(gdcm::SmartPointer<gdcm::SequenceOfItems> sequence) const
{
    if(sequence == nullptr)
    {
        return std::nullopt;
    }

    std::vector<T> res;
    res.reserve(sequence->GetNumberOfItems());
    for(std::size_t i = 1 ; i <= sequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
    {
        res.push_back(to<T>(sequence->GetItem(i).GetNestedDataSet()));
    }

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> fiducials_series::toSequence(const std::optional<std::vector<T> >& vector)
const
{
    if(!vector)
    {
        return nullptr;
    }

    auto sequence = gdcm::SequenceOfItems::New();
    for(const T& element : *vector)
    {
        gdcm::Item item;
        item.SetNestedDataSet(to_gdcm(*m_pimpl, element));
        sequence->AddItem(item);
    }

    return sequence;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> fiducials_series::toSequence(const std::vector<T>& vector) const
{
    return toSequence(std::optional {vector});
}

} // namespace sight::data
