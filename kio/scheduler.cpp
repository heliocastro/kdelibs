#include "scheduler.h"
#include "slave.h"
#include <qlist.h>
#include <kdebug.h>
#include <assert.h>

using namespace KIO;

Scheduler *Scheduler::instance = 0;

class KIO::SlaveList: public QList<Slave>
{
   public:
      SlaveList() { }
};

Scheduler::Scheduler()
    : QObject(0, "scheduler"), mytimer(this, "Scheduler::mytimer")
{
    slaveList = new SlaveList;
    idleSlaves = new SlaveList;
    connect(&mytimer, SIGNAL(timeout()),
	    SLOT(startStep()));
    joblist.setAutoDelete(false);
    busy = false;
}

void
Scheduler::debug_info()
{
    kDebugInfo(7006, "Scheduler Info");
    kDebugInfo(7006, "==========");
    kDebugInfo(7006, "Total Slaves: %d", slaveList->count());
    Slave *slave = slaveList->first();
    for(; slave; slave = slaveList->next())
    {
        kdDebug(7006) << " Slave: " << slave->protocol() << " " << slave->host() << slave->port() << endl;
    }
    kDebugInfo(7006, "Idle Slaves: %d", idleSlaves->count());
    slave = idleSlaves->first();
    for(; slave; slave = idleSlaves->next())
    {
        kdDebug(7006) << " IdleSlave: " << slave->protocol() << " " << slave->host() << slave->port() << endl;

    }
    kDebugInfo(7006, "Jobs in Queue: %d", joblist.count());
    SimpleJob *job = joblist.first();
    for(; job; job = joblist.next())
    {
        kdDebug(7006) << " Job: " << job->url().url() << endl;
    }
}

void Scheduler::_doJob(SimpleJob *job) {
    joblist.append(job);
    kDebugInfo(7006, "Scheduler has now %d jobs %p", joblist.count(), job);
    mytimer.start(0, true);
}
	
void Scheduler::_cancelJob(SimpleJob *job) {
    kDebugInfo(7006, "Scheduler: canceling job %p", job);
    if ( job->slave() ) // was running
    {
        job->slave()->kill();
        _jobFinished( job, job->slave() );
    } else { // was not yet running (don't call this on a finished job!)
        joblist.remove(job);
    }
}

void Scheduler::startStep()
{
    while (joblist.count())
    {
       kDebugInfo(7006, "Scheduling job");
       SimpleJob *job = joblist.at(0);
       QString protocol = job->url().protocol();
       QString host = job->url().host();
       int port = job->url().port();
       QString user = job->url().user();
       QString passwd = job->url().pass();

       bool newSlave = false;

       // Look for matching slave
       Slave *slave = idleSlaves->first();
       for(; slave; slave = idleSlaves->next())
       {
           if ((protocol == slave->protocol()) &&
               (host == slave->host()) &&
               (port == slave->port()) &&
               (user == slave->user()))
              break;
       }

       if (!slave)
       {
          if (slaveList->count() < 5)
          {
             int error;
             QString errortext;
             slave = Slave::createSlave(job->url(), error, errortext);
             if (slave)
             {
                newSlave = true;
                slaveList->append(slave);
                idleSlaves->append(slave);
                connect(slave, SIGNAL(slaveDied(KIO::Slave *)),
			SLOT(slotSlaveDied(KIO::Slave *)));
                connect(slave, SIGNAL(slaveStatus(pid_t,const QCString &,const QString &, bool)),
                        SLOT(slotSlaveStatus(pid_t,const QCString &, const QString &, bool)));
             }
             else
             {
                kDebugFatal(!slave, "couldn't create slave %d %s",
				    error, debugString(errortext));
             }
          }
       }

       if (!slave)
       {
          // Look for slightly matching slave
          slave = idleSlaves->first();
          for(; slave; slave = idleSlaves->next())
          {
             if (protocol == slave->protocol())
                break;
          }
       }

       if (!slave)
       {
          kDebugInfo(7006, "No slaves available");
          debug_info();
	  return;
       }

       idleSlaves->removeRef(slave);
       joblist.removeRef(job);
       kDebugInfo(7006, "scheduler: job started %p", job);
       if ((newSlave) ||
           (slave->host() != host) ||
           (slave->port() != port) ||
           (slave->user() != user) ||
           (slave->passwd() != passwd))
       {
           slave->openConnection(host, port, user, passwd);
	   // halt the queue til we know it's connected
	   slave->queueOnly(true);
	   connect(slave, SIGNAL(connected()),
		   SLOT(slotSlaveConnected()));
       }
       job->start(slave);
    }
}

void Scheduler::slotSlaveConnected() {
    kDebugInfo(7006, "slave connected");
    Slave *slave = (Slave*)sender();
    slave->queueOnly(false);
}

void Scheduler::slotSlaveStatus(pid_t pid, const QCString &protocol, const QString &host, bool connected) {
    kDebugInfo(7006, "slave status");
    Slave *slave = (Slave*)sender();
    kDebugInfo(7006, "Slave = %p (PID = %d) protocol = %s host = %s %s", 
	slave, pid, protocol.data(), host.ascii() ? host.ascii() : "[None]",
	connected ? "Connected" : "Not connected");
}

void Scheduler::_jobFinished(SimpleJob *job, Slave *slave)
{
    slave->disconnect(job);
    if (slave->isAlive())
    {
       idleSlaves->append(slave);
       if (joblist.count())
       {
           kDebugInfo(7006, "Scheduler has now %d jobs", joblist.count());
           mytimer.start(0, true);
       }
       slave->connection()->send( CMD_SLAVE_STATUS );
    }
    else
    {
       slaveList->removeRef(slave);
       delete slave;
    }
}

void Scheduler::slotSlaveDied(KIO::Slave *slave)
{
    idleSlaves->removeRef(slave);
    slaveList->removeRef(slave);
    delete slave;
}

Scheduler* Scheduler::self() {
    if ( !instance )
	instance = new Scheduler;

    return instance;
}

#include "scheduler.moc"
