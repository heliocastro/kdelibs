/*****************************************************************
Copyright (c) 1999 Torben Weis <weis@kde.org>
Copyright (c) 2000 Matthias Ettrich <ettrich@kde.org>

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
#include <qdom.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"

/**
 * Writes the skeleton
 */
void generateSkel( const QString& idl, const QString& filename, QDomElement de )
{
    QFile skel( filename );
    if ( !skel.open( IO_WriteOnly ) )
	qFatal("Could not write to %s", filename.latin1() );

    QTextStream str( &skel );
    
    str << "/****************************************************************************" << endl;
    str << "**" << endl;
    str << "** DCOP Skeleton created by dcopidl2cpp from " << idl << endl;
    str << "**" << endl;
    str << "** WARNING! All changes made in this file will be lost!" << endl;
    str << "**" << endl;
    str << "*****************************************************************************/" << endl;
    str << endl;
    
    QDomElement e = de.firstChild().toElement();
    if ( e.tagName() == "SOURCE" ) {
	str << "#include \"" << e.firstChild().toText().data() << "\"" << endl << endl;
    }

    for( ; !e.isNull(); e = e.nextSibling().toElement() ) {
	if ( e.tagName() == "CLASS" ) {
	    QDomElement n = e.firstChild().toElement();
	    ASSERT( n.tagName() == "NAME" );
	    QString className = n.firstChild().toText().data();
	    // find dcop parent ( rightmost super class )
	    QString DCOPParent;
	    QDomElement s = n.nextSibling().toElement();
	    for( ; !s.isNull(); s = s.nextSibling().toElement() ) {
		if ( s.tagName() == "SUPER" )
		    DCOPParent = s.firstChild().toText().data();
	    }
	    
	    // Write dispatcher
	    str << "bool " << className;
	    str << "::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)" << endl;
	    str << "{" << endl;
	    s = n.nextSibling().toElement();
	    QStringList funcNames;
	    for( ; !s.isNull(); s = s.nextSibling().toElement() ) {
		if ( s.tagName() == "FUNC" ) {
		    QDomElement r = s.firstChild().toElement();
		    ASSERT( r.tagName() == "TYPE" );
		    QString result = r.firstChild().toText().data();
		    r = r.nextSibling().toElement();
		    ASSERT ( r.tagName() == "NAME" );
		    QString funcName = r.firstChild().toText().data();
		    QStringList args;
		    QStringList argtypes;
		    r = r.nextSibling().toElement();
		    for( ; !r.isNull(); r = r.nextSibling().toElement() ) {
			ASSERT( r.tagName() == "ARG" );
			QDomElement a = r.firstChild().toElement();
			ASSERT( a.tagName() == "TYPE" );
			argtypes.append( a.firstChild().toText().data() );
			a = a.nextSibling().toElement();
			ASSERT ( a.tagName() == "NAME" );
			args.append ( a.firstChild().toText().data() );
		    }
		    QString plainFuncName = funcName;
		    funcName += "(";
		    bool first = TRUE;
		    for( QStringList::Iterator it = argtypes.begin(); it != argtypes.end(); ++it ){
			if ( !first )
			    funcName += ",";
			first = FALSE;
			funcName += *it;
		    }
		    funcName += ")";
		    funcNames.append( funcName );
			
		    str << "    if ( fun == \"" << funcName << "\" ) {" << endl;
		    if ( !args.isEmpty() ) {
			QStringList::Iterator ittypes = argtypes.begin();
			for( QStringList::Iterator it = args.begin(); it != args.end(); ++it ){
			    str << "\t"<< *ittypes << " " << *it << ";" <<  endl;
			    ++ittypes;
			}
			str << "\tQDataStream arg( data, IO_ReadOnly );" << endl;
			for( QStringList::Iterator it = args.begin(); it != args.end(); ++it ){
			    str << "\targ >> " << *it << ";" << endl;
			}
		    }

		    str << "\treplyType = \"" << result << "\";" << endl;
		    if ( result == "void" ) {
			str << "\t" << plainFuncName << "(";
		    } else {
			str << "\tQDataStream reply( replyData, IO_WriteOnly );"  << endl;
			str << "\treply << " << plainFuncName << "(";
		    }

		    first = TRUE;
		    for( QStringList::Iterator it = args.begin(); it != args.end(); ++it ){
			if ( !first )
			    str << ", ";
			first = FALSE;
			str << *it;
		    }
		    str << " );" << endl;
		    str << "\treturn TRUE;" << endl;
		    str << "    }" << endl;
		}
	    }

	    if (!DCOPParent.isEmpty()) {
		str << "    return " << DCOPParent << "::process( fun, data, replyType, replyData );" << endl;
	    } else {
		str << "    return FALSE;" << endl;
	    }
	    str << "}" << endl << endl;
	    
	    str << "QCString " << className;
	    str << "::functions()" << endl;
	    str << "{" << endl;
	    if (!DCOPParent.isEmpty()) {
		str << "    return " << DCOPParent << "::functions() + " << endl;
	    } else {
		str << "    return " << endl;
	    }
	    for( QStringList::Iterator it = funcNames.begin(); it != funcNames.end(); ++it ){
		str << "\t\"" << *it << ";\"" << endl;
	    }
	    str << "\t;" << endl;
	    str << "}" << endl << endl;
	}
    }
	
    skel.close();
}
