#include "kmimetype.h"

#include <qpixmapcache.h>
#include <qpixmap.h>

#include <qstring.h>

#include <kapp.h>

#include <sys/stat.h>

#include "kpixmapcache.h"
#include <kstddirs.h>
#include <kglobal.h>

QPixmap* KPixmapCache::pixmapForURL( const KURL& _url, mode_t _mode, bool _is_local_file, bool _mini )
{
  return pixmap( KMimeType::findByURL( _url, _mode, _is_local_file )->icon( _url, _is_local_file ).ascii(), _mini );
}

QPixmap* KPixmapCache::pixmapForURL( const char* _url, mode_t _mode, bool _is_local_file, bool _mini )
{
  KURL url( _url );
  
  return pixmap( KMimeType::findByURL( url, _mode, _is_local_file )->icon( url, _is_local_file ).ascii(), _mini );
}

QString KPixmapCache::pixmapFileForURL( const char* _url, mode_t _mode, bool _is_local_file, bool _mini )
{
  KURL url( _url );
  
  return pixmapFile( KMimeType::findByURL( url, _mode, _is_local_file )->icon( url, _is_local_file ).ascii(), _mini );
}

QPixmap* KPixmapCache::pixmapForMimeType( const char *_mime_type, bool _mini )
{
  return pixmap( KMimeType::mimeType( _mime_type )->icon( QString::null, false ).ascii(), _mini );
}

QPixmap* KPixmapCache::pixmapForMimeType( KMimeType *_mime_type, bool _mini )
{
  return pixmap( _mime_type->icon( QString::null, false ).ascii(), _mini );
}

QPixmap* KPixmapCache::pixmapForMimeType( KMimeType *_mime_type, const KURL& _url, bool _is_local_file, bool _mini )
{
  return pixmap( _mime_type->icon( _url, _is_local_file ).ascii(), _mini );
}

QString KPixmapCache::pixmapFileForMimeType( const char *_mime_type, bool _mini )
{
  return pixmapFile( KMimeType::mimeType( _mime_type )->icon( QString::null, false ).ascii(), _mini );
}

QString hackLocate( const QString &type, const QString &filename )
{
  QString file = locate( type, filename );
  if ( file.isNull() && filename.right( 4 ) == ".xpm" )
  {
    file = filename;
    file.truncate( file.length() - 4 );
    file.append( ".png" );
    file = locate( type, file );
  }
  return file;
}

QPixmap *hackCacheFind( const QString &key )
{
  QPixmap *pix = QPixmapCache::find( key );
  
  if ( !pix && key.right( 4 ) == ".xpm" )
  {
    QString key2 = key;
    key2.truncate( key2.length() - 4 );
    key2.append( ".png" );
    pix = QPixmapCache::find( key2 );
  }
  
  return pix;
}

QPixmap* KPixmapCache::toolbarPixmap( const char *_pixmap )
{
  QString key = "toolbar/";
  key += _pixmap;

  QPixmap* pix = hackCacheFind( key );
  if ( pix )
    return pix;

  QString file = hackLocate("toolbar", _pixmap);

  QPixmap p1;
  p1.load( file );
  if ( !p1.isNull() )
  {
    QPixmapCache::insert( key, p1 );
    return QPixmapCache::find( key );
  }
  
  return 0L;
}

QPixmap* KPixmapCache::wallpaperPixmap( const char *_wallpaper )
{
  QString key = "wallpapers/";
  key += _wallpaper;

  QPixmap* pix = hackCacheFind( key );
  if ( pix )
    return pix;

  QString file = hackLocate("wallpaper", _wallpaper);
  
  QPixmap p1;
  p1.load( file );
  if ( !p1.isNull() )
  {
    QPixmapCache::insert( key, p1 );
    return QPixmapCache::find( key );
  }

  return 0;
}

QPixmap* KPixmapCache::pixmap( const char *_pixmap, bool _mini )
{
  QString file = pixmapFile(_pixmap, _mini);

  QPixmap* pix = hackCacheFind( file );
  if ( pix )
    return pix;
  
  QPixmap p1;
  p1.load( file );
  if ( !p1.isNull() )
    QPixmapCache::insert( file, p1 );
  
  // there will always be an entry, as pixmapFile will return
  // unknown.xpm if in doubt
  return QPixmapCache::find( file );
}

QString KPixmapCache::pixmapFile( const char *_pixmap, bool _mini )
{
  QString key = _mini ? "mini" : "icon";
  QString file = hackLocate(key, _pixmap);
  if (file.isNull())
    return hackLocate(key, "unknown.xpm");
  
  return file;
}

QPixmap* KPixmapCache::defaultPixmap( bool _mini )
{
  return pixmap( "unknown.xpm", _mini );
}
