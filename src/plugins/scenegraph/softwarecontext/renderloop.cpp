/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of Qt Quick 2d Renderer module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "renderloop.h"

#include "context.h"
#include <private/qquickwindow_p.h>
#include <QElapsedTimer>
#include <private/qquickprofiler_p.h>
#include <QCoreApplication>
#include <qpa/qplatformbackingstore.h>
#include <QtGui/QBackingStore>
#include <renderer.h>

QT_BEGIN_NAMESPACE

RenderLoop::RenderLoop()
{
    sg = QSGContext::createDefaultContext();
    rc = sg->createRenderContext();
}

RenderLoop::~RenderLoop()
{
    delete rc;
    delete sg;
}

void RenderLoop::show(QQuickWindow *window)
{
    WindowData data;
    data.updatePending = false;
    data.grabOnly = false;
    m_windows[window] = data;

    if (m_backingStores[window] == nullptr) {
        m_backingStores[window] = new QBackingStore(window);
    }

    maybeUpdate(window);
}

void RenderLoop::hide(QQuickWindow *window)
{
    if (!m_windows.contains(window))
        return;

    m_windows.remove(window);
    delete m_backingStores[window];
    m_backingStores.remove(window);
    hide(window);

    QQuickWindowPrivate *cd = QQuickWindowPrivate::get(window);
    cd->fireAboutToStop();
    cd->cleanupNodesOnShutdown();

    if (m_windows.size() == 0) {
        if (!cd->persistentSceneGraph) {
            rc->invalidate();
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        }
    }
}

void RenderLoop::windowDestroyed(QQuickWindow *window)
{
    hide(window);
    if (m_windows.size() == 0) {
        rc->invalidate();
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    }
}

void RenderLoop::renderWindow(QQuickWindow *window)
{
    QQuickWindowPrivate *cd = QQuickWindowPrivate::get(window);
    if (!cd->isRenderable() || !m_windows.contains(window))
        return;

    WindowData &data = const_cast<WindowData &>(m_windows[window]);

    //Resize the backing store if necessary
    if (m_backingStores[window]->size() != window->size()) {
        m_backingStores[window]->resize(window->size());
    }

    // ### create QPainter and set up pointer to current window/painter
    SoftwareContext::RenderContext *ctx = static_cast<SoftwareContext::RenderContext*>(cd->context);
    ctx->initializeIfNeeded();

    bool alsoSwap = data.updatePending;
    data.updatePending = false;

    if (!data.grabOnly) {
        cd->flushDelayedTouchEvent();
        // Event delivery/processing triggered the window to be deleted or stop rendering.
        if (!m_windows.contains(window))
            return;
    }
    QElapsedTimer renderTimer;
    qint64 renderTime = 0, syncTime = 0, polishTime = 0;
    bool profileFrames = QSG_RASTER_LOG_TIME_RENDERLOOP().isDebugEnabled() || QQuickProfiler::featuresEnabled;
    if (profileFrames)
        renderTimer.start();

    cd->polishItems();

    if (profileFrames) {
        polishTime = renderTimer.nsecsElapsed();
    }

    emit window->afterAnimating();

    cd->syncSceneGraph();

    if (profileFrames)
        syncTime = renderTimer.nsecsElapsed();

    //Tell the renderer about the windows backing store
    auto softwareRenderer = static_cast<SoftwareContext::Renderer*>(cd->renderer);
    if (softwareRenderer)
        softwareRenderer->setCurrentPaintDevice(m_backingStores[window]->paintDevice());

    m_backingStores[window]->beginPaint(QRect(0, 0, window->width(), window->height()));
    cd->renderSceneGraph(window->size());
    m_backingStores[window]->endPaint();

    if (profileFrames)
        renderTime = renderTimer.nsecsElapsed();

    if (data.grabOnly) {
        grabContent = m_backingStores[window]->handle()->toImage();
        data.grabOnly = false;
    }

    if (alsoSwap && window->isVisible()) {
        //Flush backingstore to window
        m_backingStores[window]->flush(softwareRenderer->flushRegion());
        cd->fireFrameSwapped();
    }

    qint64 swapTime = 0;
    if (profileFrames)
        swapTime = renderTimer.nsecsElapsed();

    if (QSG_RASTER_LOG_TIME_RENDERLOOP().isDebugEnabled()) {
        static QTime lastFrameTime = QTime::currentTime();
        qCDebug(QSG_RASTER_LOG_TIME_RENDERLOOP,
                "Frame rendered with 'software' renderloop in %dms, polish=%d, sync=%d, render=%d, swap=%d, frameDelta=%d",
                int(swapTime / 1000000),
                int(polishTime / 1000000),
                int((syncTime - polishTime) / 1000000),
                int((renderTime - syncTime) / 1000000),
                int((swapTime - renderTime) / 10000000),
                int(lastFrameTime.msecsTo(QTime::currentTime())));
        lastFrameTime = QTime::currentTime();
    }

    // Might have been set during syncSceneGraph()
    if (data.updatePending)
        maybeUpdate(window);
}

void RenderLoop::exposureChanged(QQuickWindow *window)
{
    if (window->isExposed()) {
        m_windows[window].updatePending = true;
        renderWindow(window);
    }
}

QImage RenderLoop::grab(QQuickWindow *window)
{
    if (!m_windows.contains(window))
        return QImage();

    m_windows[window].grabOnly = true;

    renderWindow(window);

    QImage grabbed = grabContent;
    grabbed.detach();
    grabContent = QImage();
    return grabbed;
}



void RenderLoop::maybeUpdate(QQuickWindow *window)
{
    if (!m_windows.contains(window))
        return;

    m_windows[window].updatePending = true;
    window->requestUpdate();
}

QSurface::SurfaceType RenderLoop::windowSurfaceType() const
{
    return QSurface::RasterSurface;
}



QSGContext *RenderLoop::sceneGraphContext() const
{
    return sg;
}


void RenderLoop::handleUpdateRequest(QQuickWindow *window)
{
    renderWindow(window);
}

QT_END_NAMESPACE
