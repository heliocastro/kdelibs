/*****************************************************************

Copyright (c) 1999 Preston Brown <pbrown@kde.org>
Copyright (c) 1999 Matthias Ettrich <ettrich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include "dcopstub.h"

DCOPStub::DCOPStub( const QCString& app, const QCString& obj )
    : m_app( app ), m_obj( obj ), m_status( CallSucceeded )
{
}

DCOPStub::~DCOPStub()
{
}

DCOPStub::Status DCOPStub::status() const
{
    return m_status;
}

void DCOPStub::setStatus( Status status )
{
    m_status = status;
}

void DCOPStub::callFailed()
{
    setStatus( CallFailed );
}

QCString DCOPStub::app() const
{
    return m_app;
}

QCString DCOPStub::obj() const
{
    return m_obj;
}
