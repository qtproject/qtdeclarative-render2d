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

#ifndef RENDERLISTBUILDER_H
#define RENDERLISTBUILDER_H

#define QT_NO_OPENGL_ES_3
#undef QT_OPENGL_ES_3

#include <private/qsgadaptationlayer_p.h>

QT_BEGIN_NAMESPACE

namespace SoftwareContext {

class AbstractSoftwareRenderer;

class RenderListBuilder : public QSGNodeVisitorEx
{
public:
    RenderListBuilder(AbstractSoftwareRenderer *renderer);

    virtual bool visit(QSGTransformNode *node);
    virtual void endVisit(QSGTransformNode *);
    virtual bool visit(QSGClipNode *node);
    virtual void endVisit(QSGClipNode *node);
    virtual bool visit(QSGGeometryNode *node);
    virtual void endVisit(QSGGeometryNode *node);
    virtual bool visit(QSGOpacityNode *node);
    virtual void endVisit(QSGOpacityNode *node);
    virtual bool visit(QSGImageNode *node);
    virtual void endVisit(QSGImageNode *node);
    virtual bool visit(QSGPainterNode *node);
    virtual void endVisit(QSGPainterNode *);
    virtual bool visit(QSGRectangleNode *node);
    virtual void endVisit(QSGRectangleNode *node);
    virtual bool visit(QSGGlyphNode *node);
    virtual void endVisit(QSGGlyphNode *node);
    virtual bool visit(QSGNinePatchNode *node);
    virtual void endVisit(QSGNinePatchNode *);
    virtual bool visit(QSGRootNode *);
    virtual void endVisit(QSGRootNode *);

private:
    bool addRenderableNode(QSGNode *node);

    AbstractSoftwareRenderer *m_renderer;
};

}

QT_END_NAMESPACE

#endif // RENDERLISTBUILDER_H
