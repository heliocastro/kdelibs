/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 David Faure   <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
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

#include "kbuildservicetypefactory.h"
#include "ksycoca.h"
#include "ksycocadict.h"

#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <assert.h>

KBuildServiceTypeFactory::KBuildServiceTypeFactory() :
  KServiceTypeFactory()
{
   // Read servicetypes first, since they might be needed to read mimetype properties
   (*m_pathList) += KGlobal::dirs()->resourceDirs( "servicetypes" );
   (*m_pathList) += KGlobal::dirs()->resourceDirs( "mime" );
}

KServiceType * KBuildServiceTypeFactory::findServiceTypeByName(const QString &_name)
{
   assert (KSycoca::self()->isBuilding());
   // We're building a database - the service type must be in memory
   KSycocaEntry * servType = (*m_entryDict)[ _name ];
   return (KServiceType *) servType;
}

