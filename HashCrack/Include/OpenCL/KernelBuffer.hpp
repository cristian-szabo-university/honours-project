#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class HASHCRACK_PUBLIC KernelBuffer
    {
    public:

        KernelBuffer();

        virtual ~KernelBuffer();

        std::uint32_t getId();

        virtual std::string getType() = 0;

        virtual bool copy(std::shared_ptr<KernelBuffer> dst_buff, std::size_t size, std::size_t src_offset, std::size_t dst_offset) = 0;

        virtual std::shared_ptr<KernelBuffer> clone() = 0;

        virtual bool setData(void* data, std::size_t size, std::size_t offset) = 0;

        virtual std::size_t getElemSize() = 0;

        virtual std::size_t getElemNum() = 0;

        virtual void* getData() = 0;

    private:

        static std::uint32_t next_buffer_id;

        std::uint32_t id;

        static std::uint32_t GenerateBufferId();

    };

    template< class T >
    class TKernelBuffer : public KernelBuffer
    {
    public:

        TKernelBuffer(const T& value = T())
        {
            setData((void*)(&value), 1, 0);

            type = KernelPlatform::CleanTypeName<T>();
        }

        virtual ~TKernelBuffer()
        {
        }

        operator T()
        {
            return value;
        }

        virtual std::string getType() override
        {
            return type;
        }

        virtual bool copy(std::shared_ptr<KernelBuffer> dst_buff, std::size_t size, std::size_t src_offset, std::size_t dst_offset) override
        {
            if (getType() != dst_buff->getType())
            {
                return false;
            }

            std::shared_ptr<TKernelBuffer<T>> cast_ptr = std::dynamic_pointer_cast<TKernelBuffer<T>>(dst_buff);

            if (!cast_ptr)
            {
                return false;
            }

            return dst_buff->setData(&value, 1, 0);
        }

        virtual std::shared_ptr<KernelBuffer> clone() override
        {
            return std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<T>>());
        }

        virtual bool setData(void * data, std::size_t size, std::size_t offset) override
        {
            if (size != 1 || offset != 0)
            {
                return false;
            }

            T* cast_ptr = static_cast<T*>(data);

            if (!cast_ptr)
            {
                return false;
            }

            value = (*cast_ptr);

            return true;
        }

        virtual std::size_t getElemSize() override
        {
            return sizeof(T);
        }

        virtual std::size_t getElemNum() override
        {
            return 1;
        }

        virtual void* getData() override
        {
            return &value;
        }

    private:

        std::string type;

        T value;

    };

    template< class T >
    class TKernelBufferArray : public KernelBuffer
    {
    public:

        TKernelBufferArray(std::vector<T> values = { T() })
        {
            setData(values.data(), values.size(), 0);

            type = KernelPlatform::CleanTypeName<T*>();
        }

        virtual ~TKernelBufferArray()
        {
        }

        operator std::vector<T>()
        {
            return values;
        }

        virtual std::string getType() override
        {
            return type;
        }

        virtual bool copy(std::shared_ptr<KernelBuffer> dst_buff, std::size_t size, std::size_t src_offset, std::size_t dst_offset) override
        {
            if (src_offset + size > getElemNum())
            {
                return false;
            }

            if (getType() != dst_buff->getType())
            {
                return false;
            }

            std::shared_ptr<TKernelBufferArray<T>> cast_ptr = std::dynamic_pointer_cast<TKernelBufferArray<T>>(dst_buff);

            if (!cast_ptr)
            {
                return false;
            }

            return dst_buff->setData(values.data() + src_offset, size, dst_offset);
        }

        virtual std::shared_ptr<KernelBuffer> clone() override
        {
            return std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<T>>());
        }

        virtual bool setData(void * data, std::size_t size, std::size_t offset) override
        {
            std::size_t new_size = size + offset;

            if (getElemNum() < new_size)
            {
                values.resize(new_size);
            }

            T* cast_ptr = static_cast<T*>(data);

            if (!cast_ptr)
            {
                return false;
            }

            std::copy(cast_ptr, cast_ptr + size, values.begin() + offset);

            return true;
        }

        virtual std::size_t getElemSize() override
        {
            return sizeof(T);
        }

        virtual std::size_t getElemNum() override
        {
            return values.size();
        }

        virtual void* getData() override
        {
            return values.data();
        }

    private:

        std::string type;

        std::vector<T> values;

    };
}
