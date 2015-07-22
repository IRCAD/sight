/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_FRAMETL_HPP__
#define __EXTDATA_FRAMETL_HPP__

#include <fwData/factory/new.hpp>
#include <fwTools/Type.hpp>

#include "extData/GenericTL.hpp"
#include "extData/GenericTL.hxx"
#include "extData/timeline/GenericObject.hpp"
#include "extData/timeline/GenericObject.hxx"

fwCampAutoDeclareDataMacro((extData)(FrameTL), EXTDATA_API);

namespace extData
{
/**
 * @class   FrameTL
 * @brief   This class defines a timeline of images.
 */
class EXTDATA_CLASS_API FrameTL : public GenericTL< ::boost::uint8_t >
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (FrameTL)(::extData::TimeLine),(()),::fwData::factory::New< FrameTL >);
    fwCampMakeFriendDataMacro((extData)(FrameTL));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    EXTDATA_API FrameTL( ::fwData::Object::Key key );

    /// Destructor
    EXTDATA_API virtual ~FrameTL();

    /// Defines deep copy
    EXTDATA_API virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /// Initializes the size of the pool buffer.
    EXTDATA_API void initPoolSize(size_t width, size_t height,
                                  const ::fwTools::Type &type, size_t numberOfComponents = 1,
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
    EXTDATA_API virtual void initPoolSize(unsigned int maxElementNum);

    /// frame width
    size_t m_width;

    /// frame height
    size_t m_height;

    /// number of components
    size_t m_numberOfComponents;

    /// type of frame pixel
    ::fwTools::Type m_type;

}; // class FrameTL

} // namespace extData

#endif // __EXTDATA_FRAMETL_HPP__
