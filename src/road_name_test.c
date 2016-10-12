#include "test.h"

#include "tinyxml2.h"

using namespace tinyxml2;

void pre_load_scheme(char *filename)
{
	XMLDocument* doc = new XMLDocument();
	
	doc->LoadFile( filename );
	
	doc->Print();
}

int main(int argc, char **argv)
{


	smpl_unzip("");
	
	pre_load_scheme("");
	
	return 0;
}