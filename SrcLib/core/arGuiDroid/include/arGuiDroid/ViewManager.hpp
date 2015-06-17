/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARGUIDROID_VIEWMANAGER_HPP__
#define __ARGUIDROID_VIEWMANAGER_HPP__

#include <boost/signals2.hpp>

#include <jni.h>

#include "arGuiDroid/config.hpp"

namespace arGuiDroid
{

/**
 * @brief   ViewManager is the binding library to the java ViewManager
 * @class   SSlider
 *
 * Service registered details : \n
 * fwServicesRegisterMacro(::fwServices::IService, ::SSlider::SSlider, ::fwData::Object)
 */
class ARGUIDROID_CLASS_API ViewManager
{

public:

    /**
     * @brief Constructor.
     */
    ARGUIDROID_API ViewManager();

    /**
     * @brief Destructor.
     */
    ARGUIDROID_API virtual ~ViewManager();

    /**
     * @brief Update method: This method call the update java mehod to draw a new view
     */
    ARGUIDROID_API void update();

    /**
     * @brief createSlider method: This method binds the slider creation
     */
    ARGUIDROID_API void createSlider(int max);

    /**
     * @brief createSlider method: This method binds the button creation
     */
    ARGUIDROID_API void createButton(std::string label);

};

} // end namespace arGuiDroid

#endif // __ARGUIDROID_VIEWMANAGER_HPP__
