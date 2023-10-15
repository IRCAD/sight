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

std::string shapeToString(fiducials_series::Shape shape)
{
    switch(shape)
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
            SIGHT_WARN("Unknown shape " << static_cast<int>(shape));
            return "";
    }
}

//------------------------------------------------------------------------------

std::optional<std::string> colorToString(const std::optional<std::array<float, 4> >& color)
{
    if(!color.has_value())
    {
        return std::nullopt;
    }

    return std::to_string((*color)[0]) + ',' + std::to_string((*color)[1]) + ',' + std::to_string((*color)[2]) + ','
           + std::to_string((*color)[3]);
}

//------------------------------------------------------------------------------

std::optional<std::string> privateShapeToString(const std::optional<fiducials_series::PrivateShape>& privateShape)
{
    if(!privateShape.has_value())
    {
        return std::nullopt;
    }

    switch(*privateShape)
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

std::vector<float> toFloats(const std::vector<fiducials_series::Point2>& points)
{
    std::vector<float> res;
    res.reserve(points.size() * 2);
    for(const fiducials_series::Point2& point : points)
    {
        res.push_back(static_cast<float>(point.x));
        res.push_back(static_cast<float>(point.y));
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<double> toFloats(const std::vector<fiducials_series::Point3>& points)
{
    std::vector<double> res;
    res.reserve(points.size() * 3);
    for(const fiducials_series::Point3& point : points)
    {
        res.push_back(point.x);
        res.push_back(point.y);
        res.push_back(point.z);
    }

    return res;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, fiducials_series::ReferencedImage referencedImage)
{
    gdcm::DataSet dataSet;
    pimpl.setStringValue<kw::ReferencedSOPClassUID>(referencedImage.referencedSOPClassUID, dataSet);
    pimpl.setStringValue<kw::ReferencedSOPInstanceUID>(referencedImage.referencedSOPInstanceUID, dataSet);
    pimpl.setValues<kw::ReferencedFrameNumber>(referencedImage.referencedFrameNumber, dataSet);
    pimpl.setValues<kw::ReferencedSegmentNumber>(referencedImage.referencedSegmentNumber, dataSet);
    return dataSet;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, fiducials_series::GraphicCoordinatesData graphicCoordinatesData)
{
    gdcm::DataSet dataSet;
    auto referencedImageSequence = gdcm::SequenceOfItems::New();
    gdcm::Item item;
    item.SetNestedDataSet(toGdcm(pimpl, graphicCoordinatesData.referencedImageSequence));
    referencedImageSequence->AddItem(item);
    detail::SeriesImpl::setSequence(kw::ReferencedImageSequence::GetTag(), referencedImageSequence, dataSet);
    pimpl.setValues<kw::GraphicData>(toFloats(graphicCoordinatesData.graphicData), dataSet);
    return dataSet;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, fiducials_series::Fiducial fiducial)
{
    gdcm::DataSet dataSet;
    pimpl.setStringValue<kw::ShapeType>(shapeToString(fiducial.shapeType), dataSet);
    pimpl.setStringValue<kw::FiducialDescription>(fiducial.fiducialDescription, dataSet);
    pimpl.setStringValue<kw::FiducialIdentifier>(fiducial.fiducialIdentifier, dataSet);
    if(fiducial.graphicCoordinatesDataSequence)
    {
        auto gcds = gdcm::SequenceOfItems::New();
        for(const fiducials_series::GraphicCoordinatesData& gcd : *fiducial.graphicCoordinatesDataSequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(toGdcm(pimpl, gcd));
            gcds->AddItem(item);
        }

        detail::SeriesImpl::setSequence(kw::GraphicCoordinatesDataSequence::GetTag(), gcds, dataSet);
    }

    pimpl.setStringValue<kw::FiducialUID>(fiducial.fiducialUID.value_or(""), dataSet);
    if(!fiducial.contourData.empty())
    {
        pimpl.setValue<kw::NumberOfContourPoints>(static_cast<int>(fiducial.contourData.size()), dataSet);
        pimpl.setValues<kw::ContourData>(toFloats(fiducial.contourData), dataSet);
    }

    return dataSet;
}

//------------------------------------------------------------------------------

gdcm::DataSet toGdcm(detail::SeriesImpl& pimpl, fiducials_series::FiducialSet fiducialSet)
{
    gdcm::DataSet dataSet;
    if(fiducialSet.referencedImageSequence)
    {
        auto referencedImageSequence = gdcm::SequenceOfItems::New();
        for(const fiducials_series::ReferencedImage& referencedImage : *fiducialSet.referencedImageSequence)
        {
            gdcm::Item item;
            item.SetNestedDataSet(toGdcm(pimpl, referencedImage));
            referencedImageSequence->AddItem(item);
        }

        detail::SeriesImpl::setSequence(kw::ReferencedImageSequence::GetTag(), referencedImageSequence, dataSet);
    }

    if(fiducialSet.frameOfReferenceUID)
    {
        pimpl.setStringValue<kw::FrameOfReferenceUID>(*fiducialSet.frameOfReferenceUID, dataSet);
    }

    auto fiducialSequence = gdcm::SequenceOfItems::New();
    for(const fiducials_series::Fiducial& fiducial : fiducialSet.fiducialSequence)
    {
        gdcm::Item item;
        item.SetNestedDataSet(toGdcm(pimpl, fiducial));
        fiducialSequence->AddItem(item);
    }

    detail::SeriesImpl::setSequence(kw::FiducialSequence::GetTag(), fiducialSequence, dataSet);
    detail::SeriesImpl::setPrivateValue(0, fiducialSet.groupName, dataSet);
    detail::SeriesImpl::setPrivateValue(1, colorToString(fiducialSet.color), dataSet);
    std::optional<std::string> size;
    if(fiducialSet.size.has_value())
    {
        size = std::to_string(*fiducialSet.size);
    }

    detail::SeriesImpl::setPrivateValue(2, size, dataSet);
    detail::SeriesImpl::setPrivateValue(3, privateShapeToString(fiducialSet.shape), dataSet);
    if(fiducialSet.visibility.has_value())
    {
        detail::SeriesImpl::setPrivateValue(4, *fiducialSet.visibility ? "true" : "false", dataSet);
    }

    return dataSet;
}

//------------------------------------------------------------------------------

template<typename T>
gdcm::SmartPointer<gdcm::SequenceOfItems> appendInSequence(
    detail::SeriesImpl& pimpl,
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence,
    T element
)
{
    auto res = sequence == nullptr ? gdcm::SequenceOfItems::New() : sequence;
    gdcm::Item item;
    item.SetNestedDataSet(toGdcm(pimpl, element));
    res->AddItem(item);
    return res;
}

//------------------------------------------------------------------------------

template<typename T>
void appendInSequence(
    detail::SeriesImpl& pimpl,
    gdcm::Tag tag,
    T element,
    const std::vector<std::pair<gdcm::Tag, std::size_t> >& indices = {})
{
    pimpl.setSequence(tag, appendInSequence(pimpl, pimpl.getSequence(tag, 0, indices), element));
}

} // namespace

//------------------------------------------------------------------------------

void fiducials_series::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const fiducials_series>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void fiducials_series::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const fiducials_series>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool fiducials_series::Point2::operator==(Point2 other) const
{
    return x == other.x && y == other.y;
}

//------------------------------------------------------------------------------

bool fiducials_series::Point3::operator==(Point3 other) const
{
    return x == other.x && y == other.y && z == other.z;
}

//------------------------------------------------------------------------------

bool fiducials_series::ReferencedImage::operator==(const ReferencedImage& other) const
{
    return referencedSOPClassUID == other.referencedSOPClassUID
           && referencedSOPInstanceUID == other.referencedSOPInstanceUID
           && referencedFrameNumber == other.referencedFrameNumber
           && referencedSegmentNumber == other.referencedSegmentNumber;
}

//------------------------------------------------------------------------------

bool fiducials_series::ReferencedImage::operator!=(const ReferencedImage& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool fiducials_series::GraphicCoordinatesData::operator==(const GraphicCoordinatesData& other) const
{
    return referencedImageSequence == other.referencedImageSequence && graphicData == other.graphicData;
}

//------------------------------------------------------------------------------

bool fiducials_series::GraphicCoordinatesData::operator!=(const GraphicCoordinatesData& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool fiducials_series::Fiducial::operator==(const Fiducial& other) const
{
    return shapeType == other.shapeType && fiducialDescription == other.fiducialDescription
           && fiducialIdentifier == other.fiducialIdentifier
           && graphicCoordinatesDataSequence == other.graphicCoordinatesDataSequence && fiducialUID == other.fiducialUID
           && contourData == other.contourData;
}

//------------------------------------------------------------------------------

bool fiducials_series::Fiducial::operator!=(const Fiducial& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool fiducials_series::FiducialSet::operator==(const FiducialSet& other) const
{
    return referencedImageSequence == other.referencedImageSequence && frameOfReferenceUID == other.frameOfReferenceUID
           && fiducialSequence == other.fiducialSequence && groupName == other.groupName;
}

fiducials_series::fiducials_series()
{
    setSOPKeyword(dicom::sop::Keyword::SpatialFiducialsStorage);
}

//------------------------------------------------------------------------------

bool fiducials_series::operator==(const fiducials_series& other) const
{
    return getContentDate() == other.getContentDate() && getContentLabel() == other.getContentLabel()
           && getContentDescription() == other.getContentDescription()
           && getContentCreatorName() == other.getContentCreatorName() && getFiducialSets() == other.getFiducialSets()
           && base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool fiducials_series::operator!=(const fiducials_series& other) const
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentDate() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentDate>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentDate(const std::string& contentDate)
{
    m_pimpl->setStringValue<kw::ContentDate>(contentDate);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentLabel() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentLabel>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentLabel(const std::string& contentLabel)
{
    m_pimpl->setStringValue<kw::ContentLabel>(contentLabel);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentDescription() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentDescription>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentDescription(const std::string& contentDescription)
{
    m_pimpl->setStringValue<kw::ContentDescription>(contentDescription);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getContentCreatorName() const noexcept
{
    return m_pimpl->getStringValue<kw::ContentCreatorName>();
}

//------------------------------------------------------------------------------

void fiducials_series::setContentCreatorName(const std::string& contentCreatorName)
{
    m_pimpl->setStringValue<kw::ContentCreatorName>(contentCreatorName);
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::FiducialSet> fiducials_series::getFiducialSets() const noexcept
{
    return toVector<FiducialSet>(m_pimpl->getSequence<kw::FiducialSetSequence>()).value_or(std::vector<FiducialSet> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialSets(const std::vector<FiducialSet>& fiducialSets)
{
    m_pimpl->setSequence(kw::FiducialSetSequence::GetTag(), toSequence(fiducialSets));
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialSet(std::size_t fiducialSetNumber, FiducialSet fiducialSet)
{
    m_pimpl->getOrCreateDataSet(0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}}) = toGdcm(
        *m_pimpl,
        fiducialSet
    );
}

//------------------------------------------------------------------------------

void fiducials_series::appendFiducialSet(FiducialSet fiducialSet)
{
    appendInSequence(*m_pimpl, kw::FiducialSetSequence::GetTag(), fiducialSet);
}

//------------------------------------------------------------------------------

std::optional<std::vector<fiducials_series::ReferencedImage> > fiducials_series::getReferencedImages(
    std::size_t fiducialSetNumber
) const noexcept
{
    return toVector<ReferencedImage>(
        m_pimpl->getSequence(
            kw::ReferencedImageSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedImages(
    std::size_t fiducialSetNumber,
    const std::optional<std::vector<ReferencedImage> >& referencedImages
)
{
    m_pimpl->setSequence(
        kw::ReferencedImageSequence::GetTag(),
        toSequence(
            referencedImages
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedImage(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    ReferencedImage referencedImage
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        }) = toGdcm(*m_pimpl, referencedImage);
}

//------------------------------------------------------------------------------

void fiducials_series::appendReferencedImage(std::size_t fiducialSetNumber, ReferencedImage referencedImage)
{
    appendInSequence(
        *m_pimpl,
        kw::ReferencedImageSequence::GetTag(),
        referencedImage,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    const std::string& ReferencedSOPClassUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPClassUID>(
        ReferencedSOPClassUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    const std::string& referencedSOPInstanceUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPInstanceUID>(
        referencedSOPInstanceUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> fiducials_series::getReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    std::vector<std::int32_t> referencedFrameNumber
)
{
    m_pimpl->setValues<kw::ReferencedFrameNumber>(
        referencedFrameNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> fiducials_series::getReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t referencedImageNumber,
    std::vector<std::uint16_t> referencedSegmentNumber
)
{
    m_pimpl->setValues<kw::ReferencedSegmentNumber>(
        referencedSegmentNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::ReferencedImageSequence::GetTag(), referencedImageNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getFrameOfReferenceUID(std::size_t fiducialSetNumber) const noexcept
{
    return m_pimpl->getValue<kw::FrameOfReferenceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setFrameOfReferenceUID(
    std::size_t fiducialSetNumber,
    const std::optional<std::string>& frameOfReferenceUID
)
{
    m_pimpl->setStringValue<kw::FrameOfReferenceUID>(
        frameOfReferenceUID.value_or(
            ""
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Fiducial> fiducials_series::getFiducials(std::size_t fiducialSetNumber) const noexcept
{
    return toVector<Fiducial>(
        m_pimpl->getSequence(
            kw::FiducialSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }
            })
    ).value_or(std::vector<Fiducial> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducials(std::size_t fiducialSetNumber, const std::vector<Fiducial>& fiducials)
{
    m_pimpl->setSequence(
        kw::FiducialSequence::GetTag(),
        toSequence(fiducials),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducial(std::size_t fiducialSetNumber, std::size_t fiducialNumber, Fiducial fiducial)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        }) = toGdcm(*m_pimpl, fiducial);
}

//------------------------------------------------------------------------------

void fiducials_series::appendFiducial(std::size_t fiducialSetNumber, Fiducial fiducial)
{
    appendInSequence(
        *m_pimpl,
        kw::FiducialSequence::GetTag(),
        fiducial,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

fiducials_series::Shape fiducials_series::getShapeType(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return stringToShape(
        m_pimpl->getValue<kw::ShapeType>(
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }, {kw::FiducialSequence::GetTag(), fiducialNumber
                }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setShapeType(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    fiducials_series::Shape shapeType
)
{
    m_pimpl->setValue<kw::ShapeType>(
        shapeToString(shapeType),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::getFiducialDescription(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialDescription>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialDescription(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::string& fiducialDescription
)
{
    m_pimpl->setStringValue<kw::FiducialDescription>(
        fiducialDescription,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::getFiducialIdentifier(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialIdentifier>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialIdentifier(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::string& fiducialIdentifier
)
{
    m_pimpl->setStringValue<kw::FiducialIdentifier>(
        fiducialIdentifier,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::vector<fiducials_series::GraphicCoordinatesData> > fiducials_series::
getGraphicCoordinatesDataSequence(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return toVector<GraphicCoordinatesData>(
        m_pimpl->getSequence(
            kw::GraphicCoordinatesDataSequence::GetTag(),
            0,
            {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
            }, {kw::FiducialSequence::GetTag(), fiducialNumber
                }
            })
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setGraphicCoordinatesDataSequence(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::optional<std::vector<GraphicCoordinatesData> >& graphicCoordinatesDataSequence
)
{
    m_pimpl->setSequence(
        kw::GraphicCoordinatesDataSequence::GetTag(),
        toSequence(
            graphicCoordinatesDataSequence
        ),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setGraphicCoordinatesData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    GraphicCoordinatesData graphicCoordinatesData
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }
        }) = toGdcm(*m_pimpl, graphicCoordinatesData);
}

//------------------------------------------------------------------------------

void fiducials_series::appendGraphicCoordinatesData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    GraphicCoordinatesData graphicCoordinatesData
)
{
    appendInSequence(
        *m_pimpl,
        kw::GraphicCoordinatesDataSequence::GetTag(),
        graphicCoordinatesData,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

fiducials_series::ReferencedImage fiducials_series::getReferencedImage(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    std::optional<gdcm::DataSet> dataSet = m_pimpl->getDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
    if(!dataSet)
    {
        return {};
    }

    return toReferencedImage(*dataSet);
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedImage(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    ReferencedImage referencedImage
)
{
    m_pimpl->getOrCreateDataSet(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }, {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }) = toGdcm(*m_pimpl, referencedImage);
}

//------------------------------------------------------------------------------

std::string fiducials_series::getReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPClassUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPClassUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    const std::string& ReferencedSOPClassUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPClassUID>(
        ReferencedSOPClassUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::string fiducials_series::getReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or("");
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSOPInstanceUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    const std::string& referencedSOPInstanceUID
)
{
    m_pimpl->setStringValue<kw::ReferencedSOPInstanceUID>(
        referencedSOPInstanceUID,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::int32_t> fiducials_series::getReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedFrameNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::int32_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedFrameNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    std::vector<std::int32_t> referencedFrameNumber
)
{
    m_pimpl->setValues<kw::ReferencedFrameNumber>(
        referencedFrameNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<std::uint16_t> fiducials_series::getReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    return m_pimpl->getValues<kw::ReferencedSegmentNumber>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        }).value_or(std::vector<std::uint16_t> {});
}

//------------------------------------------------------------------------------

void fiducials_series::setReferencedSegmentNumber(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    std::vector<std::uint16_t> referencedSegmentNumber
)
{
    m_pimpl->setValues<kw::ReferencedSegmentNumber>(
        referencedSegmentNumber,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }, {kw::ReferencedImageSequence::GetTag(), 0
            }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point2> fiducials_series::getGraphicData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber
) const noexcept
{
    std::optional<std::vector<float> > graphicData = m_pimpl->getValues<kw::GraphicData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }
        });
    if(!graphicData)
    {
        return {};
    }

    return toPoint2(*graphicData);
}

//------------------------------------------------------------------------------

void fiducials_series::setGraphicData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    std::size_t graphicCoordinatesDataNumber,
    const std::vector<fiducials_series::Point2>& graphicData
)
{
    m_pimpl->setValues<kw::GraphicData>(
        toFloats(graphicData),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            },
            {kw::GraphicCoordinatesDataSequence::GetTag(), graphicCoordinatesDataNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getFiducialUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    return m_pimpl->getValue<kw::FiducialUID>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setFiducialUID(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::optional<std::string>& fiducialUID
)
{
    m_pimpl->setStringValue<kw::FiducialUID>(
        fiducialUID.value_or(""),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point3> fiducials_series::getContourData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber
) const noexcept
{
    std::optional<std::vector<double> > contourData = m_pimpl->getValues<kw::ContourData>(
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        },
            {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
    if(!contourData)
    {
        return {};
    }

    return toPoint3(*contourData);
}

//------------------------------------------------------------------------------

void fiducials_series::setContourData(
    std::size_t fiducialSetNumber,
    std::size_t fiducialNumber,
    const std::vector<fiducials_series::Point3>& contourData
)
{
    m_pimpl->setValues<kw::ContourData>(
        toFloats(contourData),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }, {kw::FiducialSequence::GetTag(), fiducialNumber
            }
        });
}

//------------------------------------------------------------------------------

std::optional<std::string> fiducials_series::getGroupName(std::size_t fiducialSetNumber) const noexcept
{
    return m_pimpl->getPrivateValue(0, 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}});
}

//------------------------------------------------------------------------------

void fiducials_series::setGroupName(std::size_t fiducialSetNumber, const std::string& groupName)
{
    m_pimpl->setPrivateValue(0, groupName, 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}});
}

//------------------------------------------------------------------------------

std::optional<std::array<float, 4> > fiducials_series::getColor(std::size_t fiducialSetNumber) const noexcept
{
    return stringToColor(m_pimpl->getPrivateValue(1, 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}}));
}

//------------------------------------------------------------------------------

void fiducials_series::setColor(std::size_t fiducialSetNumber, const std::array<float, 4>& color)
{
    m_pimpl->setPrivateValue(1, colorToString(color), 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}});
}

//------------------------------------------------------------------------------

std::optional<float> fiducials_series::getSize(std::size_t fiducialSetNumber) const noexcept
{
    std::optional<std::string> privateValue = m_pimpl->getPrivateValue(
        2,
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
    if(!privateValue.has_value())
    {
        return std::nullopt;
    }

    return std::stof(*privateValue);
}

//------------------------------------------------------------------------------

void fiducials_series::setSize(std::size_t fiducialSetNumber, float size)
{
    m_pimpl->setPrivateValue(2, std::to_string(size), 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}});
}

//------------------------------------------------------------------------------

std::optional<fiducials_series::PrivateShape> fiducials_series::getShape(std::size_t fiducialSetNumber) const noexcept
{
    return stringToPrivateShape(
        m_pimpl->getPrivateValue(3, 0, {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber}})
    );
}

//------------------------------------------------------------------------------

void fiducials_series::setShape(std::size_t fiducialSetNumber, PrivateShape shape)
{
    m_pimpl->setPrivateValue(
        3,
        privateShapeToString(shape),
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

std::optional<bool> fiducials_series::getVisibility(std::size_t fiducialSetNumber) const noexcept
{
    if(std::optional<std::string> visibility = m_pimpl->getPrivateValue(
           4,
           0,
           {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
           }
           }))
    {
        return *visibility == "true";
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void fiducials_series::setVisibility(std::size_t fiducialSetNumber, bool visibility)
{
    m_pimpl->setPrivateValue(
        4,
        visibility ? "true" : "false",
        0,
        {{kw::FiducialSetSequence::GetTag(), fiducialSetNumber
        }
        });
}

//------------------------------------------------------------------------------

void fiducials_series::setGroupNamesForPointFiducials()
{
    std::vector<FiducialSet> fiducialSets = getFiducialSets();
    for(FiducialSet& fiducialSet : fiducialSets)
    {
        if(fiducialSet.groupName.has_value() && !fiducialSet.groupName->empty())
        {
            continue;
        }

        bool containsAPointFiducial = false;
        for(const Fiducial& fiducial : fiducialSet.fiducialSequence)
        {
            if(fiducial.shapeType == Shape::POINT)
            {
                containsAPointFiducial = true;
                break;
            }
        }

        if(containsAPointFiducial)
        {
            std::size_t i         = 0;
            std::string groupName = "Group_" + std::to_string(i);
            while(std::ranges::find_if(
                      fiducialSets,
                      [&groupName](const FiducialSet& fiducialSet)
                {
                    return fiducialSet.groupName.value_or("") == groupName;
                }) != fiducialSets.end())
            {
                i++;
                groupName = "Group_" + std::to_string(i);
            }

            fiducialSet.groupName = groupName;
        }
    }

    setFiducialSets(fiducialSets);
}

//------------------------------------------------------------------------------

std::vector<std::string> fiducials_series::getPointFiducialsGroupNames() const
{
    std::vector<std::string> res;
    std::vector<FiducialSet> fiducialSets = getFiducialSets();
    res.reserve(fiducialSets.size());
    // Ignore fiducial sets which doesn't contain fiducials with ShapeType == POINT (= Landmarks)
    std::vector<FiducialSet> pointFiducialSets;
    std::ranges::copy_if(
        fiducialSets,
        std::back_inserter(pointFiducialSets),
        [](const FiducialSet& fiducialSet)
        {
            return fiducialSet.fiducialSequence.empty() || std::ranges::find_if(
                fiducialSet.fiducialSequence,
                [](
                    const Fiducial& fiducial){return fiducial.shapeType == Shape::POINT;}) != fiducialSet.fiducialSequence.end();
        });
    std::ranges::for_each(
        pointFiducialSets,
        [&res](const FiducialSet& fs)
        {
            if(fs.groupName.has_value())
            {
                res.push_back(*fs.groupName);
            }
        });
    return res;
}

//------------------------------------------------------------------------------

std::optional<std::pair<fiducials_series::FiducialSet, std::size_t> > fiducials_series::getFiducialSetAndIndex(
    const std::string& _groupName
) const
{
    for(std::size_t index = 0 ;
        const FiducialSet& fiducialSet : getFiducialSets())
    {
        if(fiducialSet.groupName == _groupName)
        {
            return {{fiducialSet, index}};
        }

        index++;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

std::optional<std::size_t> fiducials_series::getNumberOfPointsInGroup(const std::string& groupName) const
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducialSet = getFiducialSetAndIndex(groupName);
    if(!fiducialSet.has_value())
    {
        SIGHT_ASSERT("The group name '" << groupName << "' doesn't exist", false);
        return {};
    }

    return std::size_t(std::ranges::count_if(
                           fiducialSet->first.fiducialSequence,
                           [](const Fiducial& fiducial)
        {
            return fiducial.shapeType == Shape::POINT;
        }));
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<fiducials_series::Fiducial> fiducials_series::getPointFiducials(
    const fiducials_series::FiducialSet& fiducialSet
)
{
    std::vector<Fiducial> pointFiducials;
    std::ranges::copy_if(
        fiducialSet.fiducialSequence,
        std::back_inserter(pointFiducials),
        [](const auto f)
        {
            return f.shapeType == Shape::POINT;
        });
    return pointFiducials;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > fiducials_series::getPoint(
    const fiducials_series::Fiducial& fiducial
)
{
    if(fiducial.shapeType != Shape::POINT)
    {
        // Only point fiducials are supported
        return std::nullopt;
    }

    if(!fiducial.contourData.empty())
    {
        data::fiducials_series::Point3 point = fiducial.contourData[0];
        return {{point.x, point.y, point.z}};
    }

    // Position with Graphic Coordinates Data Sequence isn't supported
    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > fiducials_series::getPoint(
    const std::string& groupName,
    std::size_t index
) const
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducialSet = getFiducialSetAndIndex(groupName);
    if(!fiducialSet.has_value())
    {
        return std::nullopt;
    }

    for(std::size_t i = 0 ; const Fiducial& fiducial : fiducialSet->first.fiducialSequence)
    {
        if(fiducial.shapeType == Shape::POINT)
        {
            if(i == index)
            {
                return getPoint(fiducial);
            }

            i++;
        }
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<landmarks::LandmarksGroup> fiducials_series::getGroup(const std::string& _groupName) const
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducialSet = getFiducialSetAndIndex(_groupName);
    if(!fiducialSet.has_value())
    {
        return std::nullopt;
    }

    landmarks::ColorType color = fiducialSet->first.color.value_or(
        std::array {1.F, 1.F, 1.F, 1.F
        });
    landmarks::SizeType size                            = fiducialSet->first.size.value_or(10);
    fiducials_series::PrivateShape fiducialPrivateShape = fiducialSet->first.shape.value_or(PrivateShape::SPHERE);
    landmarks::Shape shape                              = landmarks::Shape::SPHERE;
    switch(fiducialPrivateShape)
    {
        case PrivateShape::SPHERE:
            shape = landmarks::Shape::SPHERE;
            break;

        case PrivateShape::CUBE:
            shape = landmarks::Shape::CUBE;

        default:
            break;
    }

    bool visibility = fiducialSet->first.visibility.value_or(true);

    landmarks::LandmarksGroup group(color, size, shape, visibility);
    std::ranges::for_each(
        getPointFiducials(fiducialSet->first),
        [&group](const data::fiducials_series::Fiducial& fiducial)
        {
            if(auto point = getPoint(fiducial))
            {
                group.m_points.push_back(*point);
            }
        });
    return group;
}

//------------------------------------------------------------------------------

void fiducials_series::removePoint(const std::string& groupName, std::size_t index)
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducialSet = getFiducialSetAndIndex(groupName);
    if(!fiducialSet.has_value())
    {
        return;
    }

    std::size_t pointIndex = 0;
    for(auto it = fiducialSet->first.fiducialSequence.begin() ;
        it < fiducialSet->first.fiducialSequence.end() ;
        ++it)
    {
        if(it->shapeType == data::fiducials_series::Shape::POINT)
        {
            if(pointIndex == index)
            {
                fiducialSet->first.fiducialSequence.erase(it);
                break;
            }

            pointIndex++;
        }
    }

    setFiducialSet(fiducialSet->second, fiducialSet->first);
}

//------------------------------------------------------------------------------

void fiducials_series::removeGroup(const std::string& groupName)
{
    std::vector<FiducialSet> fiducialSets = getFiducialSets();
    std::erase_if(fiducialSets, [&groupName](const FiducialSet& fs){return fs.groupName == groupName;});
    setFiducialSets(fiducialSets);
}

//------------------------------------------------------------------------------

void fiducials_series::addGroup(const std::string& groupName, const std::array<float, 4>& color, float size)
{
    FiducialSet fiducialSet;
    fiducialSet.groupName = groupName;
    fiducialSet.color     = color;
    fiducialSet.size      = size;
    fiducialSet.shape     = PrivateShape::SPHERE;
    appendFiducialSet(fiducialSet);
}

//------------------------------------------------------------------------------

void fiducials_series::addPoint(const std::string& groupName, const std::array<double, 3>& pos)
{
    std::optional<std::pair<FiducialSet, std::size_t> > fiducialSet = getFiducialSetAndIndex(groupName);
    if(!fiducialSet.has_value())
    {
        SIGHT_WARN("Couldn't add point in fiducial set '" << groupName << "', the group doesn't exist.");
        return;
    }

    std::string fiducialName = groupName + '_' + std::to_string(getPointFiducials(fiducialSet->first).size());
    Fiducial fiducial;
    fiducial.shapeType           = Shape::POINT;
    fiducial.fiducialDescription = fiducialName;
    fiducial.fiducialIdentifier  = fiducialName;
    fiducial.fiducialUID         = core::tools::UUID::generate();
    fiducial.contourData         = {{.x = pos[0], .y = pos[1], .z = pos[2]}};
    appendFiducial(fiducialSet->second, fiducial);
}

//------------------------------------------------------------------------------

fiducials_series::Shape fiducials_series::stringToShape(const std::optional<std::string>& string)
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
    if(!string || !stringToShapeMap.contains(*string))
    {
        return Shape::INVALID;
    }

    return stringToShapeMap.at(*string);
}

//------------------------------------------------------------------------------

std::optional<std::array<float, 4> > fiducials_series::stringToColor(const std::optional<std::string>& string)
{
    if(!string.has_value())
    {
        return std::nullopt;
    }

    std::vector<std::string> splits;
    boost::split(splits, *string, boost::is_any_of(","));
    if(splits.size() != 4)
    {
        return std::nullopt;
    }

    std::array<float, 4> res {};
    std::ranges::transform(splits, res.begin(), [](const std::string& value){return std::stof(value);});
    return res;
}

//------------------------------------------------------------------------------

std::optional<fiducials_series::PrivateShape> fiducials_series::stringToPrivateShape(
    const std::optional<std::string>& string
)
{
    static const std::map<std::string, PrivateShape> stringToPrivateShapeMap {
        {"SPHERE", PrivateShape::SPHERE},
        {"CUBE", PrivateShape::CUBE}
    };
    if(!string.has_value())
    {
        return std::nullopt;
    }

    if(!stringToPrivateShapeMap.contains(*string))
    {
        return std::nullopt;
    }

    return stringToPrivateShapeMap.at(*string);
}

//------------------------------------------------------------------------------

fiducials_series::FiducialSet fiducials_series::toFiducialSet(const gdcm::DataSet& dataSet) const
{
    FiducialSet fiducialSet;
    if(auto ris = m_pimpl->getSequence(kw::ReferencedImageSequence::GetTag(), dataSet))
    {
        fiducialSet.referencedImageSequence = std::vector<ReferencedImage> {};
        for(std::size_t i = 1 ; i <= ris->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducialSet.referencedImageSequence->push_back(toReferencedImage(ris->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducialSet.frameOfReferenceUID = m_pimpl->getValue<kw::FrameOfReferenceUID>(dataSet);
    if(auto fiducialSequence = m_pimpl->getSequence(kw::FiducialSequence::GetTag(), dataSet))
    {
        for(std::size_t i = 1 ; i <= fiducialSequence->GetNumberOfItems() ; i++) // GDCM Sequence of Items is 1-indexed
        {
            fiducialSet.fiducialSequence.push_back(toFiducial(fiducialSequence->GetItem(i).GetNestedDataSet()));
        }
    }

    fiducialSet.groupName = m_pimpl->getPrivateValue(0, dataSet);
    fiducialSet.color     = stringToColor(m_pimpl->getPrivateValue(1, dataSet));
    if(std::optional<std::string> size = m_pimpl->getPrivateValue(2, dataSet))
    {
        fiducialSet.size = std::stof(*size);
    }

    fiducialSet.shape = stringToPrivateShape(m_pimpl->getPrivateValue(3, dataSet));
    if(std::optional<std::string> visibility = m_pimpl->getPrivateValue(4, dataSet))
    {
        fiducialSet.visibility = (*visibility == "true");
    }

    return fiducialSet;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::FiducialSet fiducials_series::to<fiducials_series::FiducialSet>(const gdcm::DataSet& dataSet) const
{
    return toFiducialSet(dataSet);
}

//------------------------------------------------------------------------------

fiducials_series::ReferencedImage fiducials_series::toReferencedImage(const gdcm::DataSet& dataSet) const
{
    return ReferencedImage {
        .referencedSOPClassUID    = m_pimpl->getValue<kw::ReferencedSOPClassUID>(dataSet).value_or(""),
        .referencedSOPInstanceUID = m_pimpl->getValue<kw::ReferencedSOPInstanceUID>(dataSet).value_or(""),
        .referencedFrameNumber    = m_pimpl->getValues<kw::ReferencedFrameNumber>(dataSet).value_or(
            std::vector<std::int32_t> {
            }),
        .referencedSegmentNumber = m_pimpl->getValues<kw::ReferencedSegmentNumber>(dataSet).value_or(
            std::vector<std::uint16_t> {
            })
    };
}

//------------------------------------------------------------------------------

template<>
fiducials_series::ReferencedImage fiducials_series::to<fiducials_series::ReferencedImage>(const gdcm::DataSet& dataSet)
const
{
    return toReferencedImage(dataSet);
}

//------------------------------------------------------------------------------

fiducials_series::Fiducial fiducials_series::toFiducial(const gdcm::DataSet& dataSet) const
{
    Fiducial fiducial;
    fiducial.shapeType           = stringToShape(m_pimpl->getValue<kw::ShapeType>(dataSet));
    fiducial.fiducialDescription = m_pimpl->getValue<kw::FiducialDescription>(dataSet).value_or("");
    fiducial.fiducialIdentifier  = m_pimpl->getValue<kw::FiducialIdentifier>(dataSet).value_or("");
    if(auto gcds = m_pimpl->getSequence(kw::GraphicCoordinatesDataSequence::GetTag(), dataSet))
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

    fiducial.fiducialUID = m_pimpl->getValue<kw::FiducialUID>(dataSet);
    if(auto contourData = m_pimpl->getValues<kw::ContourData>(dataSet))
    {
        fiducial.contourData = toPoint3(*contourData);
    }

    return fiducial;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::Fiducial fiducials_series::to<fiducials_series::Fiducial>(const gdcm::DataSet& dataSet) const
{
    return toFiducial(dataSet);
}

//------------------------------------------------------------------------------

fiducials_series::GraphicCoordinatesData fiducials_series::toGraphicCoordinatesData(const gdcm::DataSet& dataSet) const
{
    GraphicCoordinatesData gcd;
    if(auto ris = m_pimpl->getSequence(kw::ReferencedImageSequence::GetTag(), dataSet);
       ris != nullptr && ris->GetNumberOfItems() > 0)
    {
        // GDCM Sequence of Items is 1-indexed
        gcd.referencedImageSequence = toReferencedImage(ris->GetItem(1).GetNestedDataSet());
    }

    if(auto graphicData = m_pimpl->getValues<kw::GraphicData>(dataSet))
    {
        gcd.graphicData = toPoint2(*graphicData);
    }

    return gcd;
}

//------------------------------------------------------------------------------

template<>
fiducials_series::GraphicCoordinatesData fiducials_series::to<fiducials_series::GraphicCoordinatesData>(
    const gdcm::DataSet& dataSet
) const
{
    return toGraphicCoordinatesData(dataSet);
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point2> fiducials_series::toPoint2(const std::vector<float>& floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 2", (floats.size() % 2) == 0);
    std::vector<Point2> res;
    res.reserve(floats.size() / 2);
    for(std::size_t i = 0 ; i < floats.size() ; i += 2)
    {
        res.push_back({.x = floats[i], .y = floats[i + 1]});
    }

    return res;
}

//------------------------------------------------------------------------------

std::vector<fiducials_series::Point3> fiducials_series::toPoint3(const std::vector<double>& floats)
{
    SIGHT_ASSERT("The number of elements must be a multiple of 3", (floats.size() % 3) == 0);
    std::vector<Point3> res;
    res.reserve(floats.size() / 3);
    for(std::size_t i = 0 ; i < floats.size() ; i += 3)
    {
        res.push_back({.x = floats[i], .y = floats[i + 1], .z = floats[i + 2]});
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
        item.SetNestedDataSet(toGdcm(*m_pimpl, element));
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
