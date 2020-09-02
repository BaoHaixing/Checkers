#include"header.h"

int main()
{
	//initgraph(576 + 160+200+200, 665 + 80,SHOWCONSOLE);
	initgraph(576 + 160 + 200 + 200, 665 + 80);

	ENGINE* Engine;
	Engine = new ENGINE;
	Engine->Run();
	return 0;
}