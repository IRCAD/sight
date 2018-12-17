/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDATA_LOCATION_ILOCATION_HPP__
#define __FWDATA_LOCATION_ILOCATION_HPP__

#include "fwData/Object.hpp"
#include "fwData/config.hpp"

#include <boost/filesystem/path.hpp>

namespace fwData
{
/**
 * @brief This namespace contains the classes which define a %location ( fileSystem, directory(IES) etc.... ).
 */
namespace location
{

/**
 * @brief Abstract class to define a %location ( fileSystem, directory(IES) etc.... )
 * @note cannot be instantiated but derived class OK
 * @see Folder, MultiFiles, SingleFile
 */
class FWDATA_CLASS_API ILocation : public ::fwData::Object
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ILocation)(::fwData::Object) );

    typedef ::boost::filesystem::path PathType;
    typedef std::vector<PathType> VectPathType;

protected:

    /// Constructor : cannot be instantiated but derivated class OK
    FWDATA_API ILocation();

    /// Destructor
    FWDATA_API virtual ~ILocation();
};

/**
 * @brief Get reader or writer %location
 * @param[in] rw reader or writer
 */
template<class LOCATION, class RW >
inline typename LOCATION::sptr getLocation(RW* rw)
{
    SLM_ASSERT("rw not instanced", rw);
    typename LOCATION::sptr location;
    location = LOCATION::dynamicCast( rw->getLocation() );
    if ( !location )
    {
        location = LOCATION::New();
        rw->setLocation(location);
    }
    return location;
}

/// test if a Reader/writer (ie a class implementing get/setLocation) have an specialized LOCATION
template<class LOCATION, class RW >
inline bool have(RW* rw)
{
    return (LOCATION::dynamicCast( rw->getLocation() )) ? true : false;
}

}
}

#endif /* __FWDATA_LOCATION_ILOCATION_HPP__ */
