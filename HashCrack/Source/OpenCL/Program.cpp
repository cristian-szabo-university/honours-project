#include "Config.hpp"

#include "OpenCL/Program.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/DeviceMemory.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/KernelParam.hpp"
#include "OpenCL/KernelBuffer.hpp"

#include "Core/Platform.hpp"
#include "Core/Logger.hpp"

namespace HonoursProject
{
    Program::Program(cl::Program program, cl::CommandQueue command_queue, std::shared_ptr<Device> device)
        : ready(false), enable_profile(false), handle(program), device(device), command_queue(command_queue)
    {
        cl_int cl_error = CL_SUCCESS;

        cl_command_queue_properties props = command_queue.getInfo<CL_QUEUE_PROPERTIES>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: CommandQueue::getInfo()\n");
        }

        if (props & CL_QUEUE_PROFILING_ENABLE)
        {
            enable_profile = true;
        }
    }

    Program::~Program()
    {
    }

    bool Program::open()
    {
        cl_int cl_error = CL_SUCCESS;

        if (ready)
        {
            return false;
        }

        std::vector<cl::Kernel> cl_kernels;

        cl_error = handle.createKernels(&cl_kernels);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: program::createKernels()\n");
        }

        std::vector<std::shared_ptr<Kernel>> local_kernels;

        for (cl::Kernel kernel : cl_kernels)
        {
            local_kernels.push_back(std::make_shared<Kernel>(kernel, shared_from_this()));
        }

        for (auto kernel : local_kernels)
        {
            if (kernel->create())
            {
                kernels.push_back(kernel);
            }
        }

        ready = !ready;

        return true;
    }

    bool Program::close()
    {
        if (!ready)
        {
            return false;
        }

        kernels.clear();

        buffers.clear();

        ready = !ready;

        return true;
    }

    cl::Program Program::getHandle()
    {
        return handle;
    }

    std::shared_ptr<Device> Program::getDevice()
    {
        return device;
    }

    std::shared_ptr<Kernel> Program::findKernel(const std::string & name)
    {
        auto kernel_iter = std::find_if(kernels.begin(), kernels.end(),
            [&](std::shared_ptr<Kernel> kernel)
        {
            return kernel->getName() == name;
        });

        if (kernel_iter == kernels.end())
        {
            return std::shared_ptr<Kernel>();
        }

        return (*kernel_iter);
    }

    std::shared_ptr<KernelBuffer> Program::createBuffer(std::shared_ptr<KernelParam> param)
    {
        cl_int cl_error = CL_SUCCESS;

        std::shared_ptr<KernelBuffer> result = KernelPlatform::CreateDataType(param->getType());

        if (!result)
        {
            return std::shared_ptr<KernelBuffer>();
        }

        if (param->getAddressQualifier() == KernelParam::AddressQualifier::Private)
        {
            return result;
        }

        std::shared_ptr<DeviceMemory> dev_mem = device->createMemory(result->getElemNum() * result->getElemSize(), param);

        if (!dev_mem)
        {
            return std::shared_ptr<KernelBuffer>();
        }

        auto result_iter = buffers.insert(std::make_pair(result, dev_mem));

        if (!result_iter.second)
        {
            return std::shared_ptr<KernelBuffer>();
        }

        return result;
    }

    bool Program::updateBuffer(std::shared_ptr<Kernel> kernel, std::shared_ptr<KernelParam> param, std::shared_ptr<KernelBuffer> buffer)
    {
        cl_int cl_error = CL_SUCCESS;

        if (param->getAddressQualifier() == KernelParam::AddressQualifier::Private)
        {
            cl_error = kernel->getHandle().setArg(param->getIndex(), buffer->getElemNum() * buffer->getElemSize(), buffer->getData());

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: kernel::setArg()\n");
            }

            return true;
        }

        auto buff_iter = buffers.find(buffer);

        if (buff_iter == buffers.end())
        {
            return false;
        }

        std::int32_t old_size = buff_iter->second->getSize();

        std::int32_t new_size = buffer->getElemNum() * buffer->getElemSize();

        if (old_size < new_size)
        {
            device->destroyMemory(buff_iter->second);

            buff_iter->second = device->createMemory(new_size, param);
        }

        cl_error = command_queue.enqueueWriteBuffer(buff_iter->second->getHandle(), CL_TRUE, 0, new_size, buffer->getData());

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue::enqueueWriteBuffer()\n");
        }

        cl_error = kernel->getHandle().setArg(param->getIndex(), buff_iter->second->getHandle());

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: kernel::setArg()\n");
        }

        return true;
    }

    bool Program::copyBuffer(std::shared_ptr<KernelBuffer> src_buf, std::shared_ptr<Program> dst_prog, std::shared_ptr<KernelBuffer> dst_buf, std::size_t size, std::size_t src_offset, std::size_t dst_offset)
    {
        cl_int cl_error = CL_SUCCESS;

        auto src_buf_iter = buffers.find(src_buf);

        if (src_buf_iter == buffers.end())
        {
            return false;
        }

        auto dst_buf_iter = dst_prog->buffers.find(dst_buf);

        if (dst_buf_iter == dst_prog->buffers.end())
        {
            return false;
        }

        cl_error = command_queue.enqueueCopyBuffer(
            src_buf_iter->second->getHandle(),
            dst_buf_iter->second->getHandle(),
            src_offset * src_buf->getElemSize(),
            dst_offset * src_buf->getElemSize(),
            size * src_buf->getElemSize());

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue::enqueueReadBuffer()\n");
        }

        return true;
    }

    bool Program::syncBuffer(std::shared_ptr<KernelBuffer> buffer)
    {
        cl_int cl_error = CL_SUCCESS;

        auto buff_iter = buffers.find(buffer);

        if (buff_iter == buffers.end())
        {
            return false;
        }

        cl_error = command_queue.enqueueReadBuffer(buff_iter->second->getHandle(), CL_TRUE, 0, buffer->getElemNum() * buffer->getElemSize(), buffer->getData());

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue::enqueueReadBuffer()\n");
        }

        return true;
    }

    double Program::executeKernel(std::shared_ptr<Kernel> kernel, std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size)
    {
        cl_int cl_error = CL_SUCCESS;

        for (std::size_t i = 0; i < 3; i++)
        {
            while (global_size[i] % local_size[i]) global_size[i]++;
        }

        cl::Event event;

        cl_error = command_queue.enqueueNDRangeKernel(
            kernel->getHandle(),
            cl::NDRange(offset_size[0], offset_size[1], offset_size[2]),
            cl::NDRange(global_size[0], global_size[1], global_size[2]),
            cl::NDRange(local_size[0], local_size[1], local_size[2]),
            NULL, &event);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue.enqueueNDRangeKernel()\n");
        }

        cl_error = command_queue.flush();

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue.flush()\n");
        }

        cl_error = command_queue.finish();

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue.finish()\n");
        }
        
        for (std::size_t param_pos = 0; param_pos < kernel->getParamNum(); param_pos++)
        {
            std::shared_ptr<KernelParam> param = kernel->getParamAt(param_pos);

            if (param->autoSyncEnable() &&
                (param->getAddressQualifier() == KernelParam::AddressQualifier::Global ||
                 param->getAddressQualifier() == KernelParam::AddressQualifier::Local) &&
                (param->getAccessQualifier() == KernelParam::AccessQualifier::WriteOnly ||
                 param->getAccessQualifier() == KernelParam::AccessQualifier::ReadWrite))
            {
                kernel->syncParam(param->getName());
            }
        }
        
        if (!enable_profile)
        {
            return 0.0;
        }

        std::int64_t start_time = event.getProfilingInfo<CL_PROFILING_COMMAND_START>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue.finish()\n");
        }

        std::int64_t end_time = event.getProfilingInfo<CL_PROFILING_COMMAND_END>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue.finish()\n");
        }

        double exec_time_ms = ((double)(end_time - start_time) / 1000000.0);

        return exec_time_ms;
    }

    std::shared_ptr<Program> Program::Create(std::shared_ptr<Device> device, std::string source, std::vector<std::string> build_opts, cl_command_queue_properties cmd_queue_props)
    {
        return Create(device, { std::make_pair(source.c_str(), source.size()) }, build_opts, cmd_queue_props);
    }

    std::shared_ptr<Program> Program::Create(std::shared_ptr<Device> device, cl::Program::Sources sources, std::vector< std::string > build_opts, cl_command_queue_properties cmd_queue_props)
    {
        cl_int cl_error = CL_SUCCESS;

        cl::Program program = cl::Program(device->getContext(), sources, &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Program()\n");
        }

        build_opts.push_back("-cl-kernel-arg-info");
        build_opts.push_back("-cl-std=CL1.1");

        std::string options;

        for (auto opt : build_opts)
        {
            options += (opt + " ");
        }

        cl_error = program.build(options.c_str());

        if (cl_error == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device->getHandle());

            Logger::error("Error: Failed to build program!\n %s\n", build_log.c_str());

            return std::shared_ptr<Program>();
        }

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::CommandQueue()\n");
        }

        cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device->getHandle());

        if (status != CL_BUILD_SUCCESS)
        {
            return std::shared_ptr<Program>();
        }

        cl::CommandQueue command_queue = cl::CommandQueue(device->getContext(), device->getHandle(), cmd_queue_props, &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::CommandQueue()\n");
        }

        std::shared_ptr<Program> result = std::make_shared<Program>(program, command_queue, device);

        if (!result->open())
        {
            return std::shared_ptr<Program>();
        }

        return result;
    }

    bool CompareKernelBuffer::operator()(const std::shared_ptr<KernelBuffer>& a, const std::shared_ptr<KernelBuffer>& b) const
    {
        return a->getId() < b->getId();
    }
}
