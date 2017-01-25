#pragma once

#include "OpenCL/KernelParam.hpp"

namespace HonoursProject
{
    class Device;
    class DeviceMemory;
    class Kernel;
    class KernelBuffer;

    struct HASHCRACK_PUBLIC CompareKernelBuffer
    {
        bool operator()(const std::shared_ptr<KernelBuffer>& a, const std::shared_ptr<KernelBuffer>& b) const;
    };

    class HASHCRACK_PUBLIC Program : public std::enable_shared_from_this<Program>
    {
    public:

        Program(cl::Program program, cl::CommandQueue command_queue, std::shared_ptr<Device> device);

        ~Program();

        bool open();

        bool close();

        cl::Program getHandle();

        std::shared_ptr<Device> getDevice();

        bool recompile(std::vector< std::string > build_opts = {});

        std::shared_ptr<Kernel> findKernel(const std::string& name);

        std::shared_ptr<KernelBuffer> createBuffer(std::shared_ptr<KernelParam> param);

        bool updateBuffer(std::shared_ptr<Kernel> kernel, std::shared_ptr<KernelParam> param, std::shared_ptr<KernelBuffer> buffer);

        bool copyBuffer(std::shared_ptr<KernelBuffer> src_buf, std::shared_ptr<Program> dst_prog, std::shared_ptr<KernelBuffer> dst_buf, std::size_t size, std::size_t src_offset = 0, std::size_t dst_offset = 0);

        bool syncBuffer(std::shared_ptr<KernelBuffer> buffer);

        std::uint64_t executeKernel(std::shared_ptr<Kernel> kernel, std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size = { 0, 0, 0 });

        static std::shared_ptr<Program> Create(std::shared_ptr<Device> device, std::string source, std::vector< std::string > build_opts = {}, cl_command_queue_properties cmd_queue_props = 0);

        static std::shared_ptr<Program> Create(std::shared_ptr<Device> device, cl::Program::Sources sources, std::vector< std::string > build_opts = {}, cl_command_queue_properties cmd_queue_props = 0);

    private:

        bool ready;

        bool enable_profile;

        cl::Program handle;

        cl::CommandQueue command_queue;

        std::shared_ptr<Device> device;

        std::vector< std::shared_ptr<Kernel> > kernels;

        std::map< std::shared_ptr<KernelBuffer>, std::shared_ptr<DeviceMemory>, CompareKernelBuffer> buffers;

    };
}
