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
        cl_int cl_error = CL_SUCCESS;

        if (ready)
        {
            return false;
        }

        handle = cl::Program(device->getContext(), sources, &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Program()\n");
        }

        std::stringstream ss;
        ss << "-cl-kernel-arg-info" << " ";
        ss << "-cl-std=CL" << Platform::OPENCL_COMPILER_MAJOR_VERSION << "." << Platform::OPENCL_COMPILER_MINOR_VERSION << " ";

        if (device->getVendor() == Device::Vendor::Intel)
        {
            ss << "-g -s \"D:\\GitHub\\honours-project\\HashCracker\\OpenCL\"";
        }
        else if (device->getVendor() == Device::Vendor::NVIDIA)
        {
            ss << "-cl-nv-verbose";
        }

        for (auto opt : build_opts)
        {
            ss << " " << opt;
        }

        cl_error = handle.build(ss.str().c_str());

        if (cl_error == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string build_log = handle.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device->getHandle());

            Logger::error("Error: Failed to build program!\n %s\n", build_log.c_str());

            return false;
        }

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::CommandQueue()\n");
        }

        cl_build_status status = handle.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device->getHandle());

        if (status != CL_BUILD_SUCCESS)
        {
            return false;
        }

        this->cmd_queue_props = cmd_queue_props;

        command_queue = cl::CommandQueue(device->getContext(), device->getHandle(), cmd_queue_props, &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::CommandQueue()\n");
        }

        source = handle.getInfo<CL_PROGRAM_SOURCE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: Program::getInfo<CL_PROGRAM_SOURCE>()\n");
        }

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

        cl_error = handle.createKernels(&cl_kernels);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: program::createKernels()\n");
        }

        for (cl::Kernel cl_kernel : cl_kernels)
        {
            std::shared_ptr<Kernel> kernel = std::make_shared<Kernel>(shared_from_this());

            std::string name = KernelPlatform::CleanCLString(cl_kernel.getInfo<CL_KERNEL_FUNCTION_NAME>(&cl_error));

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: cl::Kernel::getInfo<>()\n");
            }

            if (!kernel->create(name))
            {
                continue;
            }

            kernels.push_back(kernel);

            for (std::size_t param_idx = 0; param_idx < kernel->getParamNum(); param_idx++)
            {
                std::shared_ptr<KernelParam> param = kernel->getParamAt(param_idx);
                std::shared_ptr<KernelBuffer> buffer = kernel->findBuffer(param);

                if (param->getAddressQualifier() != KernelParam::AddressQualifier::Private)
                {
                    std::shared_ptr<DeviceMemory> mem = device->createMemory(param, buffer->getElemNum() * buffer->getElemSize());

                    if (!mem)
                    {
                        throw std::runtime_error("Error: Not enough memory left on device!");
                    }

                    buffers.insert(std::make_pair(buffer, mem));            
                }

                if (!updateBuffer(buffer, buffer->getElemNum() * buffer->getElemSize()))
                {
                    Logger::error("Failed to update param to kernel!\n");
                }
            }
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

        std::copy(prog->build_options.begin(), prog->build_options.end(), std::inserter(build_options, build_options.end()));

        for (auto opt : build_opts)
        {
            auto iter_opt = build_options.find(opt.first);

            if (iter_opt == build_options.end())
            {
                build_options.insert(opt);
            }
            else
            {
                if (!iter_opt->second.empty())
                {
                    iter_opt->second = opt.second;
                }
            }
        }

        std::vector<std::string> new_build_opts;
        for (auto opt : build_options)
        {
            new_build_opts.push_back(opt.first + (opt.second.empty() ? "" : "=") + opt.second);
        }

        if (!create(prog->source, new_build_opts, prog->cmd_queue_props))
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

    bool Program::updateBuffer(std::shared_ptr<KernelBuffer> buffer, std::size_t size, std::size_t offset)
    {
        cl_int cl_error = CL_SUCCESS;

        if (!buffer || !size)
        {
            return false;
        }

        if (!buffer->writePending())
        {
            return true;
        }

        std::shared_ptr<KernelParam> param = buffer->getParam();
        std::shared_ptr<Kernel> kernel = param->getKernel();

        if (param->getAddressQualifier() == KernelParam::AddressQualifier::Private)
        {
            if (size != buffer ->getElemSize() || offset)
            {
                return false;
            }

            cl_error = kernel->getHandle().setArg(param->getIndex(), size, buffer->getData(offset));

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: kernel::setArg()\n");
            }

            buffer->setWriteFlag(false);

            return true;
        }

        auto iter_buf = buffers.find(buffer);
        std::shared_ptr<DeviceMemory> mem = iter_buf->second;
      
        std::size_t new_size = size + offset;

        if (new_size > mem->getSize())
        {
            std::shared_ptr<DeviceMemory> new_mem = device->createMemory(param, new_size);

            cl_error = command_queue.enqueueWriteBuffer(new_mem->getHandle(), CL_BLOCKING, 0, mem->getSize(), iter_buf->first->getData(0));

            if (cl_error != CL_SUCCESS)
            {
                throw std::runtime_error("ERROR: command_queue::enqueueWriteBuffer()\n");
            }

            device->destroyMemory(mem);

            iter_buf->second = new_mem;

            mem = new_mem;
        }

        cl_error = command_queue.enqueueWriteBuffer(mem->getHandle(), CL_BLOCKING, offset, size, buffer->getData(offset));

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue::enqueueWriteBuffer()\n");
        }

        cl_error = kernel->getHandle().setArg(param->getIndex(), mem->getHandle());

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: kernel::setArg()\n");
        }

        buffer->setWriteFlag(false);

        return true;
    }

    bool Program::syncBuffer(std::shared_ptr<KernelBuffer> buffer, std::size_t size, std::size_t offset)
    {
        cl_int cl_error = CL_SUCCESS;

        if (!buffer->readPending())
        {
            return true;
        }

        std::shared_ptr<DeviceMemory> mem = buffers.at(buffer);

        if (!size)
        {
            size = buffer->getElemNum() * buffer->getElemSize();
        }

        cl_error = command_queue.enqueueReadBuffer(mem->getHandle(), CL_BLOCKING, offset, size, buffer->getData(offset));

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue::enqueueReadBuffer()\n");
        }

        buffer->setReadFlag(false);

        return true;
    }

    std::uint64_t Program::executeKernel(std::shared_ptr<Kernel> kernel, std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size)
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

        cl_error = event.wait();

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: event.wait()\n");
        }
        
        std::uint64_t exec_time = 0;

        if (cmd_queue_props & CL_QUEUE_PROFILING_ENABLE)
        {
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

            exec_time = end_time - start_time;
        }

        cl_error = command_queue.finish();

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: command_queue.finish()\n");
        }

        return exec_time;
    }

    bool CompareKernelBuffer::operator()(const std::shared_ptr<KernelBuffer>& a, const std::shared_ptr<KernelBuffer>& b) const
    {
        return (a->getId() < b->getId());
    }
}
