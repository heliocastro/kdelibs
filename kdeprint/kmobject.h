/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id:  $
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef KMOBJECT_H
#define KMOBJECT_H

class KMObject
{
public:
	KMObject();

	bool isDiscarded() const;
	void setDiscarded(bool on = true);

protected:
	bool m_discarded;
};

inline KMObject::KMObject() : m_discarded(false)
{ }

inline bool KMObject::isDiscarded() const
{ return m_discarded; }

inline void KMObject::setDiscarded(bool on)
{ m_discarded = on; }

#endif
