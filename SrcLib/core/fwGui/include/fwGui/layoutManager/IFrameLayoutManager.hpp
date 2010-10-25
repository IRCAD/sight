/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/IFrameLayoutManager.hpp
 * @brief This file defines the interface of the base class for managing a frame.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#ifndef _FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_
#define _FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_

#include <list>

#include <boost/function.hpp>
#include <boost/filesystem/path.hpp>

#include <fwCore/base.hpp>
#include <fwData/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/container/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
/**
 * @brief       The namespace fwGui::layoutManager contains the base interface for GUI elements managing layout.
 * @namespace   fwGui::layoutManager
 * @author      IRCAD (Research and Development Team).
 * @date        2009-2010.
 *
 */
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

    /// Defines all possible style for a frame
    typedef enum
    {
        DEFAULT,
        STAY_ON_TOP
     } Style;

     typedef std::string RegistryKeyType;

     static const std::string SOFTWARE_UI;
     static const std::string FRAME_STATE_UI;
     static const std::string FRAME_SIZE_W_UI;
     static const std::string FRAME_SIZE_H_UI;
     static const std::string FRAME_POSITION_X_UI;
     static const std::string FRAME_POSITION_Y_UI;

     typedef enum
     {
         UNKNOWN,    ///< the unknown state
         ICONIZED,   ///< the minimized state
         MAXIMIZED,  ///< the maximied state
         FULL_SCREEN ///< the full screen state
     } FrameState;

     class FrameInfo
     {
     public :

         FrameInfo() :
             m_name (""),
             m_minSize (std::make_pair(-1,-1)),
             m_style (DEFAULT),
             m_size (std::make_pair(-1,-1)),
             m_position (std::make_pair(-1,-1)),
             m_state(UNKNOWN)
         {}

         /// Frame name.
         std::string                      m_name ;
         /// Frame icon.
         ::boost::filesystem::path        m_iconPath;
         /// Frame minimum size (min width and min height)
         std::pair< int, int >            m_minSize;
         /// Frame style
         Style                            m_style;
         /// Frame size
         std::pair< int, int >            m_size;
         /// Frame position
         std::pair< int, int >            m_position;
         /// Frame state (maximize, minized, full screen)
         FrameState                       m_state;
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

    FWGUI_API virtual ::fwGui::container::fwContainer::sptr getFrame(){ return m_frame;};


    typedef ::boost::function0< void > CloseCallback;
    FWGUI_API virtual void setCloseCallback(CloseCallback fct);

protected:

    fwGettersSettersDocMacro(FrameInfo, frameInfo, FrameInfo, configuration definition.);

    ::fwGui::container::fwContainer::sptr m_frame;
    CloseCallback m_closeCallback;

    FWGUI_API void readConfig();

    FWGUI_API void writeConfig();

    FWGUI_API ::fwData::Composite::sptr getPreferenceUI();

private:


    void defaultCloseCallback();

    /// Save frame configuration definition
    FrameInfo  m_frameInfo;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_*/


