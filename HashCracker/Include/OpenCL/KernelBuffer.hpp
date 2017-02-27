#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class KernelParam;

    class HASHCRACK_PUBLIC KernelBuffer
    {
    public:

        KernelBuffer(std::shared_ptr<KernelParam> param);

        virtual ~KernelBuffer();

        std::uint32_t getId();

        bool writePending();

        void setWriteFlag(bool status);

        bool readPending();

        void setReadFlag(bool status);

        std::shared_ptr<KernelParam> getParam();

        virtual std::string getType() = 0;

        virtual std::shared_ptr<KernelBuffer> clone(std::shared_ptr<KernelParam> param) = 0;

        virtual bool setData(void* data, std::size_t size, std::size_t offset);

        virtual std::size_t getElemSize() = 0;

        virtual std::size_t getElemNum() = 0;

        virtual void* getData(std::size_t offset) = 0;

        template<class T>
        static void RegisterDataType()
        {
            std::string type_name = CleanTypeName<T>();
            data_types.insert(std::make_pair(type_name, std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferValue<T>>())));

            type_name = CleanTypeName<T*>();
            data_types.insert(std::make_pair(type_name, std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<T>>())));
        }

        template< class T >
        static std::string CleanTypeName()
        {
            std::string result = typeid(T).name();

            std::size_t pos = result.find_last_of("::");

            if (pos != std::string::npos)
            {
                std::size_t pos_space = result.find(" ");

                result = result.erase(pos_space + 1, pos - pos_space);
            }

            std::size_t pos_star = result.find("*");

            if (pos_star != std::string::npos)
            {
                pos_star--;

                if (result[pos_star] == ' ')
                {
                    result = result.erase(pos_star, 1);
                }
            }

            std::size_t pos_s = result.find("__int32");

            if (pos_s != std::string::npos)
            {
                result = result.replace(pos_s, 7, "int");
            }

            std::size_t pos_l = result.find("__int64");

            if (pos_l != std::string::npos)
            {
                result = result.replace(pos_l, 7, "long");
            }

            std::size_t pos_u = result.find("unsigned");

            if (pos_u != std::string::npos)
            {
                result = result.erase(pos_u, 8);

                result[0] = 'u';
            }

            return result;
        }

        static std::shared_ptr<KernelBuffer> Create(std::shared_ptr<KernelParam> param);

        static bool Copy(std::shared_ptr<KernelBuffer> src_buff, std::shared_ptr<KernelBuffer> dst_buff, std::size_t size, std::size_t src_offset, std::size_t dst_offset);

    protected:

        std::uint32_t id;

        bool write_flag;

        bool read_flag;

        std::shared_ptr<KernelParam> param;

        static std::uint32_t next_buffer_id;

        static std::map<std::string, std::shared_ptr<KernelBuffer>> data_types;

        template<class T>
        static std::pair<std::string, std::shared_ptr<KernelBuffer>> MakeDataType(const T& value)
        {
            std::shared_ptr<TKernelBufferValue<T>> buffer = std::make_shared<TKernelBufferValue<T>>(std::shared_ptr<KernelParam>(), value);
            return std::make_pair(buffer->getType(), std::dynamic_pointer_cast<KernelBuffer>(buffer));
        }

        template<class T>
        static std::pair<std::string, std::shared_ptr<KernelBuffer>> MakeDataType(const std::vector<T>& values)
        {
            std::shared_ptr<TKernelBufferArray<T>> buffer = std::make_shared<TKernelBufferArray<T>>(std::shared_ptr<KernelParam>(), values);
            return std::make_pair(buffer->getType(), std::dynamic_pointer_cast<KernelBuffer>(buffer));
        }

    };

    template< class T >
    class TKernelBufferValue : public KernelBuffer
    {
    public:

        TKernelBufferValue(std::shared_ptr<KernelParam> param, const T& value = T())
            : KernelBuffer(param)
        {
            setData((void*)(&value), 1, 0);

            type = CleanTypeName<T>();
        }

        virtual ~TKernelBufferValue()
        {
        }

        void set(const T& value)
        {
            setData((void*)&value, 1, 0);
        }

        T get()
        {
            return value;
        }

        virtual std::string getType() override
        {
            return type;
        }

        virtual std::shared_ptr<KernelBuffer> clone(std::shared_ptr<KernelParam> param) override
        {
            return std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferValue<T>>(param));
        }

        virtual bool setData(void * data, std::size_t size, std::size_t offset) override
        {
            if (!data || size != 1 || offset != 0)
            {
                return false;
            }

            T* cast_ptr = static_cast<T*>(data);

            if (!cast_ptr)
            {
                return false;
            }

            value = (*cast_ptr);

            return KernelBuffer::setData(data, size, offset);
        }

        virtual std::size_t getElemSize() override
        {
            return sizeof(T);
        }

        virtual std::size_t getElemNum() override
        {
            return 1;
        }

        virtual void* getData(std::size_t offset) override
        {
            if (offset)
            {
                return nullptr;
            }

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

        TKernelBufferArray(std::shared_ptr<KernelParam> param, std::vector<T> values = { T() })
            : KernelBuffer(param)
        {
            setData(values.data(), values.size(), 0);

            type = CleanTypeName<T*>();
        }

        virtual ~TKernelBufferArray()
        {
        }

        void set(const std::vector<T>& values, std::size_t offset)
        {
            setData((void*)values.data(), values.size(), offset);
        }

        std::vector<T> get(std::size_t offset)
        {
            auto iter_first = values.begin();
            std::advance(iter_first, offset);

            return std::vector<T>(iter_first, values.end());
        }

        virtual std::string getType() override
        {
            return type;
        }

        virtual std::shared_ptr<KernelBuffer> clone(std::shared_ptr<KernelParam> param) override
        {
            return std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<T>>(param));
        }

        virtual bool setData(void * data, std::size_t size, std::size_t offset) override
        {
            if (!data)
            {
                return false;
            }

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

            return KernelBuffer::setData(data, size, offset);
        }

        virtual std::size_t getElemSize() override
        {
            return sizeof(T);
        }

        virtual std::size_t getElemNum() override
        {
            return values.size();
        }

        virtual void* getData(std::size_t offset) override
        {
            if (offset >= getElemNum())
            {
                return nullptr;
            }

            return values.data() + offset;
        }

    private:

        std::string type;

        std::vector<T> values;

    };
}
