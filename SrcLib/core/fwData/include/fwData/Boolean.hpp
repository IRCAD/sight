/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_BOOLEAN_HPP_
#define _FWDATA_BOOLEAN_HPP_

#include "fwData/GenericField.hpp"
#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Boolean), FWDATA_API);
namespace fwData
{

/**
 * @class   Boolean
 * @brief   This class contains a boolean value
 *
 * Boolean object is essentially used as a field in other objects.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Boolean : public GenericField< bool >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Boolean)(::fwData::Object),( ((const bool)(false)) ), GenericFieldFactory< Boolean >) ;
    fwCampMakeFriendDataMacro((fwData)(Boolean));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Boolean( ::fwData::Object::Key key ) throw();

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Boolean() throw();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( const Object::csptr& _source );

protected:

};

} // namespace fwData

#endif // _FWDATA_BOOLEAN_HPP_
