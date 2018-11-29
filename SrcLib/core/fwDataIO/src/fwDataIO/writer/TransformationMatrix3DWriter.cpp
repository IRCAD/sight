/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDataIO/writer/TransformationMatrix3DWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <boost/filesystem/convenience.hpp>

#include <fstream>
#include <iostream>
#include <vector>

fwDataIOWriterRegisterMacro( ::fwDataIO::writer::TransformationMatrix3DWriter);

namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

TransformationMatrix3DWriter::TransformationMatrix3DWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
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
    ::fwData::TransformationMatrix3D::TMCoefArray mat = this->getConcreteObject()->getCoefficients();
    int i = 0;
    for (auto val : mat)
    {
        OSLM_TRACE("MATRIX : " << val);
        file << val;
        i++;
        if (i%4 == 0 && i != 16)
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
