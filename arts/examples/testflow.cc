    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "artsflow.h"
#include "artsserver_impl.h"
#include "debug.h"
#include <stdio.h>

vector<long> connect(long l)
{
	vector<long> connseq;
	connseq.push_back(l);
	return connseq;
}

int main()
{
	Dispatcher dispatcher;

	PortType pt;
	vector<long> nothingConnected;
	vector<ModuleDesc *> mdseq;

	// Module #1: Synth_FREQUENCY(frequency=440.0) => (pos)
	{
		vector<PortDesc *> pdseq;
		pt = PortType(input,audio_data,conn_stream,false);
		pdseq.push_back(
			new PortDesc(10,"frequency",pt,false,true,nothingConnected,440.0,"")
		);
		pt = PortType(output,audio_data,conn_stream,false);
		pdseq.push_back(
			new PortDesc(11,"pos",pt,true,false,connect(20),440.0,"")
		);
		mdseq.push_back(
			new ModuleDesc(12,"Synth_FREQUENCY",pdseq,0,0,0,0,false,false)
		);
	}

	// Module #2: Synth_WAVE_SIN(pos from Module #1) => (outvalue)
	{
		vector<PortDesc *> pdseq;
		pt = PortType(input,audio_data,conn_stream,false);
		pdseq.push_back(
			new PortDesc(20,"pos",pt,true,false,connect(11),0.0,"")
		);
		pt = PortType(output,audio_data,conn_stream,false);
		pdseq.push_back(
			new PortDesc(21,"outvalue",pt,true,false,connect(1),0.0,"")
		);

		mdseq.push_back(
			new ModuleDesc(22,"Synth_WAVE_SIN",pdseq,0,0,0,0,false,false)
		);
	}

	// Module #3: Synth_PLAY(invalue_left from Module #2, invalue_right=0.0)
	{
		vector<PortDesc *> pdseq;
	
		pt = PortType(input,audio_data,conn_stream,false);
		pdseq.push_back(
			new PortDesc(1,"invalue_left",pt,true,false,connect(21),0.0,"")
		);
		pdseq.push_back(
			new PortDesc(2,"invalue_right",pt,false,true,
											nothingConnected,0.0,"")
		);
		pt = PortType(input,string_data,conn_property,false);
		pdseq.push_back(
			new PortDesc(3,"channels",pt,false,true,nothingConnected,0.0,"2")
		);
	
		mdseq.push_back(
			new ModuleDesc(4,"Synth_PLAY",pdseq,0,0,0,0,false,false)
		);
	}

	StructureDesc sd(0,0,mdseq,"simpletest",false);

/*
	Buffer b;
	sd.writeType(b);
	printf("%s\n",b.toString("StructureDesc").c_str());

*/
	ExecutionManager_var eman = ExecutionManager::_create();

	setartsdebug(true);
	long id = eman->createStructure(sd);
	printf("id = %ld\n",id);

	/* remove this (test only) */
	artsServerRun();

	printf("=> entering dispatcher.run();\n");
	dispatcher.run();
}
