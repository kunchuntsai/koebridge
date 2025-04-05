#pragma once

#include <string>
#include "data_structures.h"

namespace koebridge {
namespace translation {

class ITranslationService {
public:
    virtual ~ITranslationService() = default;
    
    virtual bool initialize() = 0;
    virtual bool translate(const std::string& input, std::string& output) = 0;
    virtual void setOptions(const TranslationOptions& options) = 0;
    virtual TranslationOptions getOptions() const = 0;
    virtual bool isInitialized() const = 0;
};

} // namespace translation
} // namespace koebridge 