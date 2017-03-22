/**
    Program.hpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#pragma once

#include "OpenCL/KernelParam.hpp"

namespace HonoursProject
{
    class Device;
    class DeviceMemory;
    class Kernel;
    class KernelBuffer;

    struct HASH_CRACKER_PUBLIC CompareKernelBuffer
    {
        bool operator()(const std::shared_ptr<KernelBuffer>& a, const std::shared_ptr<KernelBuffer>& b) const;
    };

    class HASH_CRACKER_PUBLIC Program : public std::enable_shared_from_this<Program>
    {
    public:

        Program(std::shared_ptr<Device> device);

        ~Program();

        bool create(std::ifstream& file, std::vector< std::string > build_opts = {}, cl_command_queue_properties cmd_queue_props = 0);

        bool create(std::string source, std::vector< std::string > build_opts = {}, cl_command_queue_properties cmd_queue_props = 0);

        bool create(cl::Program::Sources sources, std::vector< std::string > build_opts = {}, cl_command_queue_properties cmd_queue_props = 0);

        bool create(std::shared_ptr<Program> prog, std::vector<std::pair<std::string, std::string>> build_opts = {});

        bool destroy();

        cl::Program getHandle();

        cl::CommandQueue getCommandQueue();

        std::shared_ptr<Device> getDevice();

        std::string getSource();

        std::shared_ptr<Kernel> findKernel(const std::string& name);

        std::shared_ptr<DeviceMemory> createMemory(std::shared_ptr<KernelBuffer> buffer);

        std::shared_ptr<DeviceMemory> findMemory(std::shared_ptr<KernelBuffer> buffer);

        bool destroyMemory(std::shared_ptr<KernelBuffer> buffer);

        bool updateBuffer(std::shared_ptr<KernelBuffer> buffer, std::size_t size, std::size_t offset = 0);

        bool syncBuffer(std::shared_ptr<KernelBuffer> buffer, std::size_t size = 0, std::size_t offset = 0);

        std::uint64_t executeKernel(std::shared_ptr<Kernel> kernel, std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size = { 0, 0, 0 });

    private:

        bool ready;

        cl_command_queue_properties cmd_queue_props;

        cl::Program handle;

        cl::CommandQueue command_queue;

        std::string source;

        std::map<std::string, std::string> build_options;

        std::shared_ptr<Device> device;

        std::vector<std::shared_ptr<Kernel>> kernels;

        std::map<std::shared_ptr<KernelBuffer>, std::shared_ptr<DeviceMemory>, CompareKernelBuffer> buffers;

    };
}
