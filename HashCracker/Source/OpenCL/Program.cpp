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
    Program::Program(std::shared_ptr<Device> device)
        : ready(false), device(device)
    {
    }

    Program::~Program()
    {
    }

    bool Program::create(std::ifstream& file, std::vector<std::string> build_opts, cl_command_queue_properties cmd_queue_props)
    {
        if (!file.is_open())
        {
            return false;
        }

        return create(std::string(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>())), build_opts, cmd_queue_props);
    }

    bool Program::create(std::string source, std::vector<std::string> build_opts, cl_command_queue_properties cmd_queue_props)
    {
        return create({ std::make_pair(source.c_str(), source.size()) }, build_opts, cmd_queue_props);;
    }

    bool Program::create(cl::Program::Sources sources, std::vector< std::string > build_opts, cl_command_queue_properties cmd_queue_props)
    {
        if (ready)
        {
            return false;
        }

        handle = cl::Program(device->getContext(), sources);

        std::stringstream ss;
        ss << "-cl-kernel-arg-info" << " ";
        ss << "-cl-std=CL" << Platform::OPENCL_COMPILER_MAJOR_VERSION << "." << Platform::OPENCL_COMPILER_MINOR_VERSION << " ";

        for (auto opt : build_opts)
        {
            ss << " " << opt;
        }

        try
        {
            handle.build(ss.str().c_str());
        }
        catch (cl::Error& ex)
        {
            if (ex.err() != CL_BUILD_PROGRAM_FAILURE)
            {
                throw ex;
            }

            std::string build_log = handle.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device->getHandle());

            throw std::runtime_error(build_log);
        }

        cl_build_status status = handle.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device->getHandle());

        if (status != CL_BUILD_SUCCESS)
        {
            return false;
        }

        this->cmd_queue_props = cmd_queue_props;

        command_queue = cl::CommandQueue(device->getContext(), device->getHandle(), cmd_queue_props);

        source = handle.getInfo<CL_PROGRAM_SOURCE>();

        for (auto opt : build_opts)
        {
            std::size_t pos = opt.find("=");

            std::string name, value;
            if (pos != std::string::npos)
            {
                name = opt.substr(0, pos);
                value = opt.substr(pos + 1);
            }
            else
            {
                name = opt;
            }

            build_options.insert(std::make_pair(name, value));
        }

        std::vector<cl::Kernel> cl_kernels;
        handle.createKernels(&cl_kernels);

        for (cl::Kernel cl_kernel : cl_kernels)
        {
            std::shared_ptr<Kernel> kernel = std::make_shared<Kernel>(shared_from_this());

            std::string name = KernelPlatform::CleanCLString(cl_kernel.getInfo<CL_KERNEL_FUNCTION_NAME>());

            if (!kernel->create(name))
            {
                continue;
            }

            kernels.push_back(kernel);
        }

        ready = !ready;

        return true;
    }

    bool Program::create(std::shared_ptr<Program> prog, std::vector<std::pair<std::string, std::string>> build_opts)
    {
        if (ready)
        {
            return false;
        }

        std::map<std::string, std::string> new_build_opts;
        std::copy(prog->build_options.begin(), prog->build_options.end(), std::inserter(new_build_opts, new_build_opts.end()));

        for (auto opt : build_opts)
        {
            auto iter_opt = new_build_opts.find(opt.first);

            if (iter_opt == new_build_opts.end())
            {
                new_build_opts.insert(opt);
            }
            else
            {
                if (!iter_opt->second.empty())
                {
                    iter_opt->second = opt.second;
                }
            }
        }

        std::vector<std::string> new_build_options;
        for (auto opt : new_build_opts)
        {
            new_build_options.push_back(opt.first + (opt.second.empty() ? "" : "=") + opt.second);
        }

        if (!create(prog->source, new_build_options, prog->cmd_queue_props))
        {
            return false;
        }

        for (auto old_kernel : prog->kernels)
        {
            std::shared_ptr<Kernel> new_kernel = findKernel(old_kernel->getName());

            for (std::size_t index = 0; index < new_kernel->getParamNum(); index++)
            {
                std::shared_ptr<KernelParam> src_param = old_kernel->getParamAt(index);
                std::shared_ptr<KernelParam> dst_param = new_kernel->getParamAt(index);
                std::shared_ptr<KernelBuffer> buffer = old_kernel->findBuffer(src_param);

                dst_param->setAccessQualifier(src_param->getAccessQualifier());
                dst_param->setAddressQualifier(src_param->getAddressQualifier());

                if (!new_kernel->createBuffer(dst_param, buffer->getElemNum()))
                {
                    continue;
                }

                if (src_param->autoSyncEnable())
                {
                    dst_param->toggleAutoSync();
                }

                if (!KernelParam::Copy(src_param, dst_param, buffer->getElemNum()))
                {
                    Logger::info("Failed to copy kernel param!\n");
                }
            }
        }

        return true;
    }

    bool Program::destroy()
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

    cl::CommandQueue Program::getCommandQueue()
    {
        return command_queue;
    }

    std::shared_ptr<Device> Program::getDevice()
    {
        return device;
    }

    std::string Program::getSource()
    {
        return source;
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

    std::shared_ptr<DeviceMemory> Program::createMemory(std::shared_ptr<KernelBuffer> buffer)
    {
        auto iter_buf = buffers.find(buffer);

        if (iter_buf != buffers.end())
        {
            return iter_buf->second;
        }

        std::size_t new_size = buffer->getElemNum() * buffer->getElemSize();

        std::shared_ptr<DeviceMemory> new_mem = device->createMemory(buffer->getParam(), new_size);

        if (!new_mem->isReady())
        {
            throw std::runtime_error("Error: Not enough memory left on device!");
        }

        std::shared_ptr<KernelParam> param = buffer->getParam();
        std::shared_ptr<Kernel> kernel = param->getKernel();

        kernel->getHandle().setArg(param->getIndex(), new_mem->getHandle());

        buffers.insert(std::make_pair(buffer, new_mem));

        return new_mem;
    }

    bool Program::destroyMemory(std::shared_ptr<KernelBuffer> buffer)
    {
        if (!buffer)
        {
            return true;
        }

        auto iter_buf = buffers.find(buffer);

        if (iter_buf == buffers.end())
        {
            return true;
        }

        device->destroyMemory(iter_buf->second);

        buffers.erase(iter_buf);

        return true;
    }

    std::shared_ptr<DeviceMemory> Program::findMemory(std::shared_ptr<KernelBuffer> buffer)
    {
        auto iter_buf = buffers.find(buffer);

        if (iter_buf == buffers.end())
        {
            return std::shared_ptr<DeviceMemory>();
        }

        return iter_buf->second;
    }

    bool Program::syncBuffer(std::shared_ptr<KernelBuffer> buffer, std::size_t size, std::size_t offset)
    {
        if (!buffer->readPending())
        {
            return true;
        }

        std::shared_ptr<DeviceMemory> mem = buffers.at(buffer);

        if (!size)
        {
            size = buffer->getElemNum() * buffer->getElemSize();
        }

        command_queue.enqueueReadBuffer(mem->getHandle(), CL_BLOCKING, offset, size, buffer->getData(offset));

        buffer->setReadFlag(false);

        return true;
    }

    std::uint64_t Program::executeKernel(std::shared_ptr<Kernel> kernel, std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size)
    {
        for (std::size_t i = 0; i < 3; i++)
        {
            while (global_size[i] % local_size[i]) global_size[i]++;
        }

        cl::Event event;

        command_queue.enqueueNDRangeKernel(
            kernel->getHandle(),
            cl::NDRange(offset_size[0], offset_size[1], offset_size[2]),
            cl::NDRange(global_size[0], global_size[1], global_size[2]),
            cl::NDRange(local_size[0], local_size[1], local_size[2]),
            NULL, &event);

        command_queue.flush();

        event.wait();
        
        std::uint64_t exec_time = 0;

        if (cmd_queue_props & CL_QUEUE_PROFILING_ENABLE)
        {
            std::int64_t start_time = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();

            std::int64_t end_time = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

            exec_time = end_time - start_time;
        }

        command_queue.finish();

        return exec_time;
    }

    bool Program::updateBuffer(std::shared_ptr<KernelBuffer> buffer, std::size_t size, std::size_t offset)
    {
        std::shared_ptr<DeviceMemory> mem = findMemory(buffer);

        if (!mem->isReady())
        {
            return false;
        }

        if (offset + size > mem->getSize())
        {
            return false;
        }

        command_queue.enqueueWriteBuffer(mem->getHandle(), CL_BLOCKING, offset, size, buffer->getData(offset));

        return true;
    }

    bool CompareKernelBuffer::operator()(const std::shared_ptr<KernelBuffer>& a, const std::shared_ptr<KernelBuffer>& b) const
    {
        return (a->getId() < b->getId());
    }
}
