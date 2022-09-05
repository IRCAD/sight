/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "modules/viz/scene3dTest/Plugin.hpp"

#include <viz/scene3d/Utils.hpp>
#include <viz/scene3d/WindowManager.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreTextureManager.h>

#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>

namespace sight::module::viz::scene3d::test
{

SIGHT_REGISTER_PLUGIN("sight::module::viz::scene3d::test::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void Plugin::start()
{
    m_ogreRoot = sight::viz::scene3d::Utils::getOgreRoot();

    // Don't output the log to the terminal and delete the file when the test is done.
    Ogre::LogManager* logMgr = Ogre::LogManager::getSingletonPtr();
    logMgr->createLog("OgreTest.log", true, false, true);

    // Set up context before running a test.
    static std::string arg1 = "OgreTest";
    std::array argv         = {arg1.data(), static_cast<char*>(nullptr)};
    int argc                = 1;
    QGuiApplication a(argc, argv.data());

    auto surface = std::make_unique<QOffscreenSurface>();

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setMajorVersion(4);
    surfaceFormat.setMinorVersion(3);
    surface->setFormat(surfaceFormat);
    surface->create();

    auto* glContext = new QOpenGLContext();
    glContext->setFormat(surfaceFormat);

    glContext->create();
    SIGHT_ASSERT("Unable to create context", glContext->isValid());

    glContext->makeCurrent(surface.get());

    Ogre::NameValuePairList parameters;
    parameters["currentGLContext"] = "true";

    // This is needed for the TextureManager to be instanced, no better way has be found.
    auto* ogreRoot = sight::viz::scene3d::Utils::getOgreRoot();

    // Use a size > 120 because windows will anyway switch to a larger size
    auto* window = ogreRoot->createRenderWindow(
        "test",
        static_cast<unsigned int>(200),
        static_cast<unsigned int>(200),
        false,
        &parameters
    );
    sight::viz::scene3d::WindowManager::sptr mgr = sight::viz::scene3d::WindowManager::get();
    mgr->add(window);

    window->setVisible(false);
    window->setAutoUpdated(false);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    m_ogreRoot = nullptr;
    sight::viz::scene3d::Utils::destroyOgreRoot();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::test
