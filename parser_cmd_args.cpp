//
// Created by Лось Егор Дмитриевич on 26.02.26.
//

#include "parser_cmd_args.h"
#include <string>

CmdArgsParser::WorkMode CmdArgsParser::Parse(int argc, char** argv) {
    if (argc < 3) {
        return WorkMode::ShowHelp;
    }

    input_fn_ = argv[1];
    output_fn_ = argv[2];

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg[0] == '-') {
            FilterDescriptor descriptor;
            descriptor.name = argv[i];
            f_descriptors_.push_back(descriptor);
        } else {
            if (f_descriptors_.empty()) {
                return WorkMode::ShowHelp;
            }
            f_descriptors_.back().params.push_back(argv[i]);
        }
    }

    return WorkMode::Main;
}