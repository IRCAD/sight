/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/MatrixConverter.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <igtl/igtlTransformMessage.h>

namespace igtlProtocol
{
namespace converter
{
const std::string MatrixConverter::s_IGTL_TYPE          = "TRANSFORM";
const std::string MatrixConverter::s_FWDATA_OBJECT_TYPE = ::fwData::TransformationMatrix3D::classname();

converterRegisterMacro(::igtlProtocol::converter::MatrixConverter);

MatrixConverter::MatrixConverter()
{
}

//-----------------------------------------------------------------------------

MatrixConverter::~MatrixConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer MatrixConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::fwData::TransformationMatrix3D::csptr srcMatrix = ::fwData::TransformationMatrix3D::dynamicConstCast(src);
    ::igtl::TransformMessage::Pointer msg;
    ::igtl::Matrix4x4 dest;

    msg = ::igtl::TransformMessage::New();
    msg->GetMatrix(dest);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            dest[i][j] = srcMatrix->getCoefficient (i, j);
        }
    }
    msg->SetMatrix(dest);
    return ::igtl::MessageBase::Pointer(msg.GetPointer());
}

//-----------------------------------------------------------------------------

void MatrixConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                      ::fwData::Object::sptr& destObj) const
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a "
                                                "::fwData::TransformationMatrix3D"),
                          destObj->getClassname() != MatrixConverter::s_FWDATA_OBJECT_TYPE);

    ::igtl::Matrix4x4 matrix;
    ::igtl::TransformMessage::Pointer srcTransform;
    ::fwData::TransformationMatrix3D::sptr dest;

    srcTransform = ::igtl::TransformMessage::Pointer(dynamic_cast< ::igtl::TransformMessage* >(src.GetPointer()));
    dest         = ::fwData::TransformationMatrix3D::dynamicCast(destObj);
    srcTransform->GetMatrix(matrix);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            dest->setCoefficient (i, j, matrix[i][j]);
        }
    }
}

//-----------------------------------------------------------------------------

IConverter::sptr MatrixConverter::New()
{
    return std::make_shared< MatrixConverter >();
}

//-----------------------------------------------------------------------------

std::string const& MatrixConverter::getIgtlType() const
{
    return MatrixConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& MatrixConverter::getFwDataObjectType() const
{
    return MatrixConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
