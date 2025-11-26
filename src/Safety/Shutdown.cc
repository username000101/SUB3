#include "Safety.h"

#include <plog/Log.h>

#include "Config/Config.h"

void sub::safety::Safety::Shutdown(int code, const char* reason)
{
	if (reason) LOGW << "Shutting down SUB by reason: " << reason << std::endl;
	if (auto cfg = config::Configuration::Get(); cfg.processes)
	{
		for (auto& proc : *cfg.processes)
		{
			if (proc)
			{
				LOGI << "Killing process with PID " << proc->pid << std::endl;
				proc->kill();
			}
		}
	}

	std::exit(code);
}
