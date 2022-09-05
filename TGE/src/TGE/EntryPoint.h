#pragma once
#ifdef TGE_PLATFORM_WINDOWS

extern TGE::Application* TGE::CreateApplication();

int main(int argc, char** argv) {
	
	TGE::log::Init();
	TGE_CORE_WARN("Initialized Log!");
	//TGE::log::GetCoreLogger()->warn("Initialized Log!");
	int a = 5;
	TGE_INFO("Hello! Var={0}", a);
	//TGE::log::GetClientLogger()->info("Hello! Var=5");
	auto app = TGE::CreateApplication();
	app->Run();
	delete app;
}
#endif
