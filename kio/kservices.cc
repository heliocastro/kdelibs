#include "kservices.h"
#include "kservicetype.h"
#include "kuserprofile.h"

#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#include <qstring.h>
#include <qmsgbox.h>
#include <qtl.h>

#include <ksimpleconfig.h>
#include <kapp.h>
#include <klocale.h>
QList<KService>* KService::s_lstServices = 0L;

void KService::initStatic()
{
  if ( !s_lstServices )
    s_lstServices = new QList<KService>;
}

KService* KService::service( const QString& _name )
{
  initStatic();
  
  assert( s_lstServices );

  KService *s;
  for( s = s_lstServices->first(); s != 0L; s = s_lstServices->next() )
    if ( strcmp( s->name(), _name ) == 0 )
      return s;
  
  return 0L;
}

KService::KService( const QString& _name, const QString& _exec, const QString& _icon,
		    const QStringList& _lstServiceTypes, const QString& _comment,
		    bool _allow_as_default, const QString& _path,
		    const QString& _terminal, const QString& _file, const QString& _act_mode,
		    const QStringList& _repo_ids, bool _put_in_list )
{
  initStatic();
  m_bValid = true;

  if ( _put_in_list )
    s_lstServices->append( this );
  
  m_strName = _name;
  m_strExec = _exec;
  m_strIcon = _icon;
  m_strComment = _comment;
  m_lstServiceTypes = _lstServiceTypes;
  m_strPath = _path;
  m_strTerminalOptions = _terminal;
  // m_strFile = _file;
  m_bAllowAsDefault = _allow_as_default;
  m_strActivationMode = _act_mode;
  m_lstRepoIds = _repo_ids;
}

KService::KService( bool _put_in_list )
{
  initStatic();
  m_bValid = false;

  if ( _put_in_list )
    s_lstServices->append( this );
}

KService::KService( KSimpleConfig& config, bool _put_in_list )
{
  initStatic();
  m_bValid = true;

  if ( _put_in_list )
    s_lstServices->append( this );

  config.setDesktopGroup();
  m_strExec = config.readEntry( "Exec" );
  m_strName = config.readEntry( "Name" );
  if ( m_strName.isEmpty() || m_strExec.isEmpty() )
  {
    m_bValid = false;
    return;
  }

  m_strIcon = config.readEntry( "Icon", "unknown.xpm" );
  m_strTerminalOptions = config.readEntry( "TerminalOptions" );  
  m_strPath = config.readEntry( "Path" );
  m_strComment = config.readEntry( "Comment" );
  m_strActivationMode = config.readEntry( "ActivationMode", "UNIX" );
  m_lstRepoIds = config.readListEntry( "RepoIds" );
  m_lstServiceTypes = config.readListEntry( "ServiceTypes" );
  // For compatibility with KDE 1.x
  m_lstServiceTypes += config.readListEntry( "MimeType", ';' );

  m_bAllowAsDefault = config.readBoolEntry( "AllowDefault" );
  
  // Load all additional properties
  QStringList::Iterator it = m_lstServiceTypes.begin();
  for( ; it != m_lstServiceTypes.end(); ++it )
  {    
    KServiceType* s = KServiceType::serviceType( *it );
    if ( s )
    {
      const QMap<QString,QProperty::Type>& pd = s->propertyDefs();
      QMap<QString,QProperty::Type>::ConstIterator pit = pd.begin();
      for( ; pit != pd.end(); ++pit )
      {
	m_mapProps.insert( pit.key(), config.readPropertyEntry( pit.key(), pit.data() ) );
      }
    }
  }
  
}

KService::KService( QDataStream& _str, bool _put_in_list )
{
  initStatic();

  if ( _put_in_list )
    s_lstServices->append( this );

  load( _str );
}

KService::~KService()
{
  s_lstServices->removeRef( this );
}

void KService::load( QDataStream& s )
{
  Q_INT8 b;
  
  s >> m_strName >> m_strExec >> m_strIcon >> m_strTerminalOptions >> m_strPath
    >> m_strComment >> m_lstServiceTypes >> b >> m_mapProps >> m_strActivationMode
    >> m_lstRepoIds;
  m_bAllowAsDefault = b;
  
  m_bValid = true;
}

void KService::save( QDataStream& s ) const
{
  s << m_strName << m_strExec << m_strIcon << m_strTerminalOptions << m_strPath
    << m_strComment << m_lstServiceTypes << (Q_INT8)m_bAllowAsDefault << m_mapProps
    << m_strActivationMode << m_lstRepoIds;
}

bool KService::hasServiceType( const QString& _servicetype ) const
{
  debug("Testing %s", m_strName.ascii());
  
  QStringList::ConstIterator it = m_lstServiceTypes.begin();
  for( ; it != m_lstServiceTypes.end(); ++it )
    debug("    has %s", it->ascii() );
  
  return ( m_lstServiceTypes.find( _servicetype ) != m_lstServiceTypes.end() );
}

KService::PropertyPtr KService::property( const QString& _name ) const
{
  QProperty* p = 0;
  
  if ( _name == "Name" )
    p = new QProperty( m_strName );
  if ( _name == "Exec" )
    p = new QProperty( m_strExec );
  if ( _name == "Icon" )
    p = new QProperty( m_strIcon );
  if ( _name == "TerminalOptions" )
    p = new QProperty( m_strTerminalOptions );
  if ( _name == "Path" )
    p = new QProperty( m_strPath );
  if ( _name == "Comment" )
    p = new QProperty( m_strComment );
  //  if ( _name == "File" )
  //    p = new QProperty( m_strFile );
  if ( _name == "ServiceTypes" )
    p = new QProperty( m_lstServiceTypes );
  if ( _name == "AllowAsDefault" )
    p = new QProperty( m_bAllowAsDefault );

  if ( p )
  {    
    // We are not interestes in these
    p->deref();
    return p;
  }
  
  QMap<QString,QProperty>::ConstIterator it = m_mapProps.find( _name );
  if ( it == m_mapProps.end() )
    return (QProperty*)0;
  
  p = (QProperty*)(&(it.data()));
  
  return p;
}

QStringList KService::propertyNames() const
{
  QStringList res;
  
  QMap<QString,QProperty>::ConstIterator it = m_mapProps.begin();
  for( ; it != m_mapProps.end(); ++it )
    res.append( it.key() );
  
  res.append( "Name" );
  res.append( "Comment" );
  res.append( "Icon" );
  res.append( "Exec" );
  res.append( "TerminalOptions" );
  res.append( "Path" );
  res.append( "File" );
  res.append( "ServiceTypes" );
  res.append( "AllowAsDefault" );

  return res;
}

QDataStream& operator>>( QDataStream& _str, KService& s )
{
  s.load( _str );
  return _str;
}

QDataStream& operator<<( QDataStream& _str, const KService& s )
{
  s.save( _str );
  return _str;
}
