#pragma once

#include <string>
#include "model_manager.h"

class Translator {
public:
    Translator();
    
    bool initialize(const std::string& modelPath);
    std::string translate(const std::string& sourceText);
    
private:
    ModelManager modelManager;
};
