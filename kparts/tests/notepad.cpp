
#include "notepad.h"
#include <partmanager.h>
#include <mainwindow.h>

#include <qsplitter.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmultilinedit.h>

#include <kapp.h>
#include <kaction.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kstddirs.h>

NotepadPart::NotepadPart( QWidget * parent, const char * name )
 : KParts::ReadWritePart( parent, name )
{
  KInstance * instance = new KInstance( "notepadpart" );
  setInstance( instance );

  m_edit = new QMultiLineEdit( parent, "NotepadPart's multiline edit" );
  m_edit->setFocus();
  setWidget( m_edit );

  (void)new KAction( i18n( "Search and replace" ), 0, this, SLOT( slotSearchReplace() ), actionCollection(), "searchreplace" );
  setXMLFile( "notepadpart.rc" );

  setReadWrite( false ); // as required by the framework
}

NotepadPart::~NotepadPart()
{
}

void NotepadPart::setReadWrite( bool rw )
{
    m_edit->setReadOnly( !rw );
    if (rw)
        connect( m_edit, SIGNAL( textChanged() ), this, SLOT( setModified() ) );
    else
        disconnect( m_edit, SIGNAL( textChanged() ), this, SLOT( setModified() ) );

    ReadWritePart::setReadWrite( rw );
}

bool NotepadPart::openFile()
{
  debug("NotepadPart: opening %s", m_file.ascii());
  // Hehe this is from a tutorial I did some time ago :)
  QFile f(m_file);
  QString s;
  if ( f.open(IO_ReadOnly) ) {
    QTextStream t( &f );
    while ( !t.eof() ) {
      s += t.readLine() + "\n";
    }
    f.close();
  }
  m_edit->setText(s);

  if (manager())
    manager()->setWindowCaption( m_url.url() );
  // see comments
  //manager()->statusBar()->message( m_url.url() );

  return true;
}

bool NotepadPart::saveFile()
{
  if ( !isReadWrite() )
    return false;
  QFile f(m_file);
  QString s;
  if ( f.open(IO_WriteOnly) ) {
    QTextStream t( &f );
    t << m_edit->text();
    f.close();
  } else
    return false;
  // save() should always call saveToURL() in the end.
  return saveToURL();
}

void NotepadPart::slotSearchReplace()
{
  // What's this ? (David)
/*
  QValueList<KParts::XMLGUIServant *> plugins = KParts::Plugin::pluginServants( this );
  QValueList<KParts::XMLGUIServant *>::ConstIterator it = plugins.begin();
  QValueList<KParts::XMLGUIServant *>::ConstIterator end = plugins.end();
  for (; it != end; ++it )
    factory()->removeServant( *it );
*/
}

#include "notepad.moc"
