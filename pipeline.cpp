//
// Created by Лось Егор Дмитриевич on 05.03.26.
//

#include "pipeline.h"
#include "filter.h"
#include "image.h"

Pipeline::~Pipeline() {
    for (auto f : vec_) {
        delete f;
    }
}

void Pipeline::AddFilter(BaseFilter* f) {
    vec_.push_back(f);
}

void Pipeline::Apply(Image& img) {
    for (auto f : vec_) {
        f->Apply(img);
    }
}

const std::vector<BaseFilter*>& Pipeline::GetFilters() const {
    return vec_;
}

size_t Pipeline::GetFiltersNum() const {
    return vec_.size();
}