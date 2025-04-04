#pragma once

class AudioCapture {
public:
    AudioCapture();
    ~AudioCapture();
    
    bool start();
    void stop();
    
private:
    // Implementation details
};
