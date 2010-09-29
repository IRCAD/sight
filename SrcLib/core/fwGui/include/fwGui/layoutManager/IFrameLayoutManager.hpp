/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_
#define _FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_

#include <list>

#include <boost/function.hpp>
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

    /// Defines all possible style for a frame
    typedef enum
    {
        DEFAULT,
        STAY_ON_TOP
     } Style;

     typedef std::string RegistryKeyType;

     class FrameInfo
     {
     public :

         FrameInfo() :
             m_name (""),
             m_minSize (std::make_pair(-1,-1)),
             m_style (DEFAULT)
         {}

         /// Application name.
         std::string                      m_name ;
         /// Application icon.
         ::boost::filesystem::path        m_iconPath;
         /// Application minimum size (min width and min height)
         std::pair< int, int >            m_minSize;
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


    typedef ::boost::function0< void > CloseCallback;
    FWGUI_API virtual void setCloseCallback(CloseCallback fct);

protected:

    FWGUI_API FrameInfo getFrameInfo(){ return m_frameInfo;};

    ::fwGui::fwContainer::sptr m_frame;
    CloseCallback m_closeCallback;

private:

    void defaultCloseCallback();

    /// Save frame configuration definition
    FrameInfo  m_frameInfo;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_IFRAMELAYOUTMANAGERB_HPP_*/


