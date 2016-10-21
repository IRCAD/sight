/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMMAND_PAINTCOMMAND_HPP__
#define __FWCOMMAND_PAINTCOMMAND_HPP__

#include "fwCommand/ICommand.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwTools/macros.hpp>

#include <boost/cstdint.hpp>

namespace fwCommand
{

/**
 * @brief Command class to be used by all paint tools.
 */
class FWCOMMAND_CLASS_API PaintCommand : public ICommand
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (PaintCommand)(ICommand), (( )), std::make_shared< PaintCommand > );

    FWCOMMAND_API PaintCommand();

    // Overrides.
    FWCOMMAND_API const boost::uint32_t  getSize() const;

    // Overrides.
    FWCOMMAND_API void apply();

    // Overrides.
    FWCOMMAND_API void unapply();

    // Overrides.
    FWCOMMAND_API const std::string getDescription( void ) const;

    /// Register paint voxel on image
    FWCOMMAND_API void paint( ::fwData::Image::BufferIndexType index, ::fwData::Image::BufferType oldValue,
                              ::fwData::Image::BufferType newValue );


    FWCOMMAND_API void prePaint( ::fwData::Image::IndexType x, ::fwData::Image::IndexType y,
                                 ::fwData::Image::IndexType z );
    FWCOMMAND_API void prePaint( ::fwData::Image::IndexType index );

    FWCOMMAND_API void postPaint();

    FWCOMMAND_API void setImage( ::fwData::Image::sptr image );

private:

    void notifyImageModification();

    /**
     * @brief Typedef for the index containers.
     */
    typedef std::vector< ::fwData::Image::BufferIndexType > IndexContainer;

    /**
     * @brief Typedef for the color containers.
     */
    typedef std::vector< std::pair< ::fwData::Image::BufferType, ::fwData::Image::BufferType > > ColorContainer;

    /**
     * @brief The index container
     */
    IndexContainer m_commandIndexContainer;

    /**
     * @brief The color container
     */
    ColorContainer m_commandColorContainer;

    /// Working image
    ::fwData::Image::wptr m_image;
    /// Helper on Working image
    ::fwDataTools::helper::Image::sptr m_imageHelper;

    ::fwData::Image::IndexType m_currentPrepaintIndex;
    std::vector< ::fwData::Image::BufferType > m_currentPrepaintBuff;

};


} // namespace fwCommand


#endif //__FWCOMMAND_PAINTCOMMAND_HPP__
