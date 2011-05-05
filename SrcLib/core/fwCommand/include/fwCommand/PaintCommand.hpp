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
struct FWCOMMAND_CLASS_API PaintCommand : public ICommand
{
    fwCoreClassDefinitionsWithFactoryMacro( (PaintCommand)(ICommand), (( )), ::fwTools::Factory::New< PaintCommand > );

    // Overrides.
    FWCOMMAND_API const boost::uint32_t  getSize() const;

    // Overrides.
    FWCOMMAND_API void apply();

    // Overrides.
    FWCOMMAND_API void unapply();

    // Overrides.
    FWCOMMAND_API const std::string getDescription( void ) const;

    /**
     * @brief Typedef for the voxel index.
     */
    typedef ::boost::uint32_t VoxelIndex;

    /**
     * @brief Typedef for the voxel color.
     */
    typedef ::boost::uint8_t VoxelType;

    /// Register paint voxel on image
    FWCOMMAND_API void paint( VoxelIndex index, VoxelType oldValue, VoxelType newValue );

    FWCOMMAND_API void setImage( ::fwData::Image::sptr  image );


private :

     void notifyImageModification();

    /**
     * @brief Typedef for the index containers.
     */
    typedef std::vector< VoxelIndex > IndexContainer;

    /**
     * @brief Typedef for the color containers.
     */
    typedef std::vector< std::pair<VoxelType,VoxelType> > ColorContainer;

    /**
     * @brief The index container
     */
    IndexContainer  m_commandIndexContainer;

    /**
     * @brief The color container
     */
    ColorContainer  m_commandColorContainer;

    /// Working image
    ::fwData::Image::wptr  m_image;
};


} // namespace command


#endif //#ifndef _FWCOMMAND_PAINTCOMMAND_HPP_
