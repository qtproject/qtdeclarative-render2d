/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick 2D Renderer module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PIXMAPRENDERER_H
#define PIXMAPRENDERER_H

#define QT_NO_OPENGL_ES_3
#undef QT_OPENGL_ES_3

#include "abstractsoftwarerenderer.h"

QT_BEGIN_NAMESPACE

namespace SoftwareContext {

class PixmapRenderer : public AbstractSoftwareRenderer
{
public:
    PixmapRenderer(QSGRenderContext *context);
    virtual ~PixmapRenderer();

    void renderScene(GLuint fboId = 0) final;
    void render() final;

    void render(QPaintDevice *target);
    void setProjectionRect(const QRect &projectionRect);

private:
    QRect m_projectionRect;
};

} // namespace

QT_END_NAMESPACE

#endif // PIXMAPRENDERER_H
