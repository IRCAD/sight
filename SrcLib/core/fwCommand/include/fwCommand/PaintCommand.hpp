/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMMAND_PAINTCOMMAND_HPP_
#define _FWCOMMAND_PAINTCOMMAND_HPP_

#include <boost/cstdint.hpp>

#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include <fwData/Image.hpp>

#include "fwCommand/ICommand.hpp"

namespace fwCommand
{

/**
 * @brief Command class to be used by all paint tools.
 */
class FWCOMMAND_CLASS_API PaintCommand : public ICommand
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (PaintCommand)(ICommand), (( )), ::fwTools::Factory::New< PaintCommand > );


    // Overrides.
    FWCOMMAND_API const boost::uint32_t  getSize() const;

    // Overrides.
    FWCOMMAND_API void apply();

    // Overrides.
    FWCOMMAND_API void unapply();

    // Overrides.
    FWCOMMAND_API const std::string getDescription( void ) const;

    /// Register paint voxel on image
    FWCOMMAND_API void paint( ::fwData::Image::BufferIndexType index, ::fwData::Image::BufferType oldValue, ::fwData::Image::BufferType newValue );


    FWCOMMAND_API void prePaint( ::fwData::Image::VoxelIndexType x, ::fwData::Image::VoxelIndexType y, ::fwData::Image::VoxelIndexType z );
    FWCOMMAND_API void prePaint( ::fwData::Image::VoxelIndexType index );

    FWCOMMAND_API void postPaint();

    FWCOMMAND_API void setImage( ::fwData::Image::sptr  image );

protected:
    FWCOMMAND_API PaintCommand();

private :

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
    IndexContainer  m_commandIndexContainer;

    /**
     * @brief The color container
     */
    ColorContainer  m_commandColorContainer;

    /// Working image
    ::fwData::Image::wptr         m_image;
    ::fwData::Image::BufferType * m_buffer;

    ::fwData::Image::VoxelIndexType m_currentPrepaintIndex;
    std::vector< ::fwData::Image::BufferType > m_currentPrepaintBuff;

};


} // namespace fwCommand


#endif //#ifndef _FWCOMMAND_PAINTCOMMAND_HPP_
