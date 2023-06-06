/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/viz/scene3dQt/Text.hpp"

#include <core/com/Slot.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <viz/scene3d/helper/Camera.hpp>
#include <viz/scene3d/registry/macros.hpp>

#include <modules/viz/scene3dQt/WindowInteractor.hpp>

#include <OGRE/OgreNode.h>

#include <QStyle>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D_TEXT(sight::module::viz::scene3dQt::Text, sight::viz::scene3d::IText::REGISTRY_KEY);

//-----------------------------------------------------------------------------

namespace sight::module::viz::scene3dQt
{

//-----------------------------------------------------------------------------

class NodeListener : public Ogre::SceneNode::Listener
{
public:

    explicit NodeListener(Text& _text, Ogre::Camera& _camera, Ogre::Node& _node) :
        m_text(_text),
        m_camera(_camera),
        m_node(_node)
    {
    }

    //------------------------------------------------------------------------------

    void nodeUpdated(const Ogre::Node* /*unused*/) override
    {
        const Ogre::Vector2 screenPos = sight::viz::scene3d::helper::Camera::convertWorldSpaceToScreenSpace(
            m_camera,
            m_node._getDerivedPosition()
        );

        m_text.setPosition(screenPos.x, screenPos.y);
    }

    Text& m_text;
    Ogre::Camera& m_camera;
    Ogre::Node& m_node;
};

class CameraListener : public Ogre::SceneNode::Listener
{
public:

    explicit CameraListener(Ogre::Camera& _camera) :
        m_camera(_camera)
    {
    }

    //------------------------------------------------------------------------------

    void nodeUpdated(const Ogre::Node* /*unused*/) override
    {
        std::ranges::for_each(
            m_text,
            [this](auto& p)
            {
                const Ogre::Vector2 screenPos = sight::viz::scene3d::helper::Camera::convertWorldSpaceToScreenSpace(
                    m_camera,
                    p.second->_getDerivedPosition()
                );

                p.first->setPosition(screenPos.x, screenPos.y);
            });
    }

    //------------------------------------------------------------------------------

    void addText(Text* _text, Ogre::Node* _node)
    {
        m_text.emplace_back(std::make_pair(_text, _node));
    }

    //------------------------------------------------------------------------------

    void removeText(Text* _text)
    {
        std::erase_if(m_text, [&](const auto& p){return p.first == _text;});
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool empty() const
    {
        return m_text.empty();
    }

private:

    std::vector<std::pair<Text*, Ogre::Node*> > m_text;
    Ogre::Camera& m_camera;
};

std::map<Ogre::Camera*, CameraListener*> s_cameraListeners;

//------------------------------------------------------------------------------

Text::Text(sight::viz::scene3d::IText::Key /*unused*/, const sight::viz::scene3d::Layer::sptr& _layer)
{
    m_resizeSlot = core::com::newSlot(
        [this](int, int)
        {
            if(m_nodeListener != nullptr)
            {
                m_nodeListener->nodeUpdated({});
            }

            this->adjustSize();
        });
    m_resizeSlot->setWorker(core::thread::getDefaultWorker());

    m_resizeConnection = _layer->signal(sight::viz::scene3d::Layer::s_RESIZE_LAYER_SIG)->connect(m_resizeSlot);

    auto interactor   = _layer->getRenderService()->getInteractorManager();
    auto qtInteractor = sight::module::viz::scene3dQt::WindowInteractor::dynamicCast(interactor);

    auto* parentWidget = qtInteractor->getQtWidget();

    m_text = new QLabel("", parentWidget);
    m_text->show();
}

//------------------------------------------------------------------------------

Text::~Text()
{
    m_resizeConnection.disconnect();

    SIGHT_ASSERT("Node should be detached first with detachFromNode()", m_nodeListener == nullptr);

    delete m_text;
}

//------------------------------------------------------------------------------

void Text::attachToNode(Ogre::SceneNode* _node, Ogre::Camera* _camera)
{
    SIGHT_ASSERT("Camera is null", _camera);
    SIGHT_ASSERT("Node is null", _node);

    m_nodeListener = new NodeListener(*this, *_camera, *_node);
    _node->setListener(m_nodeListener);

    auto itListener = s_cameraListeners.find(_camera);
    if(itListener == s_cameraListeners.end())
    {
        s_cameraListeners[_camera] = new CameraListener(*_camera);
        _camera->getParentSceneNode()->setListener(s_cameraListeners[_camera]);
    }

    s_cameraListeners[_camera]->addText(this, _node);
}

//------------------------------------------------------------------------------

void Text::detachFromNode()
{
    SIGHT_ASSERT("Node is null", m_nodeListener != nullptr);

    auto* camera = &m_nodeListener->m_camera;
    s_cameraListeners[camera]->removeText(this);
    m_nodeListener->m_node.setListener(nullptr);
    delete m_nodeListener;
    m_nodeListener = nullptr;

    if(s_cameraListeners[camera]->empty())
    {
        s_cameraListeners.erase(camera);
    }
}

//------------------------------------------------------------------------------

void Text::setText(const std::string& _text)
{
    m_text->setText(QString::fromStdString(_text));
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setPosition(float _x, float _y)
{
    m_position = Ogre::Vector2(_x, _y);
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setTextColor(const Ogre::ColourValue& _color)
{
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << _color.getAsARGB();
    m_textColor = QString::fromStdString(ss.str());
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setTextColor(const std::string& _color)
{
    m_textColor = QString::fromStdString(_color);
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setVisible(bool _visible)
{
    if(_visible)
    {
        m_text->show();
    }
    else
    {
        m_text->hide();
    }
}

//------------------------------------------------------------------------------

void Text::setTextAlignment(const std::string& _hAlign, const std::string& _vAlign)
{
    m_horizontalAlignment = _hAlign;
    m_verticalAlignment   = _vAlign;
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setFont(const std::string& fontFamily)
{
    m_fontFamily = QString::fromStdString(fontFamily);
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setFontSize(std::size_t _size)
{
    m_fontSize = _size;
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::adjustSize()
{
    m_text->adjustSize();

    Qt::Alignment alignment;
    if(m_horizontalAlignment == "left")
    {
        alignment |= Qt::AlignLeft;
    }
    else if(m_horizontalAlignment == "center")
    {
        alignment |= Qt::AlignHCenter;
    }
    else if(m_horizontalAlignment == "right")
    {
        alignment |= Qt::AlignRight;
    }

    if(m_verticalAlignment == "top")
    {
        alignment |= Qt::AlignTop;
    }
    else if(m_verticalAlignment == "center")
    {
        alignment |= Qt::AlignVCenter;
    }
    else if(m_verticalAlignment == "bottom")
    {
        alignment |= Qt::AlignBottom;
    }

    QPoint origin;
    if(m_nodeListener != nullptr)
    {
        origin = QPoint(static_cast<int>(m_position.x), static_cast<int>(m_position.y)) / m_text->devicePixelRatioF();
    }
    else
    {
        QRect const parentRect {QPoint {0, 0}, m_text->parentWidget()->size()};
        origin = QStyle::alignedRect(Qt::LeftToRight, alignment, m_text->size(), parentRect).topLeft();
    }

    m_text->move(origin);
}

//------------------------------------------------------------------------------

void Text::adjustStyle()
{
    static QString s_textCss = ".QLabel { color: %1; font-size: %2px; font-family: %3; background-color: none;} " \
                               ".QWidget { background-color: none;}";
    m_text->setStyleSheet(s_textCss.arg(m_textColor).arg(m_fontSize).arg(m_fontFamily));
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
