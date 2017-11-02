/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_FRAMETL_HPP__
#define __ARDATA_FRAMETL_HPP__

#include "arData/GenericTL.hpp"
#include "arData/GenericTL.hxx"
#include "arData/timeline/GenericObject.hpp"
#include "arData/timeline/GenericObject.hxx"

#include <fwData/factory/new.hpp>

#include <fwTools/Type.hpp>

fwCampAutoDeclareDataMacro((arData)(FrameTL), ARDATA_API);

namespace arData
{
/**
 * @brief   This class defines a timeline of images.
 */
class ARDATA_CLASS_API FrameTL : public GenericTL< uint8_t >
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (FrameTL)(::arData::TimeLine), (()), ::fwData::factory::New< FrameTL >)
    fwCampMakeFriendDataMacro((arData)(FrameTL))

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API FrameTL( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~FrameTL();

    /// Defines deep copy
    ARDATA_API virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Initializes the size of the pool buffer.
    ARDATA_API void initPoolSize(size_t width, size_t height,
                                 const ::fwTools::Type& type, size_t numberOfComponents = 1,
                                 unsigned int maxElementNum = 1);

    /// Returns the width of an image in the timeline
    size_t getWidth() const
    {
        return m_width;
    }

    /// Returns the height of an image in the timeline
    size_t getHeight() const
    {
        return m_height;
    }

    /// Returns the number of components of an image in the timeline
    size_t getNumberOfComponents() const
    {
        return m_numberOfComponents;
    }

    /// Returns the type of the frame pixel
    ::fwTools::Type getType() const
    {
        return m_type;
    }

private:

    /// Forbid the use of this inherited method.
    ARDATA_API virtual void initPoolSize(unsigned int maxElementNum) override;

    /// frame width
    size_t m_width;

    /// frame height
    size_t m_height;

    /// number of components
    size_t m_numberOfComponents;

    /// type of frame pixel
    ::fwTools::Type m_type;

}; // class FrameTL

} // namespace arData

#endif // __ARDATA_FRAMETL_HPP__
