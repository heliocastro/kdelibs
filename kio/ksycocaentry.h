/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
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

#ifndef __ksycocaentry_h__
#define __ksycocaentry_h__ "$Id$"

#include "ksycocatype.h"

#include <qdatastream.h>
#include <qlist.h>
#include <ksharedptr.h>

/**
 * Base class for all entries.
 * You can't create instance of KSycocaEntry, but it provides
 * the common functionality for servicetypes and services.
 */
class KSycocaEntry : public KShared
{
 
public:
   virtual bool isType(KSycocaType t) const { return (t == KST_KSycocaEntry); }
   virtual KSycocaType sycocaType() const { return KST_KSycocaEntry; }
   
public:
   /**
    * Default constructor
    */
   KSycocaEntry() : mOffset(0) { }

   /**
    * @internal
    * Restores itself from a stream.
    */
   KSycocaEntry( QDataStream &/*_str*/, int offset ) : mOffset( offset ) { /* Nothing to do here */ }

   /**
    * @return the name of this entry
    */
   virtual QString name() const = 0;
  
   /**
    * @return true if valid
    */
   virtual bool isValid() const = 0;

   /**
    * @internal
    * @return the position of the entry in the sycoca file
    */
   int offset() { return mOffset; }

   /**
    * @internal
    * Save ourselves to the database. Don't forget to call the parent class
    * first if you override this function.
    */
   virtual void save(QDataStream &s)
     {
       mOffset = s.device()->at(); // store position in member variable
       s << (Q_INT32) sycocaType();
     }

   /**
    * @internal
    * Load ourselves from the database. Don't call the parent class!
    */
   virtual void load(QDataStream &) = 0L;

private:
   int mOffset;
};

#endif
