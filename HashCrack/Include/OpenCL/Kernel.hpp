#pragma once

#include "Core/Platform.hpp"

#include "OpenCL/Program.hpp"
#include "OpenCL/KernelParam.hpp"
#include "OpenCL/KernelBuffer.hpp"

namespace HonoursProject
{
    class HASHCRACK_PUBLIC Kernel : public std::enable_shared_from_this<Kernel>
    {
    public:

        Kernel(cl::Kernel kernel, std::shared_ptr<Program> program);

        ~Kernel();

        bool create();

        bool destroy();

        cl::Kernel getHandle();

        std::shared_ptr<Program> getProgram();

        const std::string& getName();

        std::size_t getParamNum();

        std::shared_ptr<KernelParam> getParamAt(std::size_t index);

        std::shared_ptr<KernelParam> findParam(const std::string& param_name);

        template<class T>
        bool setParam(const std::string& name, const T& value)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            if (KernelPlatform::CleanTypeName<T>() != param->getType())
            {
                return false;
            }

            auto buff_iter = buffers.find(name);

            if (buff_iter == buffers.end())
            {
                return false;
            }

            std::shared_ptr<KernelBuffer> buffer = buff_iter->second;

            if (!buffer->setData((void*)(&value), 1, 0))
            {
                return false;
            }

            if (!program->updateBuffer(shared_from_this(), param, buffer))
            {
                return false;
            }

            return true;
        }

        template<class T>
        bool getParam(const std::string& name, T& value)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            if (KernelPlatform::CleanTypeName<T>() != param->getType())
            {
                return false;
            }

            auto buff_iter = buffers.find(name);

            if (buff_iter == buffers.end())
            {
                return false;
            }

            std::shared_ptr< TKernelBuffer<T> > cast_ptr = std::dynamic_pointer_cast<TKernelBuffer<T>>(buff_iter->second);

            if (!cast_ptr)
            {
                return false;
            }

            value = (*(cast_ptr.get()));

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

            if (KernelPlatform::CleanTypeName<T*>() != param->getType())
            {
                return false;
            }

            auto buff_iter = buffers.find(name);

            if (buff_iter == buffers.end())
            {
                return false;
            }

            std::shared_ptr<KernelBuffer> buffer = buff_iter->second;

            if (!buffer->setData((void*)(values.data()), values.size(), offset))
            {
                return false;
            }

            if (!program->updateBuffer(shared_from_this(), param, buffer))
            {
                return false;
            }

            return true;
        }

        template<class T>
        bool getParam(const std::string& name, std::vector<T>& value)
        {
            std::shared_ptr<KernelParam> param = findParam(name);

            if (!param)
            {
                return false;
            }

            if (KernelPlatform::CleanTypeName<T*>() != param->getType())
            {
                return false;
            }

            auto buff_iter = buffers.find(name);

            if (buff_iter == buffers.end())
            {
                return false;
            }

            std::shared_ptr< TKernelBufferArray<T> > cast_ptr = std::dynamic_pointer_cast<TKernelBufferArray<T>>(buff_iter->second);

            if (!cast_ptr)
            {
                return false;
            }

            value = (*(cast_ptr.get()));

            return true;
        }

        bool hasParam(const std::string& name);

        bool copyParam(const std::string& name, std::shared_ptr<Kernel> kernel, std::size_t size, std::size_t src_offset = 0, std::size_t dst_offset = 0);

        bool syncParam(const std::string& name);

        bool enableAutoSync(const std::string& name, bool flag = true);

        std::uint64_t execute(std::size_t global_size, std::size_t local_size, std::size_t offset_size = 0);

        std::uint64_t execute(std::array<std::size_t, 3> global_size, std::array<std::size_t, 3> local_size, std::array<std::size_t, 3> offset_size = { 0, 0, 0 });

        static std::shared_ptr<Kernel> Create(const std::string& name, std::shared_ptr<Program> program);

    private:

        bool ready;

        std::string name;

        cl::Kernel handle;

        std::shared_ptr<Program> program;

        std::vector< std::shared_ptr<KernelParam> > params;

        std::map< std::string, std::shared_ptr<KernelBuffer> > buffers;

    };
}
