/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 2000 Peter Kelly (pmk@post.com)
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

#ifndef _DOM_XmlImpl_h_
#define _DOM_XmlImpl_h_

#include "dom_nodeimpl.h"

namespace DOM {

    class DocumentImpl;

class DOMString;

class EntityImpl : public NodeWParentImpl
{
public:
    EntityImpl(DocumentImpl *doc);
    EntityImpl(DocumentImpl *doc, DOMString _publicId, DOMString _systemId, DOMString _notationName);
    virtual ~EntityImpl();

    virtual const DOMString nodeName() const;
    virtual unsigned short nodeType() const;

    virtual DOMString publicId() const;
    virtual DOMString systemId() const;
    virtual DOMString notationName() const;
protected:
    DOMStringImpl *m_publicId;
    DOMStringImpl *m_systemId;
    DOMStringImpl *m_notationName;
};


class EntityReferenceImpl : public NodeWParentImpl
{
public:
    EntityReferenceImpl(DocumentImpl *doc);
    EntityReferenceImpl(DocumentImpl *doc, DOMStringImpl *_entityName);
    virtual ~EntityReferenceImpl();

    virtual const DOMString nodeName() const;
    virtual unsigned short nodeType() const;
protected:
    DOMStringImpl *m_entityName;
};

class NotationImpl : public NodeWParentImpl
{
public:
    NotationImpl(DocumentImpl *doc);
    NotationImpl(DocumentImpl *doc, DOMString _publicId, DOMString _systemId);
    virtual ~NotationImpl();

    virtual const DOMString nodeName() const;
    virtual unsigned short nodeType() const;

    virtual DOMString publicId() const;
    virtual DOMString systemId() const;
protected:
    DOMStringImpl *m_publicId;
    DOMStringImpl *m_systemId;
};


class ProcessingInstructionImpl : public NodeWParentImpl
{
public:
    ProcessingInstructionImpl(DocumentImpl *doc);
    ProcessingInstructionImpl(DocumentImpl *doc, DOMString _target, DOMString _data);
    virtual ~ProcessingInstructionImpl();

    virtual const DOMString nodeName() const;
    virtual unsigned short nodeType() const;
    virtual DOMString nodeValue() const;

    virtual DOMString target() const;
    virtual DOMString data() const;
    virtual void setData( const DOMString &_data );

protected:
    DOMStringImpl *m_target;
    DOMStringImpl *m_data;
};

}; //namespace

#endif
