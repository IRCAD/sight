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

#include "fwDataIO/reader/TagReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Tag.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fstream>
#include <iostream>

fwDataIOReaderRegisterMacro( ::fwDataIO::reader::TagReader );

namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

TagReader::TagReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

TagReader::~TagReader()
{
}

//------------------------------------------------------------------------------

void TagReader::read()
{
    assert( ::std::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
    ::boost::filesystem::path path =
        ::std::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();

    OSLM_INFO( "[TagReader::read] Tag file: " << path);
    assert( path.empty() == false );

    ::std::shared_ptr< ::fwData::Tag > tag = getConcreteObject();

    std::fstream file;
    file.open(path.string().c_str(), std::fstream::in);
    if (!file.is_open())
    {
        OSLM_ERROR( "Tag file loading error for " << path.string());
        std::string str = "Unable to open ";
        str += path.string();
        throw std::ios_base::failure(str);
    }

    /// Read content and update tag data structure
    std::string name, type;
    int n;
    file>>name;
    file>>n;
    if(n >= 1)
    {
        file>>type;

        if(type == "ARTAG" || type == "CHESSBOARD" || type == "ARToolKitPlus_MARKER_ID_BCH")
        {
            int nbPts;
            double x, y, z;
            double radius = 0.0;
            file>>x>>y>>z;
            file>>nbPts;
            tag->setType(type);
            for(int i = 0; i < nbPts; i++)
            {
                ::fwData::Point::sptr p;
                fwVec3d vPoint;
                file>>vPoint[0]>>vPoint[1]>>vPoint[2]>>radius;
                p->setCoord(vPoint);
                tag->getPointList()->getPoints().push_back(p);
            }
        }
        else
        {
            SLM_ERROR( "Tag file loading error for " + path.string() + " with type " + type);
            std::string str = "Unable to open ";
            str += path.string();
            throw std::ios_base::failure(str);
        }
    }

    file.close();

}

//------------------------------------------------------------------------------

std::string TagReader::extension()
{
    return (".tag");
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace fwDataIO
