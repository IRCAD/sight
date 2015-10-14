/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <vector>

#include <boost/filesystem/convenience.hpp>

#include "fwDataIO/writer/TransformationMatrix3DWriter.hpp"
#include "fwDataIO/writer/registry/macros.hpp"


fwDataIOWriterRegisterMacro( ::fwDataIO::writer::TransformationMatrix3DWriter);


namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

TransformationMatrix3DWriter::TransformationMatrix3DWriter(::fwDataIO::writer::IObjectWriter::Key key)
    : ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

TransformationMatrix3DWriter::~TransformationMatrix3DWriter()
{
}

//------------------------------------------------------------------------------

void TransformationMatrix3DWriter::write()
{
    std::fstream file;
    file.open(getFile().string().c_str(), std::fstream::out);
    if (!file.is_open())
    {
        OSLM_ERROR( "Trf file writing error for " << getFile());
        return;
    }
    ::fwData::TransformationMatrix3D::TMCoefArray mat = this->getConcreteObject()->getRefCoefficients();
    int i = 0;
    for (auto val : mat)
    {
        OSLM_TRACE("MATRIX : " << val);
        file << val;
        i++;
        if (i%4 == 0 && i!=16)
        {
            file << std::endl;
        }
        else
        {
            file << " ";
        }
    }

    file.close();
}

//------------------------------------------------------------------------------

std::string TransformationMatrix3DWriter::extension()
{
    static std::string ext(".trf");
    return ext;
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
