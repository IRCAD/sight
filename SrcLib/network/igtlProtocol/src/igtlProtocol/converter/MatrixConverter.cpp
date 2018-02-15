/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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
            dest[i][j] = srcMatrix->getCoefficient(i, j);
        }
    }
    msg->SetMatrix(dest);
    return ::igtl::MessageBase::Pointer(msg.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr MatrixConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::igtl::Matrix4x4 matrix;
    ::igtl::TransformMessage* msg                  = dynamic_cast< ::igtl::TransformMessage* >(src.GetPointer());
    ::igtl::TransformMessage::Pointer srcTransform = ::igtl::TransformMessage::Pointer(msg);
    ::fwData::TransformationMatrix3D::sptr dest    = ::fwData::TransformationMatrix3D::New();
    srcTransform->GetMatrix(matrix);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            dest->setCoefficient(i, j, matrix[i][j]);
        }
    }

    return dest;
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
