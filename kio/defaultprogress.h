/* This file is part of the KDE libraries
   Copyright (C) 2000 Matej Koss <koss@miesto.sk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __defaultprogress_h__
#define __defaultprogress_h__

#include <qlabel.h>

#include <kprogress.h>

#include "progressbase.h"

class DefaultProgress : public ProgressBase {

  Q_OBJECT

public:

  DefaultProgress();
  ~DefaultProgress() {}

protected slots:

  virtual void slotTotalSize( KIO::Job*, unsigned long _bytes );
  virtual void slotTotalFiles( KIO::Job*, unsigned long _files );
  virtual void slotTotalDirs( KIO::Job*, unsigned long _dirs );

  virtual void slotProcessedSize( KIO::Job*, unsigned long _bytes );
  virtual void slotProcessedFiles( KIO::Job*, unsigned long _files );
  virtual void slotProcessedDirs( KIO::Job*, unsigned long _dirs );

  virtual void slotSpeed( KIO::Job*, unsigned long _bytes_per_second );
  virtual void slotPercent( KIO::Job*, unsigned int _percent );

  virtual void slotCopying( KIO::Job*, const KURL& _src, const KURL& _dest );
  virtual void slotMoving( KIO::Job*, const KURL& _src, const KURL& _dest );
  virtual void slotDeleting( KIO::Job*, const KURL& _file );
  virtual void slotCreatingDir( KIO::Job*, const KURL& _dir );

  virtual void slotCanResume( KIO::Job*, bool );

protected:

  QLabel* progressLabel;
  QLabel* sourceLabel;
  QLabel* destLabel;
  QLabel* speedLabel;
  QLabel* sizeLabel;
  QLabel* resumeLabel;

  KProgress* m_pProgressBar;

  unsigned long m_iTotalSize;
  unsigned long m_iTotalFiles;
  unsigned long m_iTotalDirs;

  unsigned long m_iProcessedDirs;
  unsigned long m_iProcessedFiles;

  enum ModeType { Copy, Move, Delete, Create };
  ModeType mode;
};

#endif // __defaultprogress_h__
