// $Id$
//
// $Log$
// Revision 1.5  1998/01/15 13:22:29  kalle
// Read-only mode for KSimpleConfig
//
// Revision 1.4  1997/12/12 14:46:04  denis
// Reverting to lower case true/false
//
// Revision 1.3  1997/12/12 14:33:48  denis
// replaced true by TRUE and false by FALSE
// Revision 1.6  1998/01/18 13:39:23  jacek
// Changes KCharsets*:
// Added KCharset class - small changes in interface. I hope it is all source
//                        and binary compatible. Some applications may now ask to recompile them,
// 		       but no changes should be neccessary.
// Added Unicode maping for adobe-symbol fonts, but they are not well displayable yet.
// And some other changes.
//
//
// Revision 1.2  1997/10/10 19:24:11  kulow
// removed mutable and replace const_cast with a more portable way.
//
// Revision 1.1  1997/10/04 19:51:06  kalle
// new KConfig
//
//
// (C) 1996 by Matthias Kalle Dalheimer

#ifndef _KCONFIGDATA_H
#define _KCONFIGDATA_H

#include <qdict.h> // QDict
#include <qtstream.h> // QTextStream

struct KEntryDictEntry
{
  QString aValue;
  bool    bDirty; // must the entry be written back to disk?
  bool    bGlobal; // entry should be written to the global config file
  bool    bNLS;    // entry should be written with locale tag
};

* @version $Id$
typedef QDict<KEntryDict> KGroupDict;
typedef QDictIterator<KEntryDict> KGroupIterator;
typedef QDictIterator<KEntryDictEntry> KEntryIterator;

/**
* Configuration data manager, used internally by KConfig.
* @short Configuration data manager, used internally by KConfig.
* @version $Id$
* @author Matthias Kalle Dalheimer (kalle@kde.org)
*/
class KConfigBaseData
friend class KConfig;
friend class KConfigBase;
friend class KSimpleConfig;
private:
  QString aLocalAppFile;
  QString aGlobalAppFile;
  QString aGroup;
  QString aLocaleString; // locale code
  bool bDirty; // is there any entry that has to be written back to disk?
  bool bLocaleInitialized;
  bool bReadOnly; // currently only used by KSimpleConfig

  QDict<KEntryDict> aGroupDict;

#ifndef NDEBUG
  QString aFile;
#endif
  
public:
  KConfigBaseData();
  KConfigBaseData( const char* pGlobalAppFile, const char* pLocalAppFile );
  KGroupIterator* groupIterator( void );
};

inline KConfigBaseData::KConfigBaseData() :
    aGroupDict( 37, false )
{
  aGroupDict.setAutoDelete( true );
  aGroup = "<default>";
  bDirty = false;
  bLocaleInitialized = false;
  bReadOnly = false;
}
inline KConfigBaseData::KConfigBaseData( const char* pGlobalAppFile,
										 const char* pLocalAppFile ) :
    aGroupDict( 37, false )
{
  aGroupDict.setAutoDelete( true );
  aLocalAppFile = pLocalAppFile;
  aGlobalAppFile = pGlobalAppFile;
  aGroup = "<default>";
  bDirty = false;
  bLocaleInitialized = false;
  bReadOnly = false;
}

inline KGroupIterator* KConfigBaseData::groupIterator(void)
{
  return new KGroupIterator(aGroupDict);
}


#endif
