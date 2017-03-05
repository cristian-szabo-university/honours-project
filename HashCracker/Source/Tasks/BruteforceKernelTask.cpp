#include "Config.hpp"

#include "Core/Charset.hpp"
#include "Core/HashFunc.hpp"
#include "Core/Logger.hpp"

#include "Tasks/BruteforceKernelTask.hpp"
#include "Tasks/BruteforceSetupTask.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/Kernel.hpp"

#include "ProgramEntry.hpp"

namespace HonoursProject
{
    BruteforceKernelTask::~BruteforceKernelTask()
    {
    }

    void BruteforceKernelTask::run(std::shared_ptr<Device> device, std::shared_ptr<HashFunc> hash_func, std::vector<std::uint32_t> msg_dgst)
    {
        std::vector<std::string> build_opts;
        build_opts.push_back("-DVECTOR_SIZE=" + std::to_string(device->getVectorWidth()));

        std::array<std::uint32_t, 4> digest = hash_func->digest();
        for (std::size_t i = 0; i < digest.size(); i++)
        {
            build_opts.push_back("-DDIGEST_INDEX_" + std::to_string(i) + "=" + std::to_string(digest[i]));
        }

        build_opts.push_back("-DDIGEST_SIZE=" + std::to_string(digest.size()));

        cl::Program::Sources program_sources;
        program_sources.push_back(std::make_pair(inc_types.getData().c_str(), inc_types.getData().size()));
        program_sources.push_back(std::make_pair(inc_hash_const.getData().c_str(), inc_hash_const.getData().size()));
        program_sources.push_back(std::make_pair(inc_hash_func.getData().c_str(), inc_hash_func.getData().size()));
        program_sources.push_back(std::make_pair(inc_hash_comp.getData().c_str(), inc_hash_comp.getData().size()));
        program_sources.push_back(std::make_pair(hash_func->source().getData().c_str(), hash_func->source().getData().size()));

        std::shared_ptr<Program> prog_hash_crack = std::make_shared<Program>(device);

        if (!prog_hash_crack->create(program_sources, build_opts, CL_QUEUE_PROFILING_ENABLE))
        {
            throw std::runtime_error("Error: Failed to create program!");
        }

        if (message_size < 16)
        {
            kernel_hash_crack = prog_hash_crack->findKernel("hash_crack_4");
        }
        else if (message_size < 32)
        {
            kernel_hash_crack = prog_hash_crack->findKernel("hash_crack_8");
        }
        else
        {
            kernel_hash_crack = prog_hash_crack->findKernel("hash_crack_16");
        }

        if (!kernel_hash_crack)
        {
            throw std::runtime_error("Error: No suitable kernel found!");
        }

        kernel_hash_crack->findParam("msg_index")->setAccessQualifier(KernelParam::AccessQualifier::WriteOnly);

        std::shared_ptr<KernelParam> param = kernel_hash_crack->findParam("msg_digest");
        std::shared_ptr<KernelBuffer> buffer = kernel_hash_crack->findBuffer(param);

        std::size_t dgst_size = (msg_dgst.size() * sizeof(std::uint32_t)) / buffer->getElemSize();
        buffer = kernel_hash_crack->createBuffer(param, dgst_size);
        buffer->setData((void*)msg_dgst.data(), dgst_size, 0);
        kernel_hash_crack->updateParam(param, dgst_size);

        cl::Program::Sources gen_word_source;
        gen_word_source.push_back(std::make_pair(inc_types.getData().c_str(), inc_types.getData().size()));
        gen_word_source.push_back(std::make_pair(gen_word.getData().c_str(), gen_word.getData().size()));

        std::shared_ptr<Program> prog_gen_word = std::make_shared<Program>(device);

        if (!prog_gen_word->create(gen_word_source, build_opts))
        {
            throw std::runtime_error("Error: Failed to create program!");
        }

        std::vector<KernelPlatform::charset_t> css;

        for (auto& charset : charsets)
        {
            KernelPlatform::charset_t cs;
            memset(&cs, 0, sizeof(KernelPlatform::charset_t));

            cs.size = charset.getSize();
            std::copy(charset.getData(), charset.getData() + charset.getSize(), cs.data);

            css.push_back(cs);
        }

        kernel_gen_word_suffix = prog_gen_word->findKernel("generate_word_suffix");

        if (!kernel_gen_word_suffix)
        {
            throw std::runtime_error("Error: No suitable kernel found!");
        }

        kernel_gen_word_suffix->findParam("msg")->setAccessQualifier(KernelParam::AccessQualifier::WriteOnly);

        kernel_gen_word_suffix->setParam("css_buf", css);
        kernel_gen_word_suffix->setParam("css_count", css.size());
        kernel_gen_word_suffix->setParam("msg_prefix_size", msg_prefix_size);
        kernel_gen_word_suffix->setParam("msg_suffix_size", msg_suffix_size);
        kernel_gen_word_suffix->setParam("mask", hash_func->mask());
        kernel_gen_word_suffix->setParam("bit14", hash_func->bit14());
        kernel_gen_word_suffix->setParam("bit15", hash_func->bit15());

        kernel_gen_word_prefix = prog_gen_word->findKernel("generate_word_prefix");

        if (!kernel_gen_word_prefix)
        {
            throw std::runtime_error("Error: No suitable kernel found!");
        }

        kernel_gen_word_prefix->findParam("msg_prefix")->setAccessQualifier(KernelParam::AccessQualifier::WriteOnly);

        kernel_gen_word_prefix->setParam("css_buf", css);
        kernel_gen_word_prefix->setParam("css_count", css.size());
        kernel_gen_word_prefix->setParam("msg_prefix_size", 0u);
        kernel_gen_word_prefix->setParam("msg_suffix_size", msg_prefix_size);
        kernel_gen_word_prefix->setParam("mask", 0u);
        kernel_gen_word_prefix->setParam("bit14", 0u);
        kernel_gen_word_prefix->setParam("bit15", 0u);

        Logger::info("Device.#%d: ....: Programs Build Successfuly\n", device->getId());
    }

    void BruteforceKernelTask::transfer(std::shared_ptr<BaseTask> task)
    {
        std::shared_ptr<BruteforceSetupTask> cast_task = std::dynamic_pointer_cast<BruteforceSetupTask>(task);

        if (cast_task)
        {
            charsets = cast_task->getCharsets();

            message_size = cast_task->getMessageSize();

            msg_prefix_size = cast_task->getMessagePrefix();

            msg_suffix_size = cast_task->getMessageSuffix();
        }
    }

    std::vector<Charset> BruteforceKernelTask::getCharsets()
    {
        return charsets;
    }

    std::shared_ptr<Kernel> BruteforceKernelTask::getGenWordPrefixKernel()
    {
        return kernel_gen_word_prefix;
    }

    std::shared_ptr<Kernel> BruteforceKernelTask::getGenWordSuffixKernel()
    {
        return kernel_gen_word_suffix;
    }
}
