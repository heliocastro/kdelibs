/**
 * This file is part of the HTML widget for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */
#ifndef render_root_h
#define render_root_h

#include "render_flow.h"

#include <qdatetime.h>

class KHTMLView;
class QScrollView;

namespace khtml {

class RenderRoot : public RenderFlow
{
public:
    RenderRoot(KHTMLView *view);
    virtual ~RenderRoot();

    virtual const char *renderName() const { return "RenderRoot"; }

    virtual bool isRendered() const { return true; }
    virtual bool isInline() const { return false; }
    virtual bool isRoot() const { return true; }

    virtual void layout(bool deep);
    virtual void calcWidth();
    virtual void calcMinMaxWidth();
    virtual void absolutePosition(int &xPos, int&yPos);
    virtual void updateSize();
    virtual void updateHeight();
    virtual void close();

    QScrollView *view();

    virtual void repaint();
    virtual void repaintRectangle(int x, int y, int w, int h);
    virtual void repaintObject(RenderObject *o, int x, int y);

    virtual void setSelection(RenderObject *s, int sp, RenderObject *e, int ep);
    virtual void clearSelection();

    virtual void printObject( QPainter *, int x, int y, int w, int h,
			int tx, int ty);

    void setPrintingMode(bool print) { printingMode = print; }

protected:

    virtual void selectionStartEnd(int& spos, int& epos);

    virtual QRect viewRect() const;

    KHTMLView *m_view;

    QTime updateTimer;

    RenderObject* selectionStart;
    RenderObject* selectionEnd;
    int selectionStartPos;
    int selectionEndPos;

    // used to ignore viewport width when printing to the printer
    bool printingMode;
};

};
#endif
