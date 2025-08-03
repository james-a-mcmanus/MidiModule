#ifndef SAFECIRCULARBUFFER_H
#define SAFECIRCULARBUFFER_H

#include "CircularBuffer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// needs to be made safe since will be operated on via xtasks.
template<typename T, size_t Capacity>
class SafeCircularBuffer {
public:
    SafeCircularBuffer(){
        mutex = xSemaphoreCreateMutex();
    }

    bool push(const T& item) {
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            bool success = buffer.push(item);
            xSemaphoreGive(mutex);
            return success;
        }
        return false; // failed to take mutex
    }

    bool pop(T& out) {
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            if (!buffer.isEmpty()) {
                out = buffer.shift();
                xSemaphoreGive(mutex);
                return true;
            }
            xSemaphoreGive(mutex);
        }
        return false;
    }

    bool isEmpty(){
        return buffer.isEmpty();
    }

private:
    CircularBuffer<T, Capacity> buffer;
    SemaphoreHandle_t mutex;
};



#endif