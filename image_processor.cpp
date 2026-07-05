#include <iostream>
#include <exception>
#include <filesystem>

#include "bmp.h"
#include "parser_cmd_args.h"
#include "pipeline.h"
#include "filter_factory.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    try {
        CmdArgsParser parser;
        if (parser.Parse(argc, argv) == CmdArgsParser::WorkMode::ShowHelp) {
            std::cout << "Usage: ./image_processor in.bmp out.bmp [-filter params]" << std::endl;
            return 0;
        }

        Pipeline pipe;
        for (const auto& d : parser.GetFDescriptors()) {
            pipe.AddFilter(FilterFactory::Create(d));
        }

        Image img = ReadImage(parser.GetInputFn());
        pipe.Apply(img);

        WriteImage(parser.GetOutputFn(), img);
        std::cout << "Done!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}