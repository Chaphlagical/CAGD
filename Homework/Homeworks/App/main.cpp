#include <Base.h>

#include "../Homework1/HW1Layer.h"
#include "../Homework2/HW2Layer.h"
#include "../Homework3/HW3Layer.h"
#include "../Homework4/HW4Layer.h"
#include "../Homework5/HW5Layer.h"
#include "../Homework6/HW6Layer.h"
#include "../Homework7/HW7Layer.h"
#include "../Homework8/HW8Layer.h"
#include "../Homework9/HW9Layer.h"
#include "../Homework10/HW10Layer.h"
#include "../Homework11/HW11Layer.h"

#include <Scene/maincamera_layer.h>
#include <Scene/scene_layer.h>

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
	//app->PushLayer(new Chaf::HW7Layer);
	//app->PushLayer(new Chaf::MainCameraLayer);
	//app->PushLayer(new Chaf::SceneLayer);
	//app->PushLayer(new Chaf::HW8Layer);
	//app->PushLayer(new Chaf::HW9Layer);
	//app->PushLayer(new Chaf::HW10Layer);
	//app->PushLayer(new Chaf::HW11Layer);


	app->Run();

	return 0;
}