/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

/**
 * @brief The namespace styleQml contains the basic Qml types to uniformize the
 * style of all the applications using Qml.
 */
namespace styleQml
{
/**
 * @page  styleQml Qml style
 *
 * @tableofcontents
 *
 * styleQml bundle contains the basic services to run a Qml application and style the application.
 *
 * The style in Qml is located inside styleQml.
 *
 * @section Singleton Material Style
 *
 * The most recent style that exist for Qt is the Material Style.
 * To setup Material for all projects, inside styleQml we apply it using QQuickStyle:
 *`QQuickStyle::setStyle("Material");`
 * We can then customise the style with the different variable:
 * - theme (light or dark)
 * - foreground: the Material color for the font
 * - background: the Material color for the background of the Window
 * - accent: the Material color use for highlight of different Qml Object
 * - primary: the Material color that you can apply on different element
 * - elevation: the size of elements like button for example
 *
 * @subsection SingletonQml Material Variable
 *
 * The variable that we talked above, only apply on one window. For example if you create a dialog, your theme will not
 * be apply but only the default theme with the default colour.
 * To overcome this problem, we came up with a Qml solution.
 * We created a Singleton qml file using `pragma Singleton` that import QtQuick 2.9 and QtQuick.Controls.Material 2.3.
 * Inside this Singleton a QtObject is initialized to store variable that can be used in each Qml file. We can find in
 * this object all variable from Material that we talked in the previous paragraph.
 *
 * @subsubsection SingletonQmlDir QmlDir
 *
 * Even if we have setup the Singleton we need to import this Qml.
 * It is made in the qmldir in the subfolder rc/styleQml of the bundle styleQml. This line: `singleton Theme 1.0
 * Material.qml` import the singleton and set the name Theme to use it.
 * To resume, you need to write Theme + the name of the Material variable: `Theme.foreground`.
 *
 * @subsubsection SingletonUse How to use the Singleton
 *
 * Inside the `ui.qml` ApplicationWindow you need to initialize the Singleton. Each Qml Element is a Component so you
 * can know when the Element has been instantied.
 * So we need to write our code about the Singleton inside `Component.onCompleted:{}`:
 *
 *      Theme.theme = Material.Light;
 *      Theme.foreground = Material.Teal; // you need to initialize each variable of the Theme
 *      Material.theme = Theme.theme; // set the theme for the main Window
 *      Material.foreground = Theme.foreground; // you need to set the Material variable inside each window in
 *      //Component.onCompleted
 *
 * @subsection ComponentQml Component Customisation
 *
 * The Material theme is generic so the primary color is never use and some control don't have style.
 * To get a full Material theme user friendly we need to use sometimes primary color and to create the style for
 * controlers who don't have one.
 * Each control has his own Qml file inside rc/styleQml. E.g if the Button.qml is change to customize the style, it will
 * apply the style in all qml where you have imported the module.
 *
 * @subsubsection ComponentQmlDir QmlDir
 *
 * Even if we have setup each Controler we need to use all Qml into one import.
 * Like the singleton, it is manage inside the qmldir. `module styleQml` regroup all Component inside the module, so we
 * just need to import styleQml 1.0 inside each Qml you create.
 *
 * @subsubsection ComponentUse How to use all Customized Component
 *
 * To override the default Style and the Style made by QtQuick.Controls, we need to make this import at the end. Each
 * control is override via the name that is the same so we don't need to do more than that.
 */
}
