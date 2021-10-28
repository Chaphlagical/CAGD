#include <Base.h>

#include "../Homework1/HW1Layer.h"
#include "../Homework2/HW2Layer.h"
#include "../Homework3/HW3Layer.h"
#include "../Homework4/HW4Layer.h"
#include "../Homework5/HW5Layer.h"
#include "../Homework6/HW6Layer.h"

int main()
{
	Chaf::Log::Init();

	auto app = Chaf::Create();

	app->PushLayer(new Chaf::HW1Layer);
	app->PushLayer(new Chaf::HW2Layer);
	app->PushLayer(new Chaf::HW3Layer);
	app->PushLayer(new Chaf::HW4Layer);
	app->PushLayer(new Chaf::HW5Layer);
	app->PushLayer(new Chaf::HW6Layer);

	app->Run();

	return 0;
}