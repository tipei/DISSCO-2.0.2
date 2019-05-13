/*
LASS (additive sound synthesis library)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
//	XmlReader.cpp
//----------------------------------------------------------------------------//
#ifndef __XML_READER_CPP
#define __XML_READER_CPP

#include "XmlReader.h"

//----------------------------------------------------------------------------//
XmlReader::tagparam::tagparam(char *n, char *v)
{
	name=new char[strlen(n)+1];
	strcpy(name,n);
	if(v)
	{
		value=new char[strlen(v)+1];
		strcpy(value,v);
	}
	else
		value=NULL;

	next=NULL;
}

//----------------------------------------------------------------------------//
XmlReader::tagparam::~tagparam()
{
	if(name)
		delete[] name;

	if(value)
		delete[] value;

	// Recurse and delete
	if(next)
		delete next;
}

//----------------------------------------------------------------------------//
void XmlReader::xmltag::setName(char *in)
{
  //name is const char* so have zname as buffer
	char* zname=new char[strlen(in)+1];
	strcpy(zname,in);
	name = zname;
}

//----------------------------------------------------------------------------//
XmlReader::xmltag::xmltag()
{
	name=NULL;
	params=NULL;
	isClosing=false;
	children=new xmltagset;
}

//----------------------------------------------------------------------------//
XmlReader::xmltag::~xmltag()
{
	destroyTag();
}

//----------------------------------------------------------------------------//
XmlReader::tagparam* XmlReader::xmltag::findParam(const char *pname)
{
	tagparam *tp=params;
	while(tp!=NULL)
	{
		if(!strcasecmp(tp->name,pname))
			return tp;

		tp=tp->next;
	}
	return NULL;
}

//----------------------------------------------------------------------------//
char* XmlReader::xmltag::getParamValue( const char *pname)
{
	tagparam *tp=findParam(pname);
	if(!tp)
		return NULL;

	return tp->value;
}

//----------------------------------------------------------------------------//
bool XmlReader::xmltag::isParamDefined(char *pname)
{
	tagparam *tp=findParam(pname);
	if(!tp)
		return false;

	return true;
}

//----------------------------------------------------------------------------//
void XmlReader::xmltag::destroyTag()
{
	if(name)
		delete[] name;

	tagparam *tp=params;
	while(tp)
	{
		tagparam *next=tp->next;
		delete tp;
		tp=next;
	}

	delete children;

	name=NULL;
	params=NULL;
	isClosing=false;

	children=new xmltagset;

}

//----------------------------------------------------------------------------//
char* XmlReader::xmltag::findChildParamValue(const char *childName,const char *paramName)
{
	xmltag *child=children->find(childName);
	if(!child)
		return NULL;

	return child->getParamValue(paramName);
}

//----------------------------------------------------------------------------//
XmlReader::xmltagset::xmltagset()
{
	tag=NULL;
	next=NULL;
	curSearch=NULL;
	searchName=NULL;
}


XmlReader::xmltagset::~xmltagset()
{
	// Delete the tag
	if(tag)
		delete tag;

	// Recurse and delete
	if(next)
		delete next;
}

//----------------------------------------------------------------------------//
void XmlReader::xmltagset::add(xmltag *itag)
{
	if(!tag)
	{
		tag=itag;
		return;
	}
	xmltagset *set=this;
	while(set->next)
		set=set->next;

	set->next=new xmltagset;
	set->next->tag=itag;	
}

//----------------------------------------------------------------------------//
XmlReader::xmltag* XmlReader::xmltagset::find(const char *name)
{
	xmltag *tag;
	if(searchName)
	{
		if(!strcmp(searchName,name))
		{
			tag=auxfind(curSearch,name);
			if(!tag)
			{
				curSearch=NULL;
				searchName=NULL;
				return NULL;
			}
			return tag;
		}	
	}

	searchName=name;
	tag=auxfind(this,name);
	if(!tag)
	{	
		searchName=NULL;
		curSearch=NULL;
		return NULL;
	}
	return tag;
}

//----------------------------------------------------------------------------//
XmlReader::xmltag* XmlReader::xmltagset::auxfind(xmltagset *set, const char *name)
{
	if(!set)
		return NULL;
	if(!set->tag)
		return NULL;

	if(!strcmp(name,set->tag->name))
	{
		curSearch=set->next;
		return set->tag;
	}
	return auxfind(set->next,name);
}


//----------------------------------------------------------------------------//
XmlReader::XmlReader()
{
	fp=NULL;
	inputbuffer=new char[XML_BUFFER_SIZE];
	tagbuffer=new char[XML_BUFFER_SIZE];
	nibuf=0;
	memset(inputbuffer,0,XML_BUFFER_SIZE);
	memset(tagbuffer,0,XML_BUFFER_SIZE);
}

//----------------------------------------------------------------------------//
XmlReader::~XmlReader()
{
	closeFile();

	delete[] inputbuffer;
	delete[] tagbuffer;
}

//----------------------------------------------------------------------------//
bool XmlReader::openFile(char *file)
{
	if(fp)
		return false;
	
	fp=fopen(file,"r");
	if(!fp)
		return false;

	return true;
}

//----------------------------------------------------------------------------//
bool XmlReader::closeFile()
{
	if(fp)
	{
		fclose(fp);
		fp=NULL;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------//
void XmlReader::dewhitespace(char *c)
{
	int l=strlen(c);
	bool wsok=false;
	int nl=0;

	for(int i=0;i<l;i++)
	{
		if(c[i]==' ' || c[i]=='\t' || c[i]=='\r' || c[i]=='\n')
		{
			if(wsok)
			{
				c[nl]=' ';
				nl++;
				wsok=false;
			}
			else
				continue;
		}
		else
		{
			c[nl]=c[i];
			nl++;
			wsok=true;
		}
	}
	c[nl]=0;
}

//----------------------------------------------------------------------------//
bool XmlReader::fillTagBuffer()
{
	char *start,*end;

	start=index(inputbuffer,'<');
	end=index(inputbuffer,'>');

	// Lets find a tag!
	if(!start)
	{
		while(!start)
		{
			#ifdef XML_DEBUG
			printf("[%s:%d] Recycling buffer\n",__FILE__,__LINE__);
			#endif

			// Effectively dump the buffer, grab new data
			if(!fgets(inputbuffer,XML_BUFFER_SIZE-nibuf,fp))
				return false;
		
			start=index(inputbuffer,'<');
		}

		nibuf=strlen(inputbuffer);
		end=index(inputbuffer,'>');
	}
	
	while(!end)
	{
		#ifdef XML_DEBUG
		printf("[%s:%d] Adding to buffer\n",__FILE__,__LINE__);
		#endif

		// What if nibuf>XML_BUFFER_SIZE?
		if(!fgets(inputbuffer+nibuf,XML_BUFFER_SIZE-nibuf,fp))
			return false;
		
		nibuf=strlen(inputbuffer);
		end=index(inputbuffer,'>');
	}
	
	// okay, now have and end and a beginning pointer, and a valid nibuf
	// that is the buffered amount of data we currently have
	
	#ifdef XML_DEBUG
	// perform sanity checks
	if(start>=end)
		//printf("[%s:%d] FAIL! Start: %X, End: %X, nibuf: %d\n",__FILE__,__LINE__,start,end,nibuf);
    std::cout<<"["<<__FILE__<<":"<<__LINE__<<"] FAIL! Start: "<<start<<", End: "<<end<<", nibuf: "<<nibuf<<std::endl;
	if(start<inputbuffer || end<inputbuffer)
		//printf("[%s:%d] FAIL! Start: %X, End: %X, nibuf: %d\n",__FILE__,__LINE__,start,end,nibuf);
	  std::cout<<"["<<__FILE__<<":"<<__LINE__<<"] FAIL! Start: "<<start<<", End: "<<end<<", nibuf: "<<nibuf<<std::endl;

	if(nibuf>XML_BUFFER_SIZE || nibuf<0)
		//printf("[%s:%d] FAIL! Start: %X, End: %X, nibuf: %d\n",__FILE__,__LINE__,start,end,nibuf);
	  std::cout<<"["<<__FILE__<<":"<<__LINE__<<"] FAIL! Start: "<<start<<", End: "<<end<<", nibuf: "<<nibuf<<std::endl;

	#endif

	memcpy(tagbuffer,start,end-start+1);
	memmove(inputbuffer,end+1,nibuf-(end-inputbuffer)+1);

	nibuf-=end-inputbuffer+1;

	tagbuffer[end-start+1]=0;
	dewhitespace(tagbuffer);

	return true;
}

//----------------------------------------------------------------------------//
// This is the tag parser.
// It admittedly is not very good.
// There are known issues with it, some formatting can break it.
// Time was not spent to fix them because lass never really required it.
// In other words, this is a pretty "lass specific" reader insteead of a
// general purpose reader.
//
// A space within a value in a tag parameter is a problem:
//  OKAY: <tag param='value'>
//  UHOH: <tag param='val ue'>
bool XmlReader::readTag(xmltag *tag)
{
	//xmltag *tag=new xmltag;
	tag->destroyTag();
	
	if(!fillTagBuffer())
		return false;

	char *tagstart=index(tagbuffer,'<')+1;

	// HACK
	// Since Xerces doesn't put a space before the closing tag slash
	// (<tag/> instead of <tag />), this checks for the closing condition
	// and should make both occasions work.
	// Check for />, and make / a > and > null.
	char *end=index(tagstart,'>')-1;
	if(end[0]=='/')
	{
		tag->isClosing=true;
		end[0]='>';
		end[1]=0;
	}

	char *n=strtok(tagstart," >");	
	if(n[0]=='/')
	{
		tag->setName(n+1);
		tag->isClosing=true;
	}
	else
		tag->setName(n);

	tagparam *tp=NULL;
	while((n = strtok(NULL, " >")) != 0)
	{
		if(!strcmp(n,"/"))
		{
			tag->isClosing=true;
		}
		// Dont even try and figure out what a blank space is
		else if(!strcmp(n," "))
			continue;
		else
		{
			char *eq=index(n,'=');
			if(eq==NULL)
				tp=new tagparam(n,NULL);
			else
			{
				eq[0]=0;
				if(eq[1]=='\"' || eq[1]=='\'')
				{
					char *eqend=eq+strlen(eq+1);
					if(eqend[0]=='\"' || eqend[0]=='\'')
						eqend[0]=0;
					else
					{
						#ifdef XML_DEBUG
						printf("Warning: No closequote found on parameter for %s\n",n);
						#endif
					}
					tp=new tagparam(n,eq+2);
				}
				else
				{
					#ifdef XML_DEBUG
					printf("Warning: No openquote found on parameter for %s\n",n);
					#endif
					tp=new tagparam(n,eq+1);
				}
			}
			tp->next=tag->params;
			tag->params=tp;
		}
	}
	
	return true;
		
}
//----------------------------------------------------------------------------//

XmlReader::xmltagset* XmlReader::readXMLDocument()
{
	//xmltagset *set=new xmltagset;

	xmltag *tag=new xmltag;
	tag->name="ROOT";
	readXMLDocument(tag);
	
	return tag->children;
}

void XmlReader::readXMLDocument(XmlReader::xmltag *tag)
{
	xmltag *ctag=new xmltag;
	while(readTag(ctag))
	{
		if(!ctag->isClosing)
		{
			readXMLDocument(ctag);
			tag->children->add(ctag);
		}
		else if(!strcmp(ctag->name,tag->name) && ctag->isClosing)
		{
			tag->children->add(ctag);
			return;
		}
		else
		{
			tag->children->add(ctag);	
		}

		ctag=new xmltag;
	}

	return;
}

#endif //__XML_READER_CPP
