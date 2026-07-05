//
// Created by Лось Егор Дмитриевич on 26.02.26.
//

#ifndef GRAPHPROCEXAMPLES_PARSER_CMD_ARGS_H
#define GRAPHPROCEXAMPLES_PARSER_CMD_ARGS_H

#include <cstddef>
#include <vector>

struct FilterDescriptor {
    const char* name;
    std::vector<const char*> params;
};

class CmdArgsParser {
public:
    static const size_t MIN_ARGS_NUM_4_MAIN_MODE = 3;

    enum class WorkMode { Main, ShowHelp, ArgEnum };

    CmdArgsParser() : input_fn_(nullptr), output_fn_(nullptr) {
    }

    WorkMode Parse(int argc, char** argv);

    const char* GetInputFn() const {
        return input_fn_;
    }
    const char* GetOutputFn() const {
        return output_fn_;
    }
    const std::vector<FilterDescriptor>& GetFDescriptors() const {
        return f_descriptors_;
    }

private:
    const char* input_fn_;
    const char* output_fn_;
    std::vector<FilterDescriptor> f_descriptors_;
};

#endif  // GRAPHPROCEXAMPLES_PARSER_CMD_ARGS_H