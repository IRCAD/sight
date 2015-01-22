/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_HELPER_VECTOR_HPP__
#define __FWCOMED_HELPER_VECTOR_HPP__

#include <fwServices/IService.hpp>

#include <fwData/Vector.hpp>

#include "fwComEd/config.hpp"
#include "fwComEd/VectorMsg.hpp"



namespace fwComEd
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Vector and create in parallel the message to announce this modification.
 * @class  Vector
 * 
 * @date    2007-20013.
 */
class FWCOMED_CLASS_API Vector
{

public :

    /// Constructor. Initialize parameters.
    FWCOMED_API Vector( ::fwData::Vector::wptr vector );

    /// Destrucotr. Do nothing.
    FWCOMED_API ~Vector();

    /**
     * @brief Add an object in the vector.
     * @param[in] _newObject    object to add in the vector
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void add( ::fwData::Object::sptr _newObject );

    /**
     * @brief Remove an object in the vector.
     * @param[in] _oldObject object to remove from the vector.
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void remove( ::fwData::Object::sptr _oldObject );

    /**
     * @brief Clear all objects in the vector.
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void clear();

    /// Send the message of modification
    FWCOMED_API void notify( ::fwServices::IService::sptr _serviceSource, bool notifySource=false );

private :

    ::fwComEd::VectorMsg::sptr m_vectorMsg;
    ::fwData::Vector::wptr m_vector;
};

} // namespace helper
} // namespace fwComEd

#endif // __FWCOMED_HELPER_VECTOR_HPP__
