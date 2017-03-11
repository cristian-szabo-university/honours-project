#pragma once

#include "Config.hpp"

#include "Core/Platform.hpp"
#include "Core/Charset.hpp"
#include "Core/Logger.hpp"
#include "Core/HashFactory.hpp"
#include "Core/AttackFactory.hpp"
#include "Core/HashCracker.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/DeviceMemory.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/KernelBuffer.hpp"
#include "OpenCL/KernelParam.hpp"
#include "OpenCL/Program.hpp"

#include "Tasks/Task.hpp"
#include "Tasks/AutotuneTask.hpp"
#include "Tasks/CrackerTask.hpp"
#include "Tasks/SetupTask.hpp"
#include "Tasks/Bruteforce/BruteforceSetupTask.hpp"
#include "Tasks/KernelTask.hpp"
#include "Tasks/Bruteforce/BruteforceKernelTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/Bruteforce/BruteforceAttackTask.hpp"
