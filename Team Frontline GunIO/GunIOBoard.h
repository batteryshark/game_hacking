// Limits

#define MAX_RESPONSE_SIZE 32

// Status Codes
#define STATUS_WRITE_OK 0xFF
#define STATUS_READ_OK  0x00
#define STATUS_RW_ERR   0x75

// Operation Codes
#define GUNIO_OPCODE_ROM_VERSION      0x04
#define GUNIO_OPCODE_GET_INPUT_1234   0x07
#define GUNIO_OPCODE_GET_INPUT_12     0x08
#define GUNIO_OPCODE_GET_INPUT_34     0x09
#define GUNIO_OPCODE_GET_DIPSW        0x10
#define GUNIO_OPCODE_SET_DIPSW_1      0x14
#define GUNIO_OPCODE_SET_DIPSW_2      0x18
#define GUNIO_OPCODE_GET_LED_STATE_12 0x1C
#define GUNIO_OPCODE_GET_LED_STATE_34 0x1D

#include <vector>
#include <cstdint>

#include "GunIOInput.h"

class GunIOBoard{
private:
    // System Inputs
    uint8_t  firmware_revision;
    uint8_t  bpc;
    uint8_t  enabled_players;
    // Enabled for Direct Mode (50" DLX) Display
    // Disabled for Mirrored Mode (29" STD) Display
    // Note: This inverts X (LR or RL)
    bool dipsw_1_direct;
    // Determines if 10 or 12 LED results are read from the gun to determine in frame.
    bool dipsw_2_12_led;
    // Emulation Only - Barring Recreation of how the Sensors Work
    uint16_t vscreen_width;
    uint16_t vscreen_height;


    // Player Inputs
    std::vector<GunIOInput*> inputs;

    bool ProcessRequest(uint8_t* request_data, uint32_t request_length, uint8_t* response_data, uint32_t* response_length);
    // Protocol Handlers
    void Protocol_GetRomVersion(uint8_t* response_data, uint32_t* response_length);
    void Protocol_GetDipSw(uint8_t* response_data, uint32_t* response_length);
public:
    GunIOBoard(uint8_t firmware_revision, uint16_t vscreen_width, uint16_t vscreen_height);
    ~GunIOBoard();

    void Protocol_SetDipsw_1(uint8_t *response_data, uint32_t *response_length);

    void Protocol_SetDipsw_2(uint8_t *response_data, uint32_t *response_length);
    void Protocol_GetSensorState(uint8_t* response_data, uint32_t* response_length,uint8_t cmd);
};