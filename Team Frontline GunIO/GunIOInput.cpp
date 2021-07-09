#include "GunIOInput.h"

GunIOInput::GunIOInput(){
    this->reset();
    this->input.trigger = false;
    this->input.x = 0;
    this->input.y = 0;
};