/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_OBJECT_HPP_
#define _FWDATA_OBJECT_HPP_

#include <vector>
#include <string>

#include <boost/enable_shared_from_this.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/Field.hpp>
#include <fwTools/DynamicAttributes.hxx>
#include <fwTools/Factory.hpp>

#include "fwData/macros.hpp"
#include "fwData/config.hpp"

fwCorePredeclare( (fwData)(visitor)(IVisitor) );

namespace fwData
{


/**
 * @brief   Base class for each data object.
 * Each object can optionally embed sub-object with string identifier. We name this concept a Field.
 * An Object containing a field name "dummy" corresponds to having a labeledObject with label "dummy" and
 * containing a specific Object. When accessing to this object with getField("dummy") we get the specific Object
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */


class FWDATA_CLASS_API Object  : public ::fwTools::Object, public ::fwTools::DynamicAttributes< ::fwData::Object >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Object)(::fwTools::Object), (( )), ::fwTools::Factory::New< Object > );

protected:

    /// Constructor
    FWDATA_API Object();

    /// Destructor
    FWDATA_API virtual ~Object() ;

};

} // fwData

#endif //_FWDATA_OBJECT_HPP_
