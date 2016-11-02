#include "road_name.h"
#include "tinyxml2.h"

using namespace tinyxml2;

#define SCHEME_ROOT "road_name_scheme"


#define USING_POINTER


void load_tns(char *filename, char *elem, char *subelem, char **text)
{
#ifdef USING_POINTER
	XMLDocument* doc = new XMLDocument(true, COLLAPSE_WHITESPACE);
	doc->LoadFile( filename );
	
	XMLElement* element = doc->RootElement();
	
	doc->PrintError();
	
#else
	XMLDocument doc( true, COLLAPSE_WHITESPACE );
	
	doc.LoadFile( filename );
	//doc.Print();
	
	const XMLElement* element = doc.RootElement();
	
	doc.PrintError();
	
#endif
	
	//printf(":::::==> %s \n", element->GetText());
	//printf(":::::==> %s \n", element->Value());
	//printf(":::::==> %s \n", element->FirstChildElement()->Value());
	//printf(":::::==> %s \n", element->FirstChildElement()->GetText());
	//printf(":::::==> %s \n", element->FirstChildElement()->FirstChild()->Value());
	//`printf(":::::==> %s \n", element->FirstChildElement()->FirstChildElement()->GetText());
	//printf(":::::==> %s \n", element->FirstChildElement(elem)->FirstChildElement(subelem)->GetText());
	
	const char *contents = element->FirstChildElement(elem)->FirstChildElement(subelem)->GetText();
	//const char *contents = "aa";
	
	//printf(":::::==> %s \n", contents);
	memcpy(*text, contents, strlen(contents) + 1);
	//strcpy(*text, contents);
	//*text = (char*)contents;
	
	
	//XMLText* text = doc.FirstChildElement()->FirstChild()->ToText();
	//printf(":::::==> %s \n", text->Value());
	//doc.FirstChildElement()->FirstChildElement()->FirstChild()->Value()
#ifdef USING_POINTER
	delete doc;
#endif
	return;
}
