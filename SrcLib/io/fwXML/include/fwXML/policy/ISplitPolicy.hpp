/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_ISPLITPOLICY_H_
#define _FWXML_POLICY_ISPLITPOLICY_H_

#include <boost/shared_ptr.hpp>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

namespace fwXML
{
/**
 * @brief   defines the ISplitPolicy class.
 *
 * The purpose of this class is to allow to define which class genererate
 * a split by creating a new XMLAggregator
 *
 */
class FWXML_CLASS_API ISplitPolicy : public ::fwCore::BaseObject
{
public:

    fwCoreNonInstanciableClassDefinitionsMacro ( (ISplitPolicy)(::fwCore::BaseObject) ) ;

    /**
     * @brief define if object must be splitted
     *
     *  Subclass should implement this method
     *
     * @return  true iff the object should be splitted, false otherwise
     */
    FWXML_API virtual bool split(::fwData::Object::sptr obj) = 0 ;

    /**
     * @brief       Virtual destructor
     */
    FWXML_API virtual ~ISplitPolicy();

};

}
#endif // _FWXML_POLICY_ISPLITPOLICY_H_



