/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
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

#ifndef CUPSADDSMB_H
#define CUPSADDSMB_H

#include <qobject.h>
#include <qstringlist.h>
#include <kprocess.h>

class QProgressDialog;

class CupsAddSmb : public QObject
{
	Q_OBJECT
public:
	enum State { None, Start, MkDir, Copy, AddDriver, AddPrinter };
	CupsAddSmb(QObject *parent = 0, const char *name = 0);
	~CupsAddSmb();

	static bool exportDest(const QString& dest, const QString& datadir);

protected slots:
	void slotReceived(KProcess*, char*, int);
	void doNextAction();
	void slotProcessExited(KProcess*);
	void slotCancelled();

protected:
	void checkActionStatus();
	void nextAction();
	bool startProcess();
	bool doExport();
	bool doInstall();

private:
	KProcess	m_proc;
	QStringList	m_buffer;
	int			m_state;
	QStringList	m_actions;
	int			m_actionindex;
	bool		m_status;
	QProgressDialog	*m_dlg;
	QString		m_login, m_password, m_dest;

	static CupsAddSmb	*m_self;
};

#endif
