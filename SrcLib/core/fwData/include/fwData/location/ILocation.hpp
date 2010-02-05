/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_LOCATION_HPP_
#define _FWDATA_LOCATION_HPP_

#include "fwData/config.hpp"
#include "fwData/Object.hpp"


namespace fwData
{
/**
 * @namespace ::fwData::location
 * @brief This namespace contains the classes which define a %location ( fileSystem, directory(IES) etc.... ).
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
namespace location
{

/**
 * @brief Abstract class to define a %location ( fileSystem, directory(IES) etc.... )
 *
 * @note cannot be instanciated but derivated class OK
 * @see Folder, MultiFiles, SingleFile
 */
class FWDATA_CLASS_API ILocation : public ::fwData::Object
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ILocation)(::fwData::Object::Baseclass) );

protected:

    /// Constructor : cannot be instanciated but derivated class OK
    FWDATA_API ILocation();

    /// Destructor
    FWDATA_API virtual ~ILocation();
};


/**
 * @brief Get reader or writer %location
 * @param[in] rw reader or writer
 */
template<class LOCATION, class RW >
inline typename LOCATION::sptr getLocation(RW *rw)
{
    assert( rw );
    typename LOCATION::sptr location;
    location = LOCATION::dynamicCast( rw->getLocation() );
    if ( !location )
    {
        location = typename LOCATION::NewSptr();
        rw->setLocation(location);
    }
    return location;
}

/// test if a Reader/writer (ie a class implementing get/setLocation) have an specialized LOCATION
template<class LOCATION, class RW >
inline bool have(RW *rw)
{
    return LOCATION::dynamicCast( rw->getLocation() );
}



}
}

#endif /* LOCATION_HPP_ */
