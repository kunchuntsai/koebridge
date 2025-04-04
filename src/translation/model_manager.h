#pragma once

#include <string>

class ModelManager {
public:
    ModelManager();
    ~ModelManager();
    
    bool loadModel(const std::string& modelPath);
    
private:
    void* model; // Will point to translation model
};
