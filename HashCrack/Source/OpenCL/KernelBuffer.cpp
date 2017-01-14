#include "Config.hpp"

#include "OpenCL/KernelBuffer.hpp"

namespace HonoursProject
{
    std::uint32_t KernelBuffer::next_buffer_id = 0;

    KernelBuffer::KernelBuffer()
    {
        id = GenerateBufferId();
    }

    KernelBuffer::~KernelBuffer()
    {
    }

    std::uint32_t KernelBuffer::getId()
    {
        return id;
    }

    std::uint32_t KernelBuffer::GenerateBufferId()
    {
        return next_buffer_id++;
    }
}
