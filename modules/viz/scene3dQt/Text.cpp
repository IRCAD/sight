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
#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/registry/macros.hpp>

#include <modules/viz/scene3dQt/WindowInteractor.hpp>

#include <OGRE/OgreNode.h>

#include <OgreAxisAlignedBox.h>
#include <QStyle>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D_TEXT(sight::module::viz::scene3dQt::Text, sight::viz::scene3d::IText::REGISTRY_KEY);

//-----------------------------------------------------------------------------

namespace sight::module::viz::scene3dQt
{

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
        m_text.setUnderlyingNodeRect(
            sight::viz::scene3d::helper::Scene::computeBoundingRect(
                m_camera,
                &dynamic_cast<Ogre::SceneNode&>(m_node)
            )
        );
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
                const auto* sceneNode = dynamic_cast<Ogre::SceneNode*>(p.second);
                SIGHT_ASSERT("cast from Ogre::Node to Ogre::SceneNode failed", sceneNode);
                p.first->setUnderlyingNodeRect(
                    sight::viz::scene3d::helper::Scene::computeBoundingRect(
                        m_camera,
                        sceneNode
                    )
                );
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

    m_text = new QLineEdit("", parentWidget);
    m_text->setReadOnly(true);
    m_text->setAttribute(Qt::WA_TransparentForMouseEvents);
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

    m_text->setAlignment(alignment);
    QObject::connect(
        m_text,
        &QLineEdit::textEdited,
        [this](QString text)
        {
            signal<IText::TextEditedSignal>(IText::s_TEXT_EDITED_SIGNAL)->asyncEmit(text.toStdString());
            adjustSize();
        });
    QObject::connect(
        m_text,
        &QLineEdit::editingFinished,
        [this]
        {
            signal<IText::EditingFinishedSignal>(IText::s_EDITING_FINISHED_SIGNAL)->asyncEmit();
        });
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
    QString qText = QString::fromStdString(_text);
    if(m_text->text() == qText)
    {
        return;
    }

    std::optional<int> cursorPosition;
    if(m_text->hasFocus())
    {
        cursorPosition = m_text->cursorPosition();
    }

    m_text->setText(qText);
    if(cursorPosition.has_value())
    {
        m_text->setCursorPosition(*cursorPosition);
    }

    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setPosition(float _x, float _y)
{
    m_position = {Ogre::Vector2(_x, _y), Ogre::Vector2(_x, _y)};
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

void Text::setEditMode(bool editMode)
{
    m_text->setReadOnly(!editMode);
    m_text->setAttribute(Qt::WA_TransparentForMouseEvents, !editMode);
}

//------------------------------------------------------------------------------

void Text::setUnderlyingNodeRect(std::pair<Ogre::Vector2, Ogre::Vector2> rect)
{
    m_position = rect;
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::adjustSize()
{
    QLabel dummyLabel(m_text->text());
    dummyLabel.setStyleSheet(computeStyle());
    dummyLabel.adjustSize();
    m_text->resize(dummyLabel.size());

    QPoint origin;
    if(m_nodeListener != nullptr)
    {
        QRectF positionRect = {QPointF(m_position.first.x, m_position.first.y), QPointF(
                                   m_position.second.x,
                                   m_position.second.y
        )
        };
        int x {};
        if(m_horizontalAlignment == "center")
        {
            x = static_cast<int>(positionRect.center().x() - static_cast<float>(m_text->width()) / 2.F);
        }
        else
        {
            x = static_cast<int>(positionRect.bottomRight().x());
        }

        origin = QPoint(x, static_cast<int>(positionRect.bottom())) / m_text->devicePixelRatioF();
    }
    else
    {
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

        QRect const parentRect {QPoint {0, 0}, m_text->parentWidget()->size()};
        origin = QStyle::alignedRect(Qt::LeftToRight, alignment, m_text->size(), parentRect).topLeft();
    }

    m_text->move(origin);
}

//------------------------------------------------------------------------------

QString Text::computeStyle()
{
    static const QString s_textCss =
        R"(
        * {
            color: %1;
            font-size: %2px;
            font-family: %3;
            background: transparent;
            border: 0;
        }
        QLineEdit:!read-only {
            color: black;
            background: white;
        })";
    return s_textCss.arg(m_textColor).arg(m_fontSize).arg(m_fontFamily);
}

//------------------------------------------------------------------------------

void Text::adjustStyle()
{
    m_text->setStyleSheet(computeStyle());
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
