#pragma once

#include "Core/Platform.hpp"

#include "OpenCL/Program.hpp"
#include "OpenCL/KernelParam.hpp"
#include "OpenCL/KernelBuffer.hpp"
#include "OpenCL/DeviceMemory.hpp"

namespace HonoursProject
{
    struct HASH_CRACKER_PUBLIC CompareKernelParam 
    {
        bool operator()(const std::shared_ptr<KernelParam>& a, const std::shared_ptr<KernelParam>& b) const;
    };

    class HASH_CRACKER_PUBLIC Kernel : public std::enable_shared_from_this<Kernel>
    {
    public:

        Kernel(std::shared_ptr<Program> program);

        ~Kernel();

        bool create(const std::string& name);

        bool destroy();

        cl::Kernel getHandle();

        std::shared_ptr<Program> getProgram();

        const std::string& getName();

        std::size_t getParamNum();

        std::shared_ptr<KernelParam> getParamAt(std::size_t index);

        bool hasParam(const std::string& name);

        std::shared_ptr<KernelParam> findParam(const std::string& name);

        std::shared_ptr<KernelBuffer> createBuffer(std::shared_ptr<KernelParam> param, std::size_t size = 1);

        std::shared_ptr<KernelBuffer> findBuffer(std::shared_ptr<KernelParam> param);

        bool updateParam(std::shared_ptr<KernelParam> param, std::size_t size, std::size_t offset = 0);

        bool syncParam(std::shared_ptr<KernelParam> param, std::size_t size = 0, std::size_t offset = 0);

        template<class T>
        bool setParam(const std::string& name, const T& value)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            std::shared_ptr<TKernelBufferValue<T>> cast_ptr = std::dynamic_pointer_cast<TKernelBufferValue<T>>(params.at(param));

            if (!cast_ptr || !cast_ptr->set(value))
            {
                return false;
            }

            return updateParam(param, 1);
        }

        template<class T>
        bool getParam(const std::string& name, T& value)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            std::shared_ptr<TKernelBufferValue<T>> cast_ptr = std::dynamic_pointer_cast<TKernelBufferValue<T>>(params.at(param));

            if (!cast_ptr)
            {
                return false;
            }

            value = cast_ptr->get();

            return true;
        }

        template<class T>
        bool setParam(const std::string& name, const std::vector<T>& values, std::size_t offset = 0)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            std::shared_ptr<KernelBuffer> buffer = findBuffer(param);
            std::shared_ptr<DeviceMemory> src_mem = program->findMemory(buffer);

            std::shared_ptr<TKernelBufferArray<T>> cast_ptr = std::dynamic_pointer_cast<TKernelBufferArray<T>>(buffer);

            if (!cast_ptr)
            {
                return false;
            }

            std::size_t new_size = values.size() + offset;

            if (new_size > cast_ptr->getElemNum())
            {
                std::shared_ptr<KernelBuffer> new_buffer = createBuffer(param, new_size);
                std::shared_ptr<DeviceMemory> dst_mem = program->findMemory(new_buffer);

                if (offset && !KernelBuffer::Copy(buffer, new_buffer, offset))
                {
                    return false;
                }

                if (offset && !DeviceMemory::Copy(src_mem, dst_mem, offset * buffer->getElemSize()))
                {
                    return false;
                }

                program->destroyMemory(buffer);

                cast_ptr = std::dynamic_pointer_cast<TKernelBufferArray<T>>(new_buffer);
            }

            if (!cast_ptr->set(values, offset))
            {
                return false;
            }

            return updateParam(param, values.size(), offset);
        }

        template<class T>
        bool getParam(const std::string& name, std::vector<T>& value, std::size_t offset = 0)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            std::shared_ptr<TKernelBufferArray<T>> cast_ptr = std::dynamic_pointer_cast<TKernelBufferArray<T>>(findBuffer(param));

            if (!cast_ptr)
            {
                return false;
            }

            if (cast_ptr->readPending())
            {
                if (!syncParam(param, cast_ptr->getElemNum(), offset))
                {
                    return false;
                }
            }

            value = cast_ptr->get(offset);

            return true;
        }

        std::uint64_t execute(std::size_t global_size, std::size_t local_size, std::size_t offset_size = 0);

        std::uint64_t execute(std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size = { 0, 0, 0 });

    private:

        bool ready;

        std::string name;

        cl::Kernel handle;

        std::shared_ptr<Program> program;

        std::map<std::shared_ptr<KernelParam>, std::shared_ptr<KernelBuffer>, CompareKernelParam> params;

    };
}
