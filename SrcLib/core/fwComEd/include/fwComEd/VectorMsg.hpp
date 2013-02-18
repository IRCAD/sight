/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_VECTORMSG_HPP__
#define __FWCOMED_VECTORMSG_HPP__

#include <fwServices/ObjectMsg.hpp>
#include <fwData/Vector.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for Vector : store modification information
 * @class   VectorMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2005-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API VectorMsg : public ::fwServices::ObjectMsg
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((VectorMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< VectorMsg > );


    /// Event identifier used to inform that objects were added
    FWCOMED_API static std::string ADDED_OBJECTS;

    /// Event identifier used to inform that objects were removed
    FWCOMED_API static std::string REMOVED_OBJECTS;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API VectorMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~VectorMsg() throw();


    /**
     * @brief       This method add an object in the vector of added objects.
     * @param[in]   _pNewObject   : the added object.
     */
    FWCOMED_API void appendAddedObject( ::fwData::Object::sptr _pNewObject );

    /**
     * @brief   Return the vector of the added objects.
     */
    FWCOMED_API ::fwData::Vector::sptr getAddedObjects() const;


    /**
     * @brief       This method add an object in the vector of removed objects.
     * @param[in]   _pOldObject   : the object which will be removed.
     */
    FWCOMED_API void appendRemovedObject( ::fwData::Object::sptr _pOldObject );

    /**
     * @brief   Return the vector of the removed objects.
     */
    FWCOMED_API ::fwData::Vector::sptr getRemovedObjects() const;

private :

    /// Stores removed object from the vector
    ::fwData::Vector::sptr m_removedObjects;

    /// Stores new added object in the vector
    ::fwData::Vector::sptr m_addedObjects;
};

} // namespace fwComEd

#endif //__FWCOMED_VECTORMSG_HPP__

