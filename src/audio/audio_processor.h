#pragma once

#include <vector>

class AudioProcessor {
public:
    AudioProcessor();
    
    void process(const std::vector<float>& input, std::vector<float>& output);
    
private:
    // Implementation details
};
