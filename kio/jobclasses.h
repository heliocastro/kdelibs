// -*- c++ -*-
/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef __kio_jobclasses_h__
#define __kio_jobclasses_h__

#include <kurl.h>

#include <qobject.h>
//#include <qintdict.h>
#include <qlist.h>
#include <qstring.h>
#include <qstringlist.h>
//#include <qdatetime.h>
//#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <kio/global.h>

namespace KIO {

    class Slave;
    class SlaveInterface;


    /**
     * The base class for all jobs.
     * For all jobs created in an application, the code looks like
     *
     *   KIO::Job * job = KIO::someoperation( some parameters );
     *   connect( job, SIGNAL( result( KIO::Job * ) ),
     *            this, SLOT( slotResult( KIO::Job * ) ) );
     *   (other connects, specific to the job)
     *
     * And slotResult is usually at least:
     *
     *  if ( job->error() )
     *      job->showErrorDialog();
     *
     */
    class Job : public QObject {
	Q_OBJECT
	
    protected:
	    Job();

    public:
	    virtual ~Job() {}

        /**
	    * Abort job
	    * This kills all subjobs and deletes the job
        */
	    virtual void kill();

    	/**
    	 * @return the error code for this job, 0 if no error
    	 * Error codes are defined in @ref KIO::Error.
    	 * Only call this method from the slot connected to @ref result.
    	 */
    	int error() { return m_error; }

    	/**
    	 * @return a string to help understand the error, usually the url
         * related to the error.
    	 * Only call if @ref error is not 0.
    	 * This is really internal, better use errorString or errorDialog.
	     */
        const QString & errorText() { return m_errorText; }

        /**
       	 * Converts an error code and a non-i18n error message into an
   	     * error message in the current language. The low level (non-i18n)
       	 * error message (usually a url) is put into the translated error
         * message using %1.
     	 * Example for errid == ERR_CANNOT_OPEN_FOR_READING:
   	     *   i18n( "Could not read\n%1" ).arg( errortext );
       	 * Use this to display the error yourself, but for a dialog box use @ref KIO::ErrorDialog.
   	     */
        QString errorString();

        /**
         * Display a dialog box to inform the user of the error given by
         * this job.
	     * Only call if @ref error is not 0, and only in the slot connected
	     * to @ref result.
   	     */
        void showErrorDialog();

    signals:
	    /**
    	 * Emitted when the job is finished, in any case (completed, canceled,
	     * failed...). Use @ref error to know the result.
    	 */
	    void result( KIO::Job *job );

    protected slots:
    	/**
	     * Called whenever a subjob finishes.
    	 * Default implementation checks for errors and propagates
    	 * to parent job, then calls @ref removeSubjob.
    	 * Override if you don't want subjobs errors to be propagated.
    	 */
        virtual void slotResult( KIO::Job *job );

	
    protected:
        /**
	     * Add a job that has to be finished before a result
    	 * is emitted. This has obviously to be called before
         * the finish signal is emitted by the slave.
    	 */
        virtual void addSubjob( Job *job );

	    /**
    	 * Mark a sub job as beeing done. If it's the last to
	     * wait on the job will emit a result - jobs with
    	 * two steps might want to override slotResult
         * in order to avoid calling this method.
    	 */
    	virtual void removeSubjob( Job *job );

    	QList<Job> subjobs;
    	int m_error;
    	QString m_errorText;
    };

    /**
     * A simple job (one url and one command)
     * This is the base class for all jobs that are scheduled.
     * Other jobs are high-level jobs (CopyJob, DeleteJob, FIleCopyJob...)
     * that manage subjobs but aren't scheduled directly.
     */
    class SimpleJob : public KIO::Job {
	Q_OBJECT

    public:
        SimpleJob(const KURL& url, int command, const QByteArray &packedArgs);

        ~SimpleJob();

        const KURL& url() const { return m_url; }

        /**
         * Abort job
    	 * Reimplemented (to cancel the job in the scheduler as well)
         */
        virtual void kill();

        /**
         * @internal
         * Called by the scheduler when a slave gets to
    	 * work on this job.
    	 **/
        virtual void start( Slave *slave );
	
        /**
         * @internal
         * Slave in use by this job
         */
    	Slave *slave() { return m_slave; }


    protected slots:
        /**
	     * Called when the slave marks the job
    	 * as finished.
    	 */
        virtual void slotFinished( );

        virtual void slotError( int , const QString & );

    protected:
    	Slave * m_slave;
	    QByteArray m_packedArgs;

    private:
    	KURL m_url;
	    int m_command;
    };

    // Stat Job
    class StatJob : public SimpleJob {

	Q_OBJECT

    public:
    	StatJob(const KURL& url, int command, const QByteArray &packedArgs);

    	/**
	     * Call this in the slot connected to @ref result,
    	 * and only after making sure no error happened.
	     */
    	const UDSEntry & statResult() const { return m_statResult; }

        /**
         * Called by the scheduler when a slave gets to
         * work on this job.
    	 */
        virtual void start( Slave *slave );
	
    protected slots:
        void slotStatEntry( const KIO::UDSEntry & entry );

    protected:
	    UDSEntry m_statResult;
    };

    /**
     * The tranfer job pumps data into and/or out of a Slave.
     * Data is sent to the slave on request of the slave (@ref dataReq).
     * If data coming from the slave can not be handled, the
     * reading of data from the slave should be suspended.
     */
    class TransferJob : public SimpleJob {
    Q_OBJECT

    public:
	    TransferJob(const KURL& url, int command,
	                const QByteArray &packedArgs,
	                const QByteArray &_staticData = QByteArray());

    	virtual void start(Slave *slave);
	
	    /**
    	 * Flow control. Suspend data processing from the slave.
	     */
    	void suspend();
	
    	/**
	     * Flow control. Resume data processing from the slave.
    	 */
	    void resume();

    signals:
        /**
         * Data from the slave has arrived.
         * @param data data received from the slave
         * End of data (EOD) has been reached if data.size() == 0
         */
        void data( KIO::Job *, const QByteArray &data);

        /**
         * Request for data.
         *
         * @param data buffer to fill with data to send to the
         * slave. An empty buffer indicates end of data. (EOD)
         */

        void dataReq( KIO::Job *, QByteArray &data);

    protected slots:
        virtual void slotRedirection( const KURL &url);
        virtual void slotData( const QByteArray &data);
        virtual void slotDataReq();

    protected:
        bool m_suspended;
        QByteArray staticData;
    };

    // Mimetype Job
    class MimetypeJob : public TransferJob {
	Q_OBJECT

    public:
    	MimetypeJob(const KURL& url, int command, const QByteArray &packedArgs);

	    /**
    	 * Call this in the slot connected to @ref result,
	     * and only after making sure no error happened.
    	 */
 	    QString mimetype() const { return m_mimetype; }

        /**
         * Called by the scheduler when a slave gets to
    	 * work on this job.
	     */
    	virtual void start( Slave *slave );
	
    protected slots:
        virtual void slotData( KIO::Job *, const QByteArray &data);
        void slotMimetype( const QString &mimetype );
        virtual void slotFinished( );

    protected:
	    QString m_mimetype;
    };

    /**
     * The FileCopyJob copies data from one place to another.
     */
    class FileCopyJob : public Job {
    Q_OBJECT

    public:
        FileCopyJob( const KURL& src, const KURL& dest, int permissions,
                     bool move, bool overwrite, bool resume);

    public slots:
        void slotData( KIO::Job *, const QByteArray &data);
        void slotDataReq( KIO::Job *, QByteArray &data);

    protected slots:
    	/**
	     * Called whenever a subjob finishes.
	     */
        virtual void slotResult( KIO::Job *job );

    protected:
        void startCopyJob();
        void startDataPump();

        KURL m_src;
        KURL m_dest;
        int m_permissions;
        bool m_move;
        bool m_overwrite;
        bool m_resume;
        QByteArray m_buffer;
        SimpleJob *m_moveJob;
        SimpleJob *m_copyJob;
        TransferJob *m_getJob;
        TransferJob *m_putJob;
        SimpleJob *m_delJob;
    };

    class ListJob : public SimpleJob {
	Q_OBJECT
	
    public:
        ListJob(const KURL& url, bool recursive = false, QString prefix = QString::null);

        virtual void start( Slave *slave );

    signals:
    	void entries( KIO::Job *, const KIO::UDSEntryList& );

    protected slots:
        virtual void slotResult( KIO::Job *job );
	    void slotListEntries( const KIO::UDSEntryList& list );
	    void gotEntries( KIO::Job *, const KIO::UDSEntryList& );

    private:
    	bool recursive;
	    QString prefix;
    };

    struct CopyInfo
    {
        KURL uSource;
        KURL uDest;
        mode_t permissions;
        mode_t type;
        time_t mtime;
        off_t size; // 0 for dirs
    };

    // Copy or Move
    class CopyJob : public Job {
	Q_OBJECT
	
    public:
    	CopyJob( const KURL::List& src, const KURL& dest, bool move = false );

    protected:
        void startNextJob();

        // Those aren't slots but submethods for slotResult.
        void slotResultStating( KIO::Job * job );
        void slotResultCreatingDirs( KIO::Job * job );
        void slotResultConflictCreatingDirs( KIO::Job * job );
        void createNextDir();
        void slotResultCopyingFiles( KIO::Job * job );
        void slotResultConflictCopyingFiles( KIO::Job * job );
        void copyNextFile();

    protected slots:
        void slotEntries( KIO::Job*, const KIO::UDSEntryList& list );
        virtual void slotResult( KIO::Job *job );

    private:
    	bool m_move;
	    enum { DEST_NOT_STATED, DEST_IS_DIR, DEST_IS_FILE, DEST_DOESNT_EXIST } destinationState;
    	enum { STATE_STATING, STATE_LISTING, STATE_CREATING_DIRS, STATE_CONFLICT_CREATING_DIRS,
               STATE_COPYING_FILES, STATE_CONFLICT_COPYING_FILES } state;
	    long int m_totalSize;
    	QValueList<CopyInfo> files;
	    QValueList<CopyInfo> dirs;
        KURL::List m_srcList;
        bool m_bCurrentSrcIsDir;
        KURL m_dest;
        KURL m_currentDest;
        //
        QStringList m_skipList;
        QStringList m_overwriteList;
        bool m_bAutoSkip;
        bool m_bOverwriteAll;
        int m_conflictError;
    };

    class DeleteJob : public Job {
    Q_OBJECT

    public:
	DeleteJob( const KURL::List& src, bool shred );

    protected:
        void startNextJob();
        void deleteNextFile();
        void deleteNextDir();

    protected slots:
        void slotEntries( KIO::Job*, const KIO::UDSEntryList& list );
        virtual void slotResult( KIO::Job *job );

    private:
        enum { STATE_STATING, STATE_LISTING,
               STATE_DELETING_FILES, STATE_DELETING_DIRS } state;
        KURL::List files;
        KURL::List dirs;
        KURL::List m_srcList;
        bool m_shred;
    };

};

#endif
