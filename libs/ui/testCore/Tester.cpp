/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
// cspell:ignore qputenv

#include "Tester.hpp"

#include <core/spy_log.hpp>

#include <ui/__/preferences.hpp>

#include <boost/algorithm/string.hpp>

#include <QAction>
#include <QApplication>
#include <QMutex>
#include <QScreen>
#include <QTest>
#include <QTimer>

#include <cmath>
#include <iostream>

namespace sight::ui::test_core
{

//------------------------------------------------------------------------------

bool Tester::s_alreadyLoaded                    = false;
std::filesystem::path Tester::s_imageOutputPath = std::filesystem::temp_directory_path();

//------------------------------------------------------------------------------

TestEvent::TestEvent(std::function<void()> _f) :
    QEvent(QEvent::User),
    m_function(std::move(_f))
{
}

//------------------------------------------------------------------------------

std::function<void()> TestEvent::function()
{
    return m_function;
}

//------------------------------------------------------------------------------

bool TestEventFilter::eventFilter(QObject* _obj, QEvent* _event)
{
    if(_event->type() == QEvent::User)
    {
        auto* test_event = dynamic_cast<TestEvent*>(_event);
        // the event catched here might not be a TestEvent, in which case, testEvent will be nullptr
        if(test_event != nullptr)
        {
            test_event->function()();
            return true;
        }
    }

    return QObject::eventFilter(_obj, _event);
}

TesterAssertionFailed::TesterAssertionFailed(const std::string& _msg) :
    std::runtime_error(_msg)
{
}

//------------------------------------------------------------------------------

bool always_true(QObject* /*unused*/)
{
    return true;
}

Tester::BacktraceLock::BacktraceLock(Tester& _tester) :
    m_tester(_tester)
{
}

Tester::BacktraceLock::~BacktraceLock()
{
    // If stack unwinding is in progress, it means the backtrace will be useful, so we shouldn't clear it
    if(std::uncaught_exceptions() == 0)
    {
        m_tester.m_backtrace.pop_back();
    }
}

Tester::Tester(std::string _test_name, bool _verbose_mode) :
    M_TEST_NAME(std::move(_test_name)),
    m_verboseMode(_verbose_mode)
{
}

Tester::~Tester()
{
    if(m_thread.joinable())
    {
        m_thread.join();
    }
}

//------------------------------------------------------------------------------

void Tester::take(
    const std::string& _component_description,
    std::function<QObject* ()> _graphic_component,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    take<QObject*>(_component_description, _graphic_component, _condition, _timeout);
}

//------------------------------------------------------------------------------

void Tester::take(const std::string& _component_description, QObject* _graphic_component)
{
    m_componentDescription = _component_description;
    m_graphicComponent     = _graphic_component;
}

//------------------------------------------------------------------------------

void Tester::take(
    const std::string& _component_description,
    QObject* _parent,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    take(
        _component_description,
        [_parent, &_object_name]
        {
            return _parent->findChild<QObject*>(QString::fromStdString(_object_name));
        },
        _condition,
        _timeout
    );
}

//------------------------------------------------------------------------------

void Tester::take(
    const std::string& _component_description,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    take(_component_description, getMainWindow(), _object_name, _condition, _timeout);
}

//------------------------------------------------------------------------------

void Tester::interact(std::unique_ptr<Interaction>&& _interaction)
{
    if(m_graphicComponent == nullptr)
    {
        return;
    }

    QWidget* widget = qobject_cast<QWidget*>(m_graphicComponent);
    QWindow* window = qobject_cast<QWindow*>(m_graphicComponent);
    if(widget == nullptr && window == nullptr)
    {
        fail("Tried to interact with \"" + m_componentDescription + "\", which is neither a widget nor a window.");
    }

    if(m_verboseMode)
    {
        qDebug() << _interaction->toString().c_str() << "on" << QString::fromStdString(m_componentDescription);
    }

    if(widget != nullptr)
    {
        _interaction->interactWith(widget);
    }
    else if(window != nullptr)
    {
        _interaction->interactWith(window);
    }

    m_interactions.push_back(std::move(_interaction));
}

//------------------------------------------------------------------------------

void Tester::doubt(const std::string& _result_description, std::function<bool(QObject*)> _result, int _timeout)
{
    doubt<QObject*>(_result_description, _result, _timeout);
}

//------------------------------------------------------------------------------

void Tester::yields(
    const std::string& _component_description,
    std::function<QObject* (QObject*)> _graphic_component,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    yields<QObject*>(_component_description, _graphic_component, _condition, _timeout);
}

//------------------------------------------------------------------------------

void Tester::yields(
    const std::string& _component_description,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    yields<QObject*>(_component_description, _object_name, _condition, _timeout);
}

//------------------------------------------------------------------------------

void Tester::maybeTake(
    const std::string& _component_description,
    std::function<QObject* ()> _graphic_component,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    maybeTake<QObject*>(_component_description, _graphic_component, _condition, _timeout);
}

//------------------------------------------------------------------------------

void Tester::doSomething(std::function<void(QObject*)> _f)
{
    doSomething<QObject*>(_f);
}

//------------------------------------------------------------------------------

void Tester::doSomethingAsynchronously(std::function<void(QObject*)> _f)
{
    doSomethingAsynchronously<QObject*>(_f);
}

//------------------------------------------------------------------------------

void Tester::takeScreenshot(const std::filesystem::path& _path)
{
    if(get<QWidget*>()->windowHandle() != nullptr)
    {
        get<QWidget*>()->screen()->grabWindow(get<QWidget*>()->winId()).save(QString::fromStdString(_path.string()));
    }
    else
    {
        doSomethingAsynchronously<QWidget*>(
            [_path](QWidget* _o)
            {
                _o->grab().save(QString::fromStdString(_path.string()));
            });
    }
}

//------------------------------------------------------------------------------

void Tester::start(std::function<void()> _f)
{
    m_thread = std::thread(
        [this, _f]() -> void
        {
            try
            {
                if(m_verboseMode)
                {
                    qDebug() << "Waiting up to 5000 ms for the main window to appear";
                }

                bool ok = waitForAsynchronously(
                    [this]() -> bool
                {
                    m_mainWindow = qApp->activeWindow();
                    return m_mainWindow != nullptr && m_mainWindow->isVisible();
                },
                    DEFAULT_TIMEOUT
                );
                if(!ok)
                {
                    fail("The main window has never showed up");
                }

                QTest::qWait(2000); // Temporize, because the window takes time to effectively show up
                _f();
                qApp->postEvent(
                    qApp,
                    new TestEvent(
                        [this]
                {
                    m_mainWindow->close();
                })
                );
                if(m_verboseMode)
                {
                    qDebug() << "Waiting up to 5000 ms for the main window to close";
                }

                ok = QTest::qWaitFor(
                    [this]() -> bool
                {
                    return qApp->activeWindow() == nullptr || !m_mainWindow->isVisible();
                },
                    DEFAULT_TIMEOUT
                );
                if(!ok)
                {
                    fail("The main window never closed");
                }
            }
            catch(TesterAssertionFailed& e)
            {
                m_failureMessage = generateFailureMessage() + "Failure: " + e.what();
                m_failed         = true;
                if(m_verboseMode)
                {
                    qDebug() << m_failureMessage.c_str();
                }

                if(m_mainWindow != nullptr)
                {
                    m_mainWindow->screen()->grabWindow(0).save(
                        QString::fromStdString(
                            (s_imageOutputPath
                             / (M_TEST_NAME + "_failure.png")).string()
                        )
                    );
                }

                qApp->exit(1);

                // If the application hasn't closed after 5 seconds, force-quit.
                bool ok = QTest::qWaitFor(
                    [this]() -> bool
                {
                    return qApp->activeWindow() == nullptr || m_mainWindow == nullptr || !m_mainWindow->isVisible();
                });
                if(!ok)
                {
                    qDebug() << "The application didn't close in 5 seconds, force-quitting.";
                    std::exit(1);
                }
            }
            catch(std::exception& e)
            {
                m_failureMessage = generateFailureMessage() + "Unexpected error: " + e.what();
                m_failed         = true;
                if(m_verboseMode)
                {
                    qDebug() << m_failureMessage.c_str();
                }

                qApp->exit(1);

                // If the application hasn't closed after 5 seconds, force-quit.
                bool ok = QTest::qWaitFor(
                    [this]() -> bool
                {
                    return qApp->activeWindow() == nullptr || m_mainWindow == nullptr || !m_mainWindow->isVisible();
                });
                if(!ok)
                {
                    qDebug() << "The application didn't close in 5 seconds, force-quitting.";
                    std::exit(1);
                }
            }
        });
}

//------------------------------------------------------------------------------

void Tester::shouldBeHidden(
    const std::string& _component_description,
    std::function<QWidget* ()> _graphic_component,
    std::function<bool(QWidget*)> _condition,
    int _timeout
)
{
    maybeTake<QWidget*>(
        _component_description,
        _graphic_component,
        [&_condition](QWidget* _obj)
        {
            return _condition(_obj) && !_obj->isVisible();
        },
        _timeout
    );
    if(exists())
    {
        fail('"' + _component_description + "\" is present, though it should be hidden");
    }
}

//------------------------------------------------------------------------------

void Tester::shouldBePresent(
    const std::string& _component_description,
    std::function<QWidget* ()> _graphic_component,
    std::function<bool(QWidget*)> _condition,
    int _timeout
)
{
    take<QWidget*>(
        _component_description,
        _graphic_component,
        [&_condition](QWidget* _obj)
        {
            return _condition(_obj) && _obj->isVisible();
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

bool Tester::exists()
{
    return m_graphicComponent != nullptr;
}

//------------------------------------------------------------------------------

QWidget* Tester::getMainWindow()
{
    return m_mainWindow;
}

//------------------------------------------------------------------------------

std::string Tester::getFailureMessage() const
{
    return m_failureMessage;
}

//------------------------------------------------------------------------------

bool Tester::failed() const
{
    return m_failed;
}

//------------------------------------------------------------------------------

Tester::BacktraceLock Tester::addInBacktrace(const std::string& _description)
{
    m_backtrace.push_back(_description);
    return {*this};
}

//------------------------------------------------------------------------------

std::string Tester::getDescription() const
{
    return m_componentDescription;
}

//------------------------------------------------------------------------------

void Tester::init()
{
    if(!s_alreadyLoaded)
    {
        qApp->installEventFilter(new TestEventFilter);
        qputenv("GUI_TESTS_ARE_RUNNING", "1");
        const std::string image_output_path = qEnvironmentVariable("IMAGE_OUTPUT_PATH").toStdString();
        if(!image_output_path.empty())
        {
            s_imageOutputPath = image_output_path;
        }

        sight::ui::preferences::ignoreFilesystem(true);

        s_alreadyLoaded = true;

    #ifdef WIN32
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    #endif
    }
}

//------------------------------------------------------------------------------

QWidget* Tester::getWidgetFromAction(QAction* _action)
{
    return _action != nullptr ? _action->associatedWidgets().last() : nullptr;
}

//------------------------------------------------------------------------------

void Tester::mouseMove(
    QWidget* _widget,
    QPoint _pos,
    int _delay,
    Qt::MouseButton _button,
    Qt::KeyboardModifiers _modifiers
)
{
    // Workaround to QTBUG-5232
    if(_delay > 0)
    {
        QTest::qWait(_delay);
    }

    if(_pos.isNull())
    {
        _pos = _widget->rect().center();
    }

    QMouseEvent event(QEvent::MouseMove, _pos, _widget->mapToGlobal(_pos), Qt::NoButton, _button, _modifiers);
    qApp->sendEvent(_widget, &event);
}

//------------------------------------------------------------------------------

void Tester::mouseMove(
    QWindow* _window,
    QPoint _pos,
    int _delay,
    Qt::MouseButton _button,
    Qt::KeyboardModifiers _modifiers
)
{
    // Workaround to QTBUG-5232
    if(_delay > 0)
    {
        QTest::qWait(_delay);
    }

    if(_pos.isNull())
    {
        _pos = _window->geometry().center();
    }

    QMouseEvent event(QEvent::MouseMove, _pos, _window->mapToGlobal(_pos), Qt::NoButton, _button, _modifiers);
    qApp->sendEvent(_window, &event);
}

//------------------------------------------------------------------------------

double Tester::compareImagesPixelPerfect(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size);
        _b = _b.scaled(size);
    }

    int identical_pixels = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            identical_pixels += static_cast<int>(_a.pixelColor(x, y) == _b.pixelColor(x, y));
        }
    }

    return static_cast<double>(identical_pixels) / (_a.height() * _a.width());
}

//------------------------------------------------------------------------------

double Tester::compareImagesMSE(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _b = _b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double res = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            const QColor ca = _a.pixelColor(x, y);
            const QColor cb = _b.pixelColor(x, y);
            res += std::pow(ca.red() - cb.red(), 2) + std::pow(ca.green() - cb.green(), 2) + std::pow(
                ca.blue() - cb.blue(),
                2
            );
        }
    }

    return 1 - (res / (_a.height() * _a.width())) / (3 * std::pow(255, 2));
}

//------------------------------------------------------------------------------

double Tester::compareImagesCosine(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _b = _b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double pa = 0;
    double pb = 0;
    double ab = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            const QColor ca = _a.pixelColor(x, y);
            int ra          = ca.red() + 1;
            int ga          = ca.green() + 1;
            int ba          = ca.blue() + 1;
            const QColor cb = _b.pixelColor(x, y);
            int rb          = cb.red() + 1;
            int gb          = cb.green() + 1;
            int bb          = cb.blue() + 1;
            pa += std::pow(ra, 2) + std::pow(ga, 2) + std::pow(ba, 2);
            pb += std::pow(rb, 2) + std::pow(gb, 2) + std::pow(bb, 2);
            ab += (ra * rb) + (ga * gb) + (ba * bb);
        }
    }

    return std::pow(ab, 2) / (pa * pb);
}

//------------------------------------------------------------------------------

double Tester::compareImagesHistogram(const QImage& _x, const QImage& _y)
{
    const QVector<QVector<QVector<double> > > histogram_a = computeHistogram(_x);
    const QVector<QVector<QVector<double> > > histogram_b = computeHistogram(_y);
    double res                                            = 0;
    for(int r = 0 ; r < 256 ; r++)
    {
        for(int g = 0 ; g < 256 ; g++)
        {
            for(int b = 0 ; b < 256 ; b++)
            {
                double addition = histogram_a[r][g][b] + histogram_b[r][g][b];
                if(addition > 0)
                {
                    res += std::pow(histogram_a[r][g][b] - histogram_b[r][g][b], 2) / addition;
                }
            }
        }
    }

    return 1 - (res / 2);
}

//------------------------------------------------------------------------------

double Tester::compareImagesCorrelation(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _b = _b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double res = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            const QColor ca = _a.pixelColor(x, y);
            const QColor cb = _b.pixelColor(x, y);
            res += std::pow(ca.red() - cb.red(), 2) + std::pow(ca.green() - cb.green(), 2) + std::pow(
                ca.blue() - cb.blue(),
                2
            );
        }
    }

    const int size = _a.height() * _a.width();
    return 1 - (6 * res) / (std::pow(size, 2) - 1);
}

//------------------------------------------------------------------------------

double Tester::compareImagesVoodoo(const QImage& _a, const QImage& _b)
{
    const QImage av = voodooize(_a);
    const QImage bv = voodooize(_b);
    return compareImagesMSE(av, bv);
}

//------------------------------------------------------------------------------

void Tester::fail(const std::string& _message)
{
    throw TesterAssertionFailed(_message);
}

//------------------------------------------------------------------------------

std::string Tester::generateFailureMessage()
{
    std::string message;
    if(m_graphicComponent != nullptr)
    {
        message += "GIVEN: " + m_componentDescription + '\n';
    }

    if(!m_interactions.empty())
    {
        message += "WHEN: ";
        for(size_t i = 0 ; i < m_interactions.size() ; i++)
        {
            if(i != 0)
            {
                message += ", ";
            }

            message += m_interactions[i]->toString();
        }

        message += '\n';
    }

    if(!m_resultDescription.empty())
    {
        message += "THEN: " + m_resultDescription + '\n';
    }

    if(!m_backtrace.empty())
    {
        message += "BACKTRACE: " + boost::join(m_backtrace, " / ") + '\n';
    }

    return message;
}

//------------------------------------------------------------------------------

QVector<QVector<QVector<double> > > Tester::computeHistogram(const QImage& _img)
{
    QVector<QVector<QVector<double> > > histogram(256, QVector(256, QVector(256, 0.0)));
    for(int y = 0 ; y < _img.height() ; y++)
    {
        for(int x = 0 ; x < _img.width() ; x++)
        {
            QColor color = _img.pixelColor(x, y);
            histogram[color.red()][color.green()][color.blue()]++;
        }
    }

    for(int r = 0 ; r < 256 ; r++)
    {
        for(int g = 0 ; g < 256 ; g++)
        {
            for(int b = 0 ; b < 256 ; b++)
            {
                histogram[r][g][b] /= _img.width() * _img.height();
            }
        }
    }

    return histogram;
}

//------------------------------------------------------------------------------

QImage Tester::normalize(QImage _img)
{
    int lightest_red   = -1;
    int lightest_green = -1;
    int lightest_blue  = -1;
    int darkest_red    = 256;
    int darkest_green  = 256;
    int darkest_blue   = 256;
    for(int y = 0 ; y < _img.height() ; y++)
    {
        for(int x = 0 ; x < _img.width() ; x++)
        {
            QColor color = _img.pixelColor(x, y);

            lightest_red   = std::max(lightest_red, color.red());
            darkest_red    = std::min(darkest_red, color.red());
            lightest_green = std::max(lightest_green, color.green());
            darkest_green  = std::min(darkest_green, color.green());
            lightest_blue  = std::max(lightest_blue, color.blue());
            darkest_blue   = std::min(darkest_blue, color.blue());
        }
    }

    // If the lightest and darkest color are the same, don't modify the color
    if(lightest_red == darkest_red)
    {
        darkest_red  = 0;
        lightest_red = 255;
    }

    if(lightest_green == darkest_green)
    {
        darkest_green  = 0;
        lightest_green = 255;
    }

    if(lightest_blue == darkest_blue)
    {
        darkest_blue  = 0;
        lightest_blue = 255;
    }

    for(int y = 0 ; y < _img.height() ; y++)
    {
        for(int x = 0 ; x < _img.width() ; x++)
        {
            QColor color = _img.pixelColor(x, y);
            color.setRed((color.red() - darkest_red) * (255 / (lightest_red - darkest_red)));
            color.setGreen((color.green() - darkest_green) * (255 / (lightest_green - darkest_green)));
            color.setBlue((color.blue() - darkest_blue) * (255 / (lightest_blue - darkest_blue)));
            _img.setPixelColor(x, y, color);
        }
    }

    return _img;
}

//------------------------------------------------------------------------------

QImage Tester::voodooize(const QImage& _img)
{
    QImage res = _img.scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    res = normalize(res);
    // We must put the lightest edge in the top left, the next next darker edge to the top right, the next one to the
    // bottom left, etc
    for(int i = 0 ; i < 2 ; i++)
    {
        int lightest                = 0;
        int edge                    = 0;
        const QVector<QColor> edges =
        {res.pixelColor(0, 0), res.pixelColor(63, 0), res.pixelColor(0, 63), res.pixelColor(
             63,
             63
        )
        };
        for(int j = i ; j < 4 ; j++)
        {
            int light = edges[i].red() + edges[i].green() + edges[i].blue();
            if(light > lightest)
            {
                lightest = light;
                edge     = i;
            }
        }

        // First edge:
        //  - Top left: Do nothing
        //  - Top right: Flip horizontally
        //  - Bottom left: Flip vertically
        //  - Bottom right: Flip both horizontally and vertically
        // Second edge:
        //  - Top left: Impossible (the first edge is there)
        //  - Top right: Do nothing
        //  - Bottom left: Flip vertically and rotate to the right
        //  - Bottom right: Impossible to put it to the right place
        res = res.mirrored((i == 0 && ((edge & 1) != 0)), (edge == 2 || (i == 0 && edge == 3)));
        if(i == 1 && edge == 2)
        {
            res = res.transformed(QTransform().rotate(-90));
        }
    }

    return res;
}

//------------------------------------------------------------------------------

bool Tester::waitForAsynchronously(std::function<bool()> _predicate, int _timeout)
{
    return QTest::qWaitFor(
        [_predicate]() -> bool
        {
            QMutex mutex;
            mutex.lock(); // Lock the mutex by default
            bool ok = false;
            qApp->postEvent(
                qApp,
                new TestEvent(
                    [&mutex, &ok, _predicate]
            {
                ok = _predicate();
                mutex.unlock(); // Unlock the mutex for the calling thread to be unlocked
            })
            );
            mutex.lock();   // Wait for the main thread to execute the predicate
            mutex.unlock(); // We must unlock it, as destroying a locked mutex is undefined behavior
            return ok;
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

QPoint Tester::centerOf(const QWidget* _widget)
{
    return _widget->rect().center();
}

//------------------------------------------------------------------------------

QPoint Tester::leftOf(const QWidget* _widget)
{
    return {1, centerOf(_widget).y()};
}

//------------------------------------------------------------------------------

QPoint Tester::rightOf(const QWidget* _widget)
{
    return {_widget->width() - 1, centerOf(_widget).y()};
}

//------------------------------------------------------------------------------

QPoint Tester::topOf(const QWidget* _widget)
{
    return {centerOf(_widget).x(), 1};
}

//------------------------------------------------------------------------------

QPoint Tester::bottomOf(const QWidget* _widget)
{
    return {centerOf(_widget).x(), _widget->height() - 1};
}

//------------------------------------------------------------------------------

std::filesystem::path Tester::getImageOutputPath()
{
    return s_imageOutputPath;
}

//------------------------------------------------------------------------------

QTouchDevice* Tester::getDummyTouchScreen()
{
    static QTouchDevice* res = nullptr;
    if(res == nullptr)
    {
        res = QTest::createTouchDevice();
        res->setMaximumTouchPoints(2);
    }

    return res;
}

} // namespace sight::ui::test_core
