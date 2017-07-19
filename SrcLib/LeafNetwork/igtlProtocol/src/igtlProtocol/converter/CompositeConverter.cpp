/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/CompositeConverter.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <igtlTrackingDataMessage.h>

namespace igtlProtocol
{
namespace converter
{

const std::string CompositeConverter::s_IGTL_TYPE          = "TDATA";
const std::string CompositeConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Composite::classname();

converterRegisterMacro(::igtlProtocol::converter::CompositeConverter);

CompositeConverter::CompositeConverter()
{
}

//-----------------------------------------------------------------------------

CompositeConverter::~CompositeConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer CompositeConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::igtl::TrackingDataMessage::Pointer trackingMsg = ::igtl::TrackingDataMessage::New();

    ::fwData::Composite::csptr composite = ::fwData::Composite::dynamicConstCast(src);
    for(const ::fwData::Composite::ContainerType::value_type& elt : composite->getContainer())
    {
        ::fwData::TransformationMatrix3D::csptr transfoMatrix =
            ::fwData::TransformationMatrix3D::dynamicConstCast(elt.second);
        if(transfoMatrix)
        {
            ::igtl::TrackingDataElement::Pointer trackElement = ::igtl::TrackingDataElement::New();
            const std::string name(elt.first);
            trackElement->SetName(name.c_str());
            trackElement->SetType(::igtl::TrackingDataElement::TYPE_TRACKER);

            ::igtl::Matrix4x4 matrix;
            for (size_t i = 0; i < 4; ++i)
            {
                for (size_t j = 0; j < 4; ++j)
                {
                    matrix[i][j] = transfoMatrix->getCoefficient (i, j);
                }
            }
            trackElement->SetMatrix(matrix);
            trackingMsg->AddTrackingDataElement(trackElement);
        }
    }
    return ::igtl::MessageBase::Pointer(trackingMsg.GetPointer());
}

//-----------------------------------------------------------------------------

void CompositeConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                         ::fwData::Object::sptr& destObj) const
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a "
                                                "::fwData::Composite"),
                          destObj->getClassname() != CompositeConverter::s_FWDATA_OBJECT_TYPE);

    ::igtl::TrackingDataMessage::Pointer trackingMsg;
    trackingMsg = ::igtl::TrackingDataMessage::Pointer(dynamic_cast< ::igtl::TrackingDataMessage* >(src.GetPointer()));
    int nbTrckingElement = trackingMsg->GetNumberOfTrackingDataElements();

    FW_RAISE_EXCEPTION_IF(::igtlProtocol::exception::Conversion("TrackingDataElements"), nbTrckingElement < 0);

    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(destObj);
    for(int i = 0; i < nbTrckingElement; ++i)
    {
        ::igtl::TrackingDataElement::Pointer trackElement = ::igtl::TrackingDataElement::New();
        trackingMsg->GetTrackingDataElement(i, trackElement);
        const std::string name = trackElement->GetName();

        ::fwData::TransformationMatrix3D::sptr transfoMatrix;
        ::fwData::Composite::iterator iter = composite->find(name);
        if( iter != composite->end())
        {
            transfoMatrix = ::fwData::TransformationMatrix3D::dynamicCast(iter->second);
            SLM_ASSERT("Object '"+name+"' already exist, but it's not a TransformationMatrix3D.", transfoMatrix);
        }
        else
        {
            transfoMatrix      = ::fwData::TransformationMatrix3D::New();
            (*composite)[name] = transfoMatrix;
        }

        ::igtl::Matrix4x4 matrix;
        trackElement->GetMatrix(matrix);
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                transfoMatrix->setCoefficient (i, j, matrix[i][j]);
            }
        }
    }
}

//-----------------------------------------------------------------------------

IConverter::sptr CompositeConverter::New()
{
    return std::make_shared< CompositeConverter >();
}

//-----------------------------------------------------------------------------

std::string const& CompositeConverter::getIgtlType() const
{
    return CompositeConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& CompositeConverter::getFwDataObjectType() const
{
    return CompositeConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
