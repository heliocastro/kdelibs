/*  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Waldo Bastian <bastian@kde.org>
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

#include "config.h"

#include "ksycoca.h"
#include "ksycocatype.h"
#include "ksycocafactory.h"
#include "kuserprofile.h"

#include <qdatastream.h>
#include <qfile.h>
#include <qbuffer.h>

#include <kapp.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

template class QList<KSycocaFactory>;

struct KSycocaPrivate {
    KSycocaPrivate() {
        database = 0;
    }
    QFile *database;
};

// Read-only constructor
KSycoca::KSycoca()
  : DCOPObject("ksycoca")
{
   d = new KSycocaPrivate;
   openDatabase();
   _self = this;
   // Register app as able to receive DCOP messages
   if (kapp && !kapp->dcopClient()->isAttached())
   {
      kapp->dcopClient()->attach();
   }
}

bool KSycoca::openDatabase( bool ignoreErrors )
{
   m_sycoca_mmap = 0;
   QString path = KGlobal::dirs()->saveLocation("config") + "ksycoca";
   if (d->database != 0) delete d->database;
   QFile *database = new QFile(path);
   if (database->open( IO_ReadOnly ))
   {
     m_sycoca_size = database->size();
#ifdef HAVE_MMAP
     m_sycoca_mmap = (const char *) mmap(0, m_sycoca_size,
				PROT_READ, MAP_SHARED,
				database->handle(), 0);
     if (!m_sycoca_mmap)
     {
#endif
        kdWarning(7011) << "mmap failed. (length = " << m_sycoca_size << ")" << endl;
        m_str = new QDataStream(database);
#ifdef HAVE_MMAP
     }
     else
     {
        QByteArray b_array;
        b_array.setRawData(m_sycoca_mmap, m_sycoca_size);
        QBuffer *buffer = new QBuffer( b_array );
        buffer->open(IO_ReadWrite);
        m_str = new QDataStream( buffer);
     }
#endif
     bNoDatabase = false;
   }
   else
   {
     // No database file

     bNoDatabase = true;
     if (!ignoreErrors)  // misnamed argument ?
       return false;

     // We open a dummy database instead.
     kdDebug(7011) << "No database, opening a dummy one." << endl;
     QBuffer *buffer = new QBuffer( QByteArray() );
     buffer->open(IO_ReadWrite);
     m_str = new QDataStream( buffer);
     (*m_str) << (Q_INT32) KSYCOCA_VERSION;
     (*m_str) << (Q_INT32) 0;
   }
   m_lstFactories = new KSycocaFactoryList();
   m_lstFactories->setAutoDelete( true );
   d->database = database;
   return true;
}

// Read-write constructor - only for KBuildSycoca
KSycoca::KSycoca( bool /* dummy */ )
  : DCOPObject("kbuildsycoca")
{
   d = new KSycocaPrivate;
   m_lstFactories = new KSycocaFactoryList();
   m_lstFactories->setAutoDelete( true );
   _self = this;
}

static void delete_ksycoca_self() {
  delete KSycoca::self();
}

KSycoca * KSycoca::self()
{
    if (!_self) {
        qAddPostRoutine(delete_ksycoca_self);
        _self = new KSycoca();
    }
  return _self;
}

KSycoca::~KSycoca()
{
   closeDatabase();
   delete m_lstFactories;
   if (d->database != 0) delete d->database;
   delete d;
   _self = 0L;
}

void KSycoca::closeDatabase()
{
   QIODevice *device = 0;
   if (m_str)
      device = m_str->device();
#ifdef HAVE_MMAP
   if (device && m_sycoca_mmap)
   {
      QBuffer *buf = (QBuffer *) device;
      buf->buffer().resetRawData(m_sycoca_mmap, m_sycoca_size);
      munmap((void *) m_sycoca_mmap, m_sycoca_size);
      m_sycoca_mmap = 0;
   }
#endif
   if (device)
      device->close();

   delete m_str;
   delete device;
   m_str = 0L;
   // It is very important to delete all factories here
   // since they cache information about the database file
   delete m_lstFactories;
   m_lstFactories = 0L;
}

void KSycoca::addFactory( KSycocaFactory *factory )
{
   assert(m_lstFactories);
   m_lstFactories->append(factory);
}

bool KSycoca::process(const QCString &fun, const QByteArray &/*data*/,
                      QCString &replyType, QByteArray &/*replyData*/)
{
  if (fun == "databaseChanged()") {
    //kdDebug(7011) << "got a databaseChanged signal !" << endl;
    // kded tells us the database file changed
    // Close the database and forget all about what we knew
    // The next call to any public method will recreate
    // everything that's needed.
    closeDatabase();
    // same for KUserProfile
    KServiceTypeProfile::clear();

    // Now notify applications
    emit databaseChanged();

    replyType = "void";
    return true;
  } else {
    kdWarning(7011) << QString("unknown function call to KSycoca::process() : %1").arg(fun) << endl;
    return false;
  }
}

QDataStream * KSycoca::findEntry(int offset, KSycocaType &type)
{
   if ( !m_str )
      openDatabase();
   //kdDebug(7011) << QString("KSycoca::_findEntry(offset=%1)").arg(offset,8,16) << endl;
   m_str->device()->at(offset);
   Q_INT32 aType;
   (*m_str) >> aType;
   type = (KSycocaType) aType;
   //kdDebug(7011) << QString("KSycoca::found type %1").arg(aType) << endl;
   return m_str;
}

bool KSycoca::checkVersion(bool abortOnError)
{
   if ( !m_str )
      if( !openDatabase(abortOnError) )
        return false; // No database and don't abort on error
   m_str->device()->at(0);
   Q_INT32 aVersion;
   (*m_str) >> aVersion;
   if ( aVersion < KSYCOCA_VERSION )
   {
      if (!abortOnError) return false;
      kdError(7011) << "Outdated database ! Stop kded and restart it !" << endl;
      kdError(7011) << "Found version " << aVersion << ", expecting version " << KSYCOCA_VERSION << " or higher." << endl;
      abort();
   }
   return true;
}

QDataStream * KSycoca::findFactory(KSycocaFactoryId id)
{
   if (!checkVersion(false))
   {
      KProcess proc;
      proc << locate("exe","kdeinit");
      proc.start( KProcess::Block );
      checkVersion();
   }
   Q_INT32 aId;
   Q_INT32 aOffset;
   while(true)
   {
      (*m_str) >> aId;
      //kdDebug(7011) << QString("KSycoca::findFactory : found factory %1").arg(aId) << endl;
      if (aId == 0)
      {
         kdError(7011) << "Error, KSycocaFactory (id = " << int(id) << ") not found!\n" << endl;
         break;
      }
      (*m_str) >> aOffset;
      if (aId == id)
      {
         //kdDebug(7011) << QString("KSycoca::findFactory(%1) offset %2").arg((int)id).arg(aOffset) << endl;
         m_str->device()->at(aOffset);
         return m_str;
      }
   }
   return 0;
}

QString KSycoca::kfsstnd_prefixes()
{
   if (bNoDatabase) return "";
   if (!checkVersion(false)) return "";
   Q_INT32 aId;
   Q_INT32 aOffset;
   // skip factories offsets
   while(true)
   {
      (*m_str) >> aId;
      if ( aId )
        (*m_str) >> aOffset;
      else
        break; // just read 0
   }
   // We now point to the header
   QString prefixes;
   (*m_str) >> prefixes;
   (*m_str) >> m_timeStamp;
   return prefixes;
}

Q_UINT32 KSycoca::timeStamp()
{
   if (!m_timeStamp)
      (void) kfsstnd_prefixes();
   return m_timeStamp;
}

QString KSycoca::determineRelativePath( const QString & _fullpath, const char *_resource )
{
  QString sRelativeFilePath;
  QStringList dirs = KGlobal::dirs()->resourceDirs( _resource );
  QStringList::ConstIterator dirsit = dirs.begin();
  for ( ; dirsit != dirs.end() && sRelativeFilePath.isEmpty(); ++dirsit ) {
    // might need canonicalPath() ...
    if ( _fullpath.find( *dirsit ) == 0 ) // path is dirs + relativePath
      sRelativeFilePath = _fullpath.mid( (*dirsit).length() ); // skip appsdirs
  }
  if ( sRelativeFilePath.isEmpty() )
    kdFatal(7011) << QString("Couldn't find %1 in any %2 dir !!!").arg( _fullpath ).arg( _resource) << endl;
  //else
    // debug code
    //kdDebug(7011) << sRelativeFilePath << endl;
  return sRelativeFilePath;
}

KSycoca * KSycoca::_self = 0L;

#include "ksycoca.moc"
