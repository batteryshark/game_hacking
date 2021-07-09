#pragma once
#pragma pack(1)
typedef struct _GamepadState {
	unsigned char btn_a;
	unsigned char btn_b;
	unsigned char btn_x;
	unsigned char btn_y;
	unsigned char btn_l1;
	unsigned char btn_r1;
	unsigned char btn_l3;
	unsigned char btn_r3;
	unsigned char btn_start;
	unsigned char btn_back;
	unsigned char dpad_up;
	unsigned char dpad_down;
	unsigned char dpad_left;
	unsigned char dpad_right;
	unsigned char trigger_l2;
	unsigned char trigger_r2;
	unsigned short analog_lx;
	unsigned short analog_ly;
	unsigned short analog_rx;
	unsigned short analog_ry;
}GamepadState;



extern GamepadState gamepad_p1;
extern GamepadState gamepad_p2;
extern GamepadState gamepad_p3;
extern GamepadState gamepad_p4;

void update_gamepad();
unsigned int scale_analog(SHORT analog_in, float src_min, float src_max, float dest_min, float dest_max);
unsigned int get_analog_stick_scaled(SHORT analog_in, float dest_min, float dest_max);