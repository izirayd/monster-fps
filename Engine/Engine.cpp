#include "App.hpp"
#include "engine_config.hpp"

int main(void)
{
	CApplication App;
	CError Error = App.Init(NAME_ENGINE, 1024, 768);

	if (Error)
	{
		fprintf(stderr, "%s\n", (const char*)Error);
		getchar();
		App.EndWork();
	}

	Error = App.Render();

	if (Error)
	{
		fprintf(stderr, "%s\n", (const char*)Error);
		getchar();
		App.EndWork();
	}

	return 0;
}