    /*

    Copyright (C) 2001 Nikolas Zimmermann <wildfox@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#ifndef KPLAYOBJECTFACTORY_H
#define KPLAYOBJECTFACTORY_H

#include <kurl.h>
#include "kwrapper.h"
#include "soundserver.h"
#include "kplayobject.h"

class KPlayObjectFactory
{
    public:
    	KPlayObjectFactory(Arts::SimpleSoundServer server);
	~KPlayObjectFactory();

	KPlayObject *createPlayObject(KURL url, bool createBUS);

    private:
	Arts::KWrapperFactory m_factory;
};

#endif
