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

#include <core/spyLog.hpp>

#include <ui/base/Preferences.hpp>

#include <boost/algorithm/string.hpp>

#include <QAction>
#include <QApplication>
#include <QMutex>
#include <QScreen>
#include <QTest>
#include <QTimer>

#include <cmath>
#include <iostream>

namespace sight::ui::testCore
{

//------------------------------------------------------------------------------

bool Tester::s_alreadyLoaded                    = false;
std::filesystem::path Tester::s_imageOutputPath = std::filesystem::temp_directory_path();

//------------------------------------------------------------------------------

TestEvent::TestEvent(std::function<void()> f) :
    QEvent(QEvent::User),
    m_function(std::move(f))
{
}

//------------------------------------------------------------------------------

std::function<void()> TestEvent::function()
{
    return m_function;
}

//------------------------------------------------------------------------------

bool TestEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::User)
    {
        auto* testEvent = dynamic_cast<TestEvent*>(event);
        // the event catched here might not be a TestEvent, in which case, testEvent will be nullptr
        if(testEvent != nullptr)
        {
            testEvent->function()();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

TesterAssertionFailed::TesterAssertionFailed(const std::string& msg) :
    std::runtime_error(msg)
{
}

//------------------------------------------------------------------------------

bool alwaysTrue(QObject* /*unused*/)
{
    return true;
}

Tester::BacktraceLock::BacktraceLock(Tester& tester) :
    m_tester(tester)
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

Tester::Tester(std::string testName, bool verboseMode) :
    m_testName(std::move(testName)),
    m_verboseMode(verboseMode)
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
    const std::string& componentDescription,
    std::function<QObject* ()> graphicComponent,
    std::function<bool(QObject*)> condition,
    int timeout
)
{
    take<QObject*>(componentDescription, graphicComponent, condition, timeout);
}

//------------------------------------------------------------------------------

void Tester::take(const std::string& componentDescription, QObject* graphicComponent)
{
    m_componentDescription = componentDescription;
    m_graphicComponent     = graphicComponent;
}

//------------------------------------------------------------------------------

void Tester::take(
    const std::string& componentDescription,
    QObject* parent,
    const std::string& objectName,
    std::function<bool(QObject*)> condition,
    int timeout
)
{
    take(
        componentDescription,
        [parent, &objectName]
        {
            return parent->findChild<QObject*>(QString::fromStdString(objectName));
        },
        condition,
        timeout
    );
}

//------------------------------------------------------------------------------

void Tester::take(
    const std::string& componentDescription,
    const std::string& objectName,
    std::function<bool(QObject*)> condition,
    int timeout
)
{
    take(componentDescription, getMainWindow(), objectName, condition, timeout);
}

//------------------------------------------------------------------------------

void Tester::interact(std::unique_ptr<Interaction>&& interaction)
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
        qDebug() << interaction->toString().c_str() << "on" << QString::fromStdString(m_componentDescription);
    }

    if(widget != nullptr)
    {
        interaction->interactWith(widget);
    }
    else if(window != nullptr)
    {
        interaction->interactWith(window);
    }

    m_interactions.push_back(std::move(interaction));
}

//------------------------------------------------------------------------------

void Tester::doubt(const std::string& resultDescription, std::function<bool(QObject*)> result, int timeout)
{
    doubt<QObject*>(resultDescription, result, timeout);
}

//------------------------------------------------------------------------------

void Tester::yields(
    const std::string& componentDescription,
    std::function<QObject* (QObject*)> graphicComponent,
    std::function<bool(QObject*)> condition,
    int timeout
)
{
    yields<QObject*>(componentDescription, graphicComponent, condition, timeout);
}

//------------------------------------------------------------------------------

void Tester::yields(
    const std::string& componentDescription,
    const std::string& objectName,
    std::function<bool(QObject*)> condition,
    int timeout
)
{
    yields<QObject*>(componentDescription, objectName, condition, timeout);
}

//------------------------------------------------------------------------------

void Tester::maybeTake(
    const std::string& componentDescription,
    std::function<QObject* ()> graphicComponent,
    std::function<bool(QObject*)> condition,
    int timeout
)
{
    maybeTake<QObject*>(componentDescription, graphicComponent, condition, timeout);
}

//------------------------------------------------------------------------------

void Tester::doSomething(std::function<void(QObject*)> f)
{
    doSomething<QObject*>(f);
}

//------------------------------------------------------------------------------

void Tester::doSomethingAsynchronously(std::function<void(QObject*)> f)
{
    doSomethingAsynchronously<QObject*>(f);
}

//------------------------------------------------------------------------------

void Tester::takeScreenshot(const std::filesystem::path& path)
{
    if(get<QWidget*>()->windowHandle() != nullptr)
    {
        get<QWidget*>()->screen()->grabWindow(get<QWidget*>()->winId()).save(QString::fromStdString(path.string()));
    }
    else
    {
        doSomethingAsynchronously<QWidget*>([path](QWidget* o){o->grab().save(QString::fromStdString(path.string()));});
    }
}

//------------------------------------------------------------------------------

void Tester::start(std::function<void()> f)
{
    m_thread = std::thread(
        [this, f]() -> void
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
                f();
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
                             / (m_testName + "_failure.png")).string()
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
    const std::string& componentDescription,
    std::function<QWidget* ()> graphicComponent,
    std::function<bool(QWidget*)> condition,
    int timeout
)
{
    maybeTake<QWidget*>(
        componentDescription,
        graphicComponent,
        [&condition](QWidget* obj)
        {
            return condition(obj) && !obj->isVisible();
        },
        timeout
    );
    if(exists())
    {
        fail('"' + componentDescription + "\" is present, though it should be hidden");
    }
}

//------------------------------------------------------------------------------

void Tester::shouldBePresent(
    const std::string& componentDescription,
    std::function<QWidget* ()> graphicComponent,
    std::function<bool(QWidget*)> condition,
    int timeout
)
{
    take<QWidget*>(
        componentDescription,
        graphicComponent,
        [&condition](QWidget* obj)
        {
            return condition(obj) && obj->isVisible();
        },
        timeout
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

Tester::BacktraceLock Tester::addInBacktrace(const std::string& description)
{
    m_backtrace.push_back(description);
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

        sight::ui::base::Preferences::ignoreFilesystem(true);

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

QWidget* Tester::getWidgetFromAction(QAction* action)
{
    return action != nullptr ? action->associatedWidgets().last() : nullptr;
}

//------------------------------------------------------------------------------

void Tester::mouseMove(
    QWidget* widget,
    QPoint pos,
    int delay,
    Qt::MouseButton button,
    Qt::KeyboardModifiers modifiers
)
{
    // Workaround to QTBUG-5232
    if(delay > 0)
    {
        QTest::qWait(delay);
    }

    if(pos.isNull())
    {
        pos = widget->rect().center();
    }

    QMouseEvent event(QEvent::MouseMove, pos, widget->mapToGlobal(pos), Qt::NoButton, button, modifiers);
    qApp->sendEvent(widget, &event);
}

//------------------------------------------------------------------------------

void Tester::mouseMove(
    QWindow* window,
    QPoint pos,
    int delay,
    Qt::MouseButton button,
    Qt::KeyboardModifiers modifiers
)
{
    // Workaround to QTBUG-5232
    if(delay > 0)
    {
        QTest::qWait(delay);
    }

    if(pos.isNull())
    {
        pos = window->geometry().center();
    }

    QMouseEvent event(QEvent::MouseMove, pos, window->mapToGlobal(pos), Qt::NoButton, button, modifiers);
    qApp->sendEvent(window, &event);
}

//------------------------------------------------------------------------------

double Tester::compareImagesPixelPerfect(QImage a, QImage b, bool strict)
{
    if(a.size() != b.size())
    {
        if(strict)
        {
            return 0;
        }

        const QSize size = a.size().boundedTo(b.size());
        a = a.scaled(size);
        b = b.scaled(size);
    }

    int identicalPixels = 0;
    for(int y = 0 ; y < a.height() ; y++)
    {
        for(int x = 0 ; x < a.width() ; x++)
        {
            identicalPixels += static_cast<int>(a.pixelColor(x, y) == b.pixelColor(x, y));
        }
    }

    return static_cast<double>(identicalPixels) / (a.height() * a.width());
}

//------------------------------------------------------------------------------

double Tester::compareImagesMSE(QImage a, QImage b, bool strict)
{
    if(a.size() != b.size())
    {
        if(strict)
        {
            return 0;
        }

        const QSize size = a.size().boundedTo(b.size());
        a = a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        b = b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double res = 0;
    for(int y = 0 ; y < a.height() ; y++)
    {
        for(int x = 0 ; x < a.width() ; x++)
        {
            const QColor ca = a.pixelColor(x, y);
            const QColor cb = b.pixelColor(x, y);
            res += std::pow(ca.red() - cb.red(), 2) + std::pow(ca.green() - cb.green(), 2) + std::pow(
                ca.blue() - cb.blue(),
                2
            );
        }
    }

    return 1 - (res / (a.height() * a.width())) / (3 * std::pow(255, 2));
}

//------------------------------------------------------------------------------

double Tester::compareImagesCosine(QImage a, QImage b, bool strict)
{
    if(a.size() != b.size())
    {
        if(strict)
        {
            return 0;
        }

        const QSize size = a.size().boundedTo(b.size());
        a = a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        b = b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double pa = 0;
    double pb = 0;
    double ab = 0;
    for(int y = 0 ; y < a.height() ; y++)
    {
        for(int x = 0 ; x < a.width() ; x++)
        {
            const QColor ca = a.pixelColor(x, y);
            int ra          = ca.red() + 1;
            int ga          = ca.green() + 1;
            int ba          = ca.blue() + 1;
            const QColor cb = b.pixelColor(x, y);
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

double Tester::compareImagesHistogram(const QImage& x, const QImage& y)
{
    const QVector<QVector<QVector<double> > > histogramA = computeHistogram(x);
    const QVector<QVector<QVector<double> > > histogramB = computeHistogram(y);
    double res                                           = 0;
    for(int r = 0 ; r < 256 ; r++)
    {
        for(int g = 0 ; g < 256 ; g++)
        {
            for(int b = 0 ; b < 256 ; b++)
            {
                double addition = histogramA[r][g][b] + histogramB[r][g][b];
                if(addition > 0)
                {
                    res += std::pow(histogramA[r][g][b] - histogramB[r][g][b], 2) / addition;
                }
            }
        }
    }

    return 1 - (res / 2);
}

//------------------------------------------------------------------------------

double Tester::compareImagesCorrelation(QImage a, QImage b, bool strict)
{
    if(a.size() != b.size())
    {
        if(strict)
        {
            return 0;
        }

        const QSize size = a.size().boundedTo(b.size());
        a = a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        b = b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double res = 0;
    for(int y = 0 ; y < a.height() ; y++)
    {
        for(int x = 0 ; x < a.width() ; x++)
        {
            const QColor ca = a.pixelColor(x, y);
            const QColor cb = b.pixelColor(x, y);
            res += std::pow(ca.red() - cb.red(), 2) + std::pow(ca.green() - cb.green(), 2) + std::pow(
                ca.blue() - cb.blue(),
                2
            );
        }
    }

    const int size = a.height() * a.width();
    return 1 - (6 * res) / (std::pow(size, 2) - 1);
}

//------------------------------------------------------------------------------

double Tester::compareImagesVoodoo(const QImage& a, const QImage& b)
{
    const QImage av = voodooize(a);
    const QImage bv = voodooize(b);
    return compareImagesMSE(av, bv);
}

//------------------------------------------------------------------------------

void Tester::fail(const std::string& message)
{
    throw TesterAssertionFailed(message);
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

QVector<QVector<QVector<double> > > Tester::computeHistogram(const QImage& img)
{
    QVector<QVector<QVector<double> > > histogram(256, QVector(256, QVector(256, 0.0)));
    for(int y = 0 ; y < img.height() ; y++)
    {
        for(int x = 0 ; x < img.width() ; x++)
        {
            QColor color = img.pixelColor(x, y);
            histogram[color.red()][color.green()][color.blue()]++;
        }
    }

    for(int r = 0 ; r < 256 ; r++)
    {
        for(int g = 0 ; g < 256 ; g++)
        {
            for(int b = 0 ; b < 256 ; b++)
            {
                histogram[r][g][b] /= img.width() * img.height();
            }
        }
    }

    return histogram;
}

//------------------------------------------------------------------------------

QImage Tester::normalize(QImage img)
{
    int lightestRed   = -1;
    int lightestGreen = -1;
    int lightestBlue  = -1;
    int darkestRed    = 256;
    int darkestGreen  = 256;
    int darkestBlue   = 256;
    for(int y = 0 ; y < img.height() ; y++)
    {
        for(int x = 0 ; x < img.width() ; x++)
        {
            QColor color = img.pixelColor(x, y);

            lightestRed   = std::max(lightestRed, color.red());
            darkestRed    = std::min(darkestRed, color.red());
            lightestGreen = std::max(lightestGreen, color.green());
            darkestGreen  = std::min(darkestGreen, color.green());
            lightestBlue  = std::max(lightestBlue, color.blue());
            darkestBlue   = std::min(darkestBlue, color.blue());
        }
    }

    // If the lightest and darkest color are the same, don't modify the color
    if(lightestRed == darkestRed)
    {
        darkestRed  = 0;
        lightestRed = 255;
    }

    if(lightestGreen == darkestGreen)
    {
        darkestGreen  = 0;
        lightestGreen = 255;
    }

    if(lightestBlue == darkestBlue)
    {
        darkestBlue  = 0;
        lightestBlue = 255;
    }

    for(int y = 0 ; y < img.height() ; y++)
    {
        for(int x = 0 ; x < img.width() ; x++)
        {
            QColor color = img.pixelColor(x, y);
            color.setRed((color.red() - darkestRed) * (255 / (lightestRed - darkestRed)));
            color.setGreen((color.green() - darkestGreen) * (255 / (lightestGreen - darkestGreen)));
            color.setBlue((color.blue() - darkestBlue) * (255 / (lightestBlue - darkestBlue)));
            img.setPixelColor(x, y, color);
        }
    }

    return img;
}

//------------------------------------------------------------------------------

QImage Tester::voodooize(const QImage& img)
{
    QImage res = img.scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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

bool Tester::waitForAsynchronously(std::function<bool()> predicate, int timeout)
{
    return QTest::qWaitFor(
        [predicate]() -> bool
        {
            QMutex mutex;
            mutex.lock(); // Lock the mutex by default
            bool ok = false;
            qApp->postEvent(
                qApp,
                new TestEvent(
                    [&mutex, &ok, predicate]
            {
                ok = predicate();
                mutex.unlock(); // Unlock the mutex for the calling thread to be unlocked
            })
            );
            mutex.lock();   // Wait for the main thread to execute the predicate
            mutex.unlock(); // We must unlock it, as destroying a locked mutex is undefined behavior
            return ok;
        },
        timeout
    );
}

//------------------------------------------------------------------------------

QPoint Tester::centerOf(const QWidget* widget)
{
    return widget->rect().center();
}

//------------------------------------------------------------------------------

QPoint Tester::leftOf(const QWidget* widget)
{
    return {1, centerOf(widget).y()};
}

//------------------------------------------------------------------------------

QPoint Tester::rightOf(const QWidget* widget)
{
    return {widget->width() - 1, centerOf(widget).y()};
}

//------------------------------------------------------------------------------

QPoint Tester::topOf(const QWidget* widget)
{
    return {centerOf(widget).x(), 1};
}

//------------------------------------------------------------------------------

QPoint Tester::bottomOf(const QWidget* widget)
{
    return {centerOf(widget).x(), widget->height() - 1};
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

} // namespace sight::ui::testCore
