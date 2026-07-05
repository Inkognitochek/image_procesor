add_executable(image_processor image_processor.cpp
)

target_sources(image_processor PRIVATE
        bmp.cpp
        parser_cmd_args.cpp
        pipeline.cpp
)