#include "Config.hpp"

#include "OpenCL/Kernel.hpp"

#include "Core/Platform.hpp"

namespace HonoursProject
{
    Kernel::Kernel(cl::Kernel kernel, std::shared_ptr<Program> program)
        : ready(false), handle(kernel), program(program)
    {
        cl_int cl_error = CL_SUCCESS;

        name = KernelPlatform::CleanCLString(handle.getInfo<CL_KERNEL_FUNCTION_NAME>(&cl_error));

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: kernel::getInfo()\n");
        }
    }

    Kernel::~Kernel()
    {
    }

    bool Kernel::create()
    {
        cl_int cl_error = CL_SUCCESS;

        if (ready)
        {
            return false;
        }

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

            std::shared_ptr<KernelParam> kernel_param = KernelParam::Create(param_name, param_type, param_pos, param_address_qualifier, param_access_qualifier);

            if (!kernel_param)
            {
                continue;
            }

            params.push_back(kernel_param);

            std::shared_ptr<KernelBuffer> kernel_buffer = program->createBuffer(kernel_param);

            if (!kernel_buffer)
            {
                continue;
            }

            if (!program->updateBuffer(shared_from_this(), kernel_param, kernel_buffer))
            {
                continue;
            }

            buffers.insert(std::make_pair(kernel_param->getName(), kernel_buffer));
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

        buffers.clear();

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
        return params.at(index);
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

    bool Kernel::copyParam(const std::string & name, std::shared_ptr<Kernel> kernel, std::size_t size, std::size_t src_offset, std::size_t dst_offset)
    {
        cl_int cl_error = CL_SUCCESS;

        std::shared_ptr<KernelParam> param = kernel->findParam(name);

        if (!param)
        {
            return false;
        }

        auto src_buff_iter = buffers.find(name);

        if (src_buff_iter == buffers.end())
        {
            return false;
        }

        std::shared_ptr<KernelBuffer> src_buff = src_buff_iter->second;

        auto dst_buff_iter = kernel->buffers.find(name);

        if (dst_buff_iter == kernel->buffers.end())
        {
            return false;
        }

        std::shared_ptr<KernelBuffer> dst_buff = dst_buff_iter->second;

        if (src_buff->getType() != dst_buff->getType())
        {
            return false;
        }

        if (!src_buff->copy(dst_buff, size, src_offset, dst_offset))
        {
            return false;
        }

        if (!kernel->getProgram()->updateBuffer(kernel, param, dst_buff))
        {
            return false;
        }

        if (!program->copyBuffer(src_buff, kernel->getProgram(), dst_buff, size, src_offset, dst_offset))
        {
            return false;
        }

        return true;
    }

    bool Kernel::syncParam(const std::string & name)
    {
        auto buff_iter = buffers.find(name);

        if (buff_iter == buffers.end())
        {
            return false;
        }

        if (!program->syncBuffer(buff_iter->second))
        {
            return false;
        }

        return true;
    }

    bool Kernel::enableAutoSync(const std::string & name, bool flag)
    {
        std::shared_ptr<KernelParam> param = findParam(name);

        if (!param)
        {
            return false;
        }

        if (flag != param->autoSyncEnable())
        {
            param->toggleAutoSync();
        }

        return true;
    }

    double Kernel::execute(std::size_t global_size, std::size_t local_size, std::size_t offset_size)
    {
        return execute({ global_size, 1, 1 }, { local_size, 1, 1 }, { offset_size, 0, 0 });
    }

    double Kernel::execute(std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size)
    {
        return program->executeKernel(shared_from_this(), global_size, local_size, offset_size);
    }

    std::shared_ptr<Kernel> Kernel::Create(const std::string & name, std::shared_ptr<Program> program)
    {
        cl_int cl_error = CL_SUCCESS;

        cl::Kernel cl_kernel = cl::Kernel(program->getHandle(), name.c_str(), &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Kernel()\n");
        }

        std::shared_ptr<Kernel> result = std::make_shared<Kernel>(cl_kernel, program);

        if (!result->create())
        {
            return std::shared_ptr<Kernel>();
        }

        return result;
    }

    std::shared_ptr<KernelParam> Kernel::findParam(const std::string& param_name)
    {
        auto param_iter = std::find_if(params.begin(), params.end(),
            [&](std::shared_ptr<KernelParam> param)
        {
            return param->getName() == param_name;
        });

        if (param_iter == params.end())
        {
            return std::shared_ptr<KernelParam>();
        }

        return (*param_iter);
    }
}
