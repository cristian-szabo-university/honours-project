#include "Config.hpp"

#include "OpenCL/KernelBuffer.hpp"
#include "OpenCL/KernelParam.hpp"
#include "OpenCL/Kernel.hpp"

#include "Core/Logger.hpp"

namespace HonoursProject
{
    std::uint32_t KernelBuffer::next_buffer_id = 0;

    std::map<std::string, std::shared_ptr<KernelBuffer>> KernelBuffer::data_types =
    {
        KernelBuffer::MakeDataType(cl_bool()),
        KernelBuffer::MakeDataType(std::vector<cl_bool>()),
        KernelBuffer::MakeDataType(cl_char()),
        KernelBuffer::MakeDataType(std::vector<cl_char>()),
        KernelBuffer::MakeDataType(cl_uchar()),
        KernelBuffer::MakeDataType(std::vector<cl_uchar>()),
        KernelBuffer::MakeDataType(cl_short()),
        KernelBuffer::MakeDataType(std::vector<cl_short>()),
        KernelBuffer::MakeDataType(cl_ushort()),
        KernelBuffer::MakeDataType(std::vector<cl_ushort>()),
        KernelBuffer::MakeDataType(cl_int()),
        KernelBuffer::MakeDataType(std::vector<cl_int>()),
        KernelBuffer::MakeDataType(cl_uint()),
        KernelBuffer::MakeDataType(std::vector<cl_uint>()),
        KernelBuffer::MakeDataType(cl_long()),
        KernelBuffer::MakeDataType(std::vector<cl_long>()),
        KernelBuffer::MakeDataType(cl_ulong()),
        KernelBuffer::MakeDataType(std::vector<cl_ulong>()),
        KernelBuffer::MakeDataType(cl_float()),
        KernelBuffer::MakeDataType(std::vector<cl_float>()),
        KernelBuffer::MakeDataType(cl_double()),
        KernelBuffer::MakeDataType(std::vector<cl_double>()),

        KernelBuffer::MakeDataType(KernelPlatform::charset_t()),
        KernelBuffer::MakeDataType(std::vector<KernelPlatform::charset_t>()),
        KernelBuffer::MakeDataType(KernelPlatform::message_t()),
        KernelBuffer::MakeDataType(std::vector<KernelPlatform::message_t>()),
        KernelBuffer::MakeDataType(KernelPlatform::message_prefix_t()),
        KernelBuffer::MakeDataType(std::vector<KernelPlatform::message_prefix_t>()),
        KernelBuffer::MakeDataType(KernelPlatform::message_index_t()),
        KernelBuffer::MakeDataType(std::vector<KernelPlatform::message_index_t>())
    };

    KernelBuffer::KernelBuffer(std::shared_ptr<KernelParam> param)
        : write_flag(true), read_flag(false), param(param)
    {
        id = next_buffer_id++;
    }

    KernelBuffer::~KernelBuffer()
    {
    }

    std::uint32_t KernelBuffer::getId()
    {
        return id;
    }

    bool KernelBuffer::writePending()
    {
        return write_flag;
    }

    void KernelBuffer::setWriteFlag(bool status)
    {
        write_flag = status;
    }

    bool KernelBuffer::readPending()
    {
        return read_flag;
    }

    void KernelBuffer::setReadFlag(bool status)
    {
        if (param->getAddressQualifier() != KernelParam::AddressQualifier::Private &&
            (param->getAccessQualifier() == KernelParam::AccessQualifier::ReadWrite ||
            param->getAccessQualifier() == KernelParam::AccessQualifier::WriteOnly))
        {
            read_flag = status;
        }
    }

    std::shared_ptr<KernelParam> KernelBuffer::getParam()
    {
        return param;
    }

    bool KernelBuffer::setData(void * data, std::size_t size, std::size_t offset)
    {
        write_flag = true;

        return true;
    }

    std::shared_ptr<KernelBuffer> KernelBuffer::Create(std::shared_ptr<KernelParam> param)
    {
        auto iter = data_types.find(param->getType());

        if (iter == data_types.end())
        {
            throw std::runtime_error("Error: Data type not found! " + param->getType());
        }

        return iter->second->clone(param, 1);
    }

    bool KernelBuffer::Copy(std::shared_ptr<KernelBuffer> src_buff, std::shared_ptr<KernelBuffer> dst_buff, std::size_t size, std::size_t src_offset, std::size_t dst_offset)
    {
        if (src_buff->getType() != dst_buff->getType())
        {
            return false;
        }

        if (!size ||
            size + dst_offset > dst_buff->getElemNum() ||
            size + src_offset > src_buff->getElemNum())
        {
            return false;
        }

        return dst_buff->setData(src_buff->getData(src_offset), size, dst_offset);
    }
}
