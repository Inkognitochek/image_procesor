//
// Created by Лось Егор Дмитриевич on 05.03.26.
//

#ifndef GRAPHPROCEXAMPLES_PIPELINE_H
#define GRAPHPROCEXAMPLES_PIPELINE_H

#include <vector>
#include "filter.h"

class Pipeline {
public:
    virtual ~Pipeline();

    void AddFilter(BaseFilter* filter);
    void Apply(Image& img);

    const std::vector<BaseFilter*>& GetFilters() const;
    size_t GetFiltersNum() const;

protected:
    std::vector<BaseFilter*> vec_;
};

#endif  // GRAPHPROCEXAMPLES_PIPELINE_H