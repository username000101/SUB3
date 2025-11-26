#include "Config.h"

#include <plog/Log.h>

void sub::config::Configuration::KillProcesses()
{
	if (!config.processes) return;

	for (auto& process : *config.processes)
	{
		if (process)
		{
			LOGI << "Killing process " << process->pid << std::endl;
			process->kill();
		}
	}
}
