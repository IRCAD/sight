/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_
#define _FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_

#include <list>
#include <boost/filesystem/path.hpp>

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

namespace layoutManager
{

/**
 * @brief   Defines the frame layout manager.
 * @class   IFrameLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IFrameLayoutManager : public ::fwCore::BaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IFrameLayoutManager)(::fwCore::BaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Defines all possible orientation for a LineLayout
    typedef enum
    {
        VERTICAL,
        HORIZONTAL
     } Style;

     typedef std::string RegistryKeyType;

     class FrameInfo
     {
     public :

         FrameInfo() :
             m_name (""),
             m_minSize (std::make_pair(-1,-1))
         {}

         /// Application name.
         std::string                      m_name ;
         /// Application icon.
         ::boost::filesystem::path        m_iconPath;
         /// Application minimum size (min width and min height)
         std::pair< int, int >            m_minSize;
         /// Application minimum width.
         /// Style
         Style                            m_style;
     };

    /// Constructor. Do nothing.
    FWGUI_API IFrameLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IFrameLayoutManager();

    /**
     * @brief Configure the layout before creation.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    /**
     * @brief Instantiate frame.
     */
    FWGUI_API virtual void createFrame() = 0;

    /**
     * @brief Destroy frame.
     */
    FWGUI_API virtual void destroyFrame() = 0;

    FWGUI_API virtual ::fwGui::fwContainer::sptr getFrame(){ return m_frame;};

protected:

    FWGUI_API FrameInfo getFrameInfo(){ return m_frameInfo;};

    ::fwGui::fwContainer::sptr m_frame;

private:

    /// Save frame configuration definition
    FrameInfo  m_frameInfo;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_*/


