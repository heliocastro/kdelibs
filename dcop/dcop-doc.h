/*
	A dummy source file for documenting the library.
	Copied from HOWTO with small syntactic changes.
*/

/**
 @libdoc The DCOP Desktop COmmunication Protocol library
DCOP is a simple IPC/RPC mechanism built to operate over sockets.
Either unix domain sockets or tcp/ip sockets are supported. DCOP is
built on top of the Inter Client Exchange (ICE) protocol, which comes
standard as a part of X11R6 and later. It also depends on Qt, but
beyond that it does not require any other libraries. Because of this,
it is extremely lightweight, enabling it to be linked into all KDE
applications with low overhead.

Model:

The model is simple.  Each application using DCOP is a client.  They
communicate to each other through a DCOP server, which functions like
a traffic director, dispatching messages/calls to the proper
destinations.  All clients are peers of each other.

Two types of actions are possible with DCOP: "send and forget"
messages, which do not block, and "calls," which block waiting for
some data to be returned.

Any data that will be sent is serialized (marshalled, for you CORBA
types) using the built-in QDataStream operators available in all of
the Qt classes.  This is fast and easy.  In fact it's so little work
that you can easily write the marshalling code by hand. In addition,
there's a simple IDL-like compiler available (dcopidl and dcopidl2cpp)
that generates stubs and skeletons for you. Using the dcopidl compiler
has the additional benefit of type safety.

This HOWTO describes the manual method first and covers the dcopidl
compiler later.

Establishing the Connection:

KApplication has gained a method called "KApplication::dcopClient()"
which returns a pointer to a DCOPClient instance.  The first time this
method is called, the client class will be created.  DCOPClients have
unique identifiers attached to them which are based on what
KApplication::name() returns.  In fact, if there is only a single
instance of the program running, the appId will be equal to
KApplication::name().

To actually enable DCOP communication to begin, you must use
DCOPClient::attach().  This will attempt to attach to the DCOP server.
If no server is found or there is any other type of error, attach()
will return false. KApplication will catch a dcop signal and display an
appropriate error message box in that case.

After connecting with the server via DCOPClient::attach(), you need to
register this appId with the server so it knows about you.  Otherwise,
you are communicating anonymously.  Use the
DCOPClient::registerAs(const QCString &name) to do so.  In the simple
case:

<pre>
appId = client->registerAs(kApp->name());
</pre>

If you never retrieve the DCOPClient pointer from KApplication, the
object will not be created and thus there will be no memory overhead.

You may also detach from the server by calling DCOPClient::detach().
If you wish to attach again you will need to re-register as well.  If
you only wish to change the ID under which you are registered, simply
call DCOPClient::registerAs() with the new name.

KUniqueApplication automatically registers itself to DCOP. If you
are using KUniqueApplication you should not attach or register
yourself, this is already done. The appId is by definition
equal to kapp->name(). You can retrieve the registered DCOP client
by calling kapp->dcopClient().

Sending Data to a Remote Application:

To actually communicate, you have one of two choices.  You may either
call the "send" or the "call" method.  Both methods require three
identification parameters: an application identifier, a remote object,
a remote function. Sending is asynchronous (i.e. it returns immediately)
and may or may not result in your own application being sent a message at
some point in the future. Then "send" requires one and "call" requires
two data parameters.

The remote object must be specified as an object hierarchy.  That is,
if the toplevel object is called "fooObject" and has the child
"barObject", you would reference this object as "fooObject/barObject".
Functions must be described by a full function signature.  If the
remote function is called "doIt", and it takes an int, it would be
described as "doIt(int)".  Please note that the return type is not
specified here, as it is not part of the function signature (or at
least the C++ understanding of a function signature).  You will get
the return type of a function back as an extra parameter to
DCOPClient::call().  See the section on call() for more details.

In order to actually get the data to the remote client, it must be
"serialized" via a QDataStream operating on a QByteArray. This is how
the data parameter is "built". A few examples will make clear how this
works.

Say you want to call "doIt" as described above, and not block (or wait
for a response).  You will not receive the return value of the remotely
called function, but you will not hang while the RPC is processed either.
The return value of send() indicates whether DCOP communication succeeded
or not.

<pre>
QByteArray data;
QDataStream arg(data, IO_WriteOnly);
arg << 5;
if (!client->send("someAppId", "fooObject/barObject", "doIt(int)",
	          data))
  qDebug("there was some error using DCOP.");
</pre>

OK, now let's say we wanted to get the data back from the remotely
called function.  You have to execute a call() instead of a send().
The returned value will then be available in the data parameter "reply".
The actual return value of call() is still whether or not DCOP
communication was successful.

<pre>
QByteArray data, replyData;
QCString replyType;
QDataStream arg(data, IO_WriteOnly);
arg << 5;
if (!client->call("someAppId", "fooObject/barObject", "doIt(int)",
                  data, replyType, replyData))
  qDebug("there was some error using DCOP.");
else {
  QDataStream reply(replyData, IO_ReadOnly);
  if (replyType == "QString") {
    QString result;
    reply >> result;
    print("the result is: %s",result.latin1());
  } else
    qDebug("doIt returned an unexpected type of reply!");
}
</pre>

Receiving Data via DCOP:

Currently the only real way to receive data from DCOP is to multiply
inherit from the normal class that you are inheriting (usually some
sort of QWidget subclass or QObject) as well as the DCOPObject class.
DCOPObject provides one very important method: DCOPObject::process().
This is a pure virtual method that you must implement in order to
process DCOP messages that you receive.  It takes a function
signature, QByteArray of parameters, and a reference to a QByteArray
for the reply data that you must fill in.

Think of DCOPObject::process() as a sort of dispatch agent.  In the
future, there will probably be a precompiler for your sources to write
this method for you.  However, until that point you need to examine
the incoming function signature and take action accordingly.  Here is
an example implementation.

<pre>
bool BarObject::process(const QCString &fun, const QByteArray &data,
		        QCString &replyType, QByteArray &replyData)
{
  if (fun == "doIt(int)") {
    QDataStream arg(data, IO_ReadOnly);
    int i; // parameter
    arg >> i;
    QString result = self->doIt (i);
    QDataStream reply(replyData, IO_WriteOnly);
    reply << result;
    replyType = "QString";
    return true;
  } else {
    qDebug("unknown function call to BarObject::process()");
    return false;
  }
}
</pre>

Receiving Calls and processing them:

If your applications is able to process incoming function calls
right away the above code is all you need. When your application
needs to do more complex tasks you might want to do the processing
out of 'process' function call and send the result back later when
it becomes available.

For this you can ask your DCOPClient for a transactionId. You can 
then return from the 'process' function and when the result is 
available finish the transaction. In the mean time your application
can receive incoming DCOP function calls from other clients.

Such code could like this:

<pre>
bool BarObject::process(const QCString &fun, const QByteArray &data,
		        QCString &, QByteArray &)
{
  if (fun == "doIt(int)") {
    QDataStream arg(data, IO_ReadOnly);
    int i; // parameter
    arg >> i;
    QString result = self->doIt(i);
    
    DCOPClientTransaction *myTransaction;
    myTransaction = kapp->dcopClient()->beginTransaction();

    // start processing...
    // Calls slotProcessingDone when finished.
    startProcessing( myTransaction, i);

    return true;
  } else {
    qDebug("unknown function call to BarObject::process()");
    return false;
  }
}

slotProcessingDone(DCOPClientTransaction *myTransaction, const QString &result)
{
    QCString replyType = "QString";
    QByteArray replyData;  
    QDataStream reply(replyData, IO_WriteOnly);
    reply << result;
    kapp->dcopClient()->endTransaction( myTransaction, replyType, replyData );
}
</pre>

Using the dcopidl compiler

TODO

Conclusion:

Hopefully this document will get you well on your way into the world
of inter-process communication with KDE!  Please direct all comments
and/or suggestions to Preston Brown <pbrown@kde.org> and Matthias
Ettrich <ettrich@kde.org>.


Inter-user communication

Sometimes it might be interesting to use DCOP between processes
belonging to different users, e.g. a frontend process running
with the user's id, and a backend process running as root.

To do this, two steps have to be taken:

a) both processes need to talk to the same DCOP server
b) the authentication must be ensured

For the first step, you simply pass the server address (as
found in .DCOPserver) to the second process. For the authentication,
you can use the ICEAUTHORITY environment variable to tell the
second process where to find the authentication information.
(Note that this implies that the second process is able to
read the authentication file, so it will probably only work
if the second process runs as root. If it should run as another
user, a similar approach to what kdesu does with xauth must
be taken. In fact, it would be a very good idea to add DCOP
support to kdesu!)

For example

ICEAUTHORITY=~user/.ICEauthority kdesu root -c kcmroot -dcopserver `cat ~user/.DCOPserver`

will, after kdesu got the root password, execute kcmroot as root, talking
to the user's dcop server.


NOTE: DCOP communication is not encrypted, so please do not
pass important information around this way.


Performance Tests:

A few back-of-the-napkin tests folks:

Code:

<pre>
#include <kapp.h>

int main(int argc, char **argv)
{
  KApplication *app;

  app = new KApplication(argc, argv, "testit");
  return app->exec();
}
</pre>

Compiled with:

<pre>
g++ -O2 -o testit testit.cpp -I$QTDIR/include -L$QTDIR/lib -lkdecore
</pre>

on Linux yields the following memory use statistics:

<pre>
VmSize:     8076 kB
VmLck:         0 kB
VmRSS:      4532 kB
VmData:      208 kB
VmStk:        20 kB
VmExe:         4 kB
VmLib:      6588 kB
</pre>

If I create the KApplication's DCOPClient, and call attach() and
registerAs(), it changes to this:

<pre>
VmSize:     8080 kB
VmLck:         0 kB
VmRSS:      4624 kB
VmData:      208 kB
VmStk:        20 kB
VmExe:         4 kB
VmLib:      6588 kB
</pre>

Basically it appears that using DCOP causes 100k more memory to be
resident, but no more data or stack.  So this will be shared between all
processes, right?  100k to enable DCOP in all apps doesn't seem bad at
all. :)

OK now for some timings.  Just creating a KApplication and then exiting
(i.e. removing the call to KApplication::exec) takes this much time:

<pre>
0.28user 0.02system 0:00.32elapsed 92%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (1084major+62minor)pagefaults 0swaps
</pre>

I.e. about 1/3 of a second on my PII-233.  Now, if we create our DCOP
object and attach to the server, it takes this long:

<pre>
0.27user 0.03system 0:00.34elapsed 87%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (1107major+65minor)pagefaults 0swaps
</pre>

I.e. about 1/3 of a second.  Basically DCOPClient creation and attaching
gets lost in the statistical variation ("noise").  I was getting times
between .32 and .48 over several runs for both of the example programs, so
obviously system load is more relevant than the extra two calls to
DCOPClient::attach and DCOPClient::registerAs, as well as the actual
DCOPClient constructor time.

*/
