// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "kprocio.h"

#include <kdebug.h>
#include <qtextcodec.h>

KProcIO::KProcIO ( QTextCodec *_codec)
  : codec(_codec)
{
  rbi=0;
  readsignalon=writeready=TRUE;
  
  if (!codec)
  {
     codec = QTextCodec::codecForName("ISO 8859-1");
     if (!codec)
     {
        kdError(750) << "Can't create ISO 8859-1 codec!" << endl;
     }
  }
}

void
KProcIO::resetAll ()
{
  if (isRunning())
    kill();

  clearArguments();
  rbi=0;
  readsignalon=writeready=TRUE;

  disconnect (this, SIGNAL (receivedStdout (KProcess *, char *, int)),
	   this, SLOT (received (KProcess *, char *, int)));

  disconnect (this, SIGNAL (wroteStdin(KProcess *)),
	   this, SLOT (sent (KProcess *)));

  qlist.clear();

}

bool KProcIO::start (RunMode runmode)
{
  connect (this, SIGNAL (receivedStdout (KProcess *, char *, int)),
	   this, SLOT (received (KProcess *, char *, int)));

  connect (this, SIGNAL (wroteStdin(KProcess *)),
	   this, SLOT (sent (KProcess *)));
           
  qlist.setAutoDelete (TRUE);

  return KProcess::start (runmode, KProcess::All);
}

bool KProcIO::writeStdin (const QString &line, bool appendnewline)
{
  QCString qs( codec->fromUnicode(line));
  if (appendnewline)
    qs+='\n';

  qlist.append (qs.data());

  //kdDebug(750) << "KPIO::write [" << buffer << "],[" << qlist.current() << "]" << endl;

  if (writeready)
    {
      kdDebug(750) << "really writing" << endl;
      writeready=FALSE;
      return KProcess::writeStdin (qlist.current(),
				   strlen (qlist.current()));
    }
  kdDebug(750) << "NOT really writing" << endl;
  return TRUE;
}

void KProcIO::sent (KProcess *)
{
  if (qlist.first())     kdDebug(750) << "KP::sent [" << qlist.first() << "]" << endl;

  qlist.removeFirst();

  if (qlist.count()==0)
    {
            kdDebug(750) << "Empty" << endl;
      writeready=TRUE;
    }
  else
    {
      kdDebug(750) << "Sending [" << qlist.first() << "]" << endl;
      KProcess::writeStdin (qlist.first(), strlen (qlist.first()));
    }

}

void KProcIO::received (KProcess *, char *buffer, int buflen)
{
  int i;

  buffer [buflen]='\0';

  kdDebug(750) << "KPIO: recv'd [" << buffer << "]" << endl;

  for (i=0;i<buflen;i++)
    recvbuffer+=buffer [i];

  controlledEmission();
}

void KProcIO::ackRead ()
{
  readsignalon=TRUE;
  if (needreadsignal || recvbuffer.length()!=0)
    controlledEmission();
}

void KProcIO::controlledEmission ()
{
  if (readsignalon)
    {
      needreadsignal=FALSE;
      readsignalon=FALSE; //will stay off until read is acknowledged
      emit readReady (this);
    }
  else
    needreadsignal=TRUE;
  
}

void KProcIO::enableReadSignals (bool enable)
{
  readsignalon=enable;

  if (enable && needreadsignal)
	emit readReady (this);
}

int KProcIO::readln (QString &line, bool autoAck)
{
  int len;

  if (autoAck)
    readsignalon=TRUE;

  //need to reduce the size of recvbuffer at some point...

  len=recvbuffer.find ('\n',rbi)-rbi;

  kdDebug(750) << "KPIO::readln\n" << endl;

  //in case there's no '\n' at the end of the buffer
  if (len<0 && (unsigned int)rbi<recvbuffer.length())
    {
      QString qs=recvbuffer.mid (rbi,recvbuffer.length()-rbi);
      recvbuffer=qs;
      rbi=0;
      return -1;
    }

  if (len>=0)
    {
      line = codec->toUnicode(recvbuffer.mid(rbi,len).ascii(), len);   
      rbi += len+1;
      return len;
    }
  
  recvbuffer="";
  rbi=0;

  //-1 on return signals "no more data" not error
  return -1;
    
}
#include "kprocio.moc"

