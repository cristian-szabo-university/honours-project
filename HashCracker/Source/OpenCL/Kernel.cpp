#include "Config.hpp"

#include "OpenCL/Kernel.hpp"
#include "OpenCL/DeviceMemory.hpp"

#include "Core/Platform.hpp"
#include "Core/Logger.hpp"

namespace HonoursProject
{
    Kernel::Kernel(std::shared_ptr<Program> program)
        : ready(false), program(program)
    {
    }

    Kernel::~Kernel()
    {
    }

    bool Kernel::create(const std::string& name)
    {
        cl_int cl_error = CL_SUCCESS;

        if (ready)
        {
            return false;
        }

        handle = cl::Kernel(program->getHandle(), name.c_str(), &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Kernel()\n");
        }

        this->name = name;

        cl_uint param_count = handle.getInfo<CL_KERNEL_NUM_ARGS>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: kernel::getInfo()\n");
        }

        for (cl_uint param_pos = 0; param_pos < param_count; param_pos++)
        {
            cl_kernel_arg_address_qualifier param_address_qualifier = handle.getArgInfo<CL_KERNEL_ARG_ADDRESS_QUALIFIER>(param_pos, &cl_error);

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: kernel::getArgInfo()\n");
            }

            cl_kernel_arg_access_qualifier param_access_qualifier = handle.getArgInfo<CL_KERNEL_ARG_ACCESS_QUALIFIER>(param_pos, &cl_error);

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: kernel::getArgInfo()\n");
            }

            std::string param_type = KernelPlatform::CleanCLString(handle.getArgInfo<CL_KERNEL_ARG_TYPE_NAME>(param_pos, &cl_error));

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: kernel::getArgInfo()\n");
            }

            std::string param_name = KernelPlatform::CleanCLString(handle.getArgInfo<CL_KERNEL_ARG_NAME>(param_pos, &cl_error));

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: kernel::getArgInfo()\n");
            }

            std::shared_ptr<KernelParam> param = std::make_shared<KernelParam>(shared_from_this()); 
            
            if (!param->create(param_name, param_type, param_pos, param_address_qualifier, param_access_qualifier))
            {
                continue;
            }

            std::shared_ptr<KernelBuffer> buffer = KernelBuffer::Create(param);

            if (!buffer)
            {
                continue;
            }

            params.insert(std::make_pair(param, buffer));
        }

        ready = !ready;

        return true;
    }

    bool Kernel::destroy()
    {
        if (!ready)
        {
            return false;
        }

        params.clear();

        ready = !ready;

        return true;
    }

    cl::Kernel Kernel::getHandle()
    {
        return handle;
    }

    std::shared_ptr<Program> Kernel::getProgram()
    {
        return program;
    }

    const std::string & Kernel::getName()
    {
        return name;
    }

    std::size_t Kernel::getParamNum()
    {
        return params.size();
    }

    std::shared_ptr<KernelParam> Kernel::getParamAt(std::size_t index)
    {
        auto iter_param = params.begin();

        std::advance(iter_param, index);

        if (iter_param == params.end())
        {
            return std::shared_ptr<KernelParam>();
        }

        return iter_param->first;
    }

    bool Kernel::hasParam(const std::string & name)
    {
        std::shared_ptr<KernelParam> param = findParam(name);

        if (!param)
        {
            return false;
        }

        return true;
    }

    std::uint64_t Kernel::execute(std::size_t global_size, std::size_t local_size, std::size_t offset_size)
    {
        return execute({ global_size, 1, 1 }, { local_size, 1, 1 }, { offset_size, 0, 0 });
    }

    std::uint64_t Kernel::execute(std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size)
    {
        for (auto iter : params)
        {
            std::shared_ptr<KernelParam> param = iter.first;
            std::shared_ptr<KernelBuffer> buffer = iter.second;

            if (!buffer->writePending())
            {
                continue;
            }

            if (!program->updateBuffer(buffer, buffer->getElemNum() * buffer->getElemSize()))
            {
                Logger::error("Failed to update param to kernel!\n");

                return 0;
            }
        }

        std::uint64_t exec_time = program->executeKernel(shared_from_this(), global_size, local_size, offset_size);

        for (auto iter : params)
        {
            std::shared_ptr<KernelParam> param = iter.first;
            std::shared_ptr<KernelBuffer> buffer = iter.second;

            buffer->setReadFlag(true);

            if (!param->autoSyncEnable())
            {
                continue;
            }

            if (!program->syncBuffer(buffer, buffer->getElemNum() * buffer->getElemSize()))
            {
                Logger::error("Failed to update kernel param!\n");

                continue;
            }
        }

        return exec_time;
    }

    std::shared_ptr<KernelParam> Kernel::findParam(const std::string& name)
    {
        auto param_iter = std::find_if(params.begin(), params.end(),
            [&](std::pair<std::shared_ptr<KernelParam>, std::shared_ptr<KernelBuffer>> pair)
        {
            return pair.first->getName() == name;
        });

        if (param_iter == params.end())
        {
            return std::shared_ptr<KernelParam>();
        }

        return param_iter->first;
    }

    std::shared_ptr<KernelBuffer> Kernel::findBuffer(std::shared_ptr<KernelParam> param)
    {
        auto iter_param = params.find(param);

        if (iter_param == params.end())
        {
            return std::shared_ptr<KernelBuffer>();
        }

        return iter_param->second;
    }

    bool Kernel::updateParam(std::shared_ptr<KernelParam> param, void * data, std::size_t size, std::size_t offset)
    {
        cl_int cl_error = CL_SUCCESS;

        if (!param)
        {
            return false;
        }

        std::shared_ptr<KernelBuffer> buffer = params.at(param);

        if (!buffer->setData(data, size, offset))
        {
            return false;
        }

        if (!program->updateBuffer(buffer, size * buffer->getElemSize(), offset * buffer->getElemSize()))
        {
            return false;
        }

        return true;
    }

    bool Kernel::syncParam(std::shared_ptr<KernelParam> param, std::size_t size, std::size_t offset)
    {
        if (!param)
        {
            return false;
        }

        std::shared_ptr<KernelBuffer> buffer = params.at(param);

        if (!size)
        {
            size = buffer->getElemNum();
        }

        if (!program->updateBuffer(buffer, size * buffer->getElemSize(), offset * buffer->getElemSize()))
        {
            return false;
        }

        return true;
    }

    bool CompareKernelParam::operator()(const std::shared_ptr<KernelParam>& a, const std::shared_ptr<KernelParam>& b) const
    {
        return (a->getName().compare(b->getName()) < 0);
    }
}