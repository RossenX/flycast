/*
	Copyright 2019 flyinghead

	This file is part of reicast.

    reicast is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    reicast is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with reicast.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "gamepad_device.h"
#include "cfg/cfg.h"
#include "oslib/oslib.h"
#include "rend/gui.h"
#include "emulator.h"
#include "hw/maple/maple_devs.h"
#include "stdclass.h"

#include <algorithm>
#include <climits>
#include <fstream>

#define MAPLE_PORT_CFG_PREFIX "maple_"

// Gamepads
u32 kcode[4] = { ~0u, ~0u, ~0u, ~0u };
u32 kcode_events[4] = { ~0u, ~0u, ~0u, ~0u };
u32 kcode_lastframe[4] = { ~0u, ~0u, ~0u, ~0u };

u32 kcode_events_temp2;


s8 joyx[4];
s8 joyy[4];
s8 joyrx[4];
s8 joyry[4];
u8 rt[4];
u8 lt[4];

std::vector<std::shared_ptr<GamepadDevice>> GamepadDevice::_gamepads;
std::mutex GamepadDevice::_gamepads_mutex;

#ifdef TEST_AUTOMATION
#include "hw/sh4/sh4_sched.h"
static FILE *record_input;
#endif

void GamepadDevice::gamepad_btn_reset(){
	kcode[_maple_port] |= 0xFFFFFF;
	joyx[_maple_port] = 0;
 	joyy[_maple_port] = 0;
 	joyrx[_maple_port] = 0;
 	joyry[_maple_port] = 0;
 	rt[_maple_port] = 0;
 	lt[_maple_port] = 0;

}


void GamepadDevice::gamepad_btn_cleanup(){ // Is the cleanup code messy? Yes, does it work? also yes. Might clean up later make pretty or optimize
	
	kcode_events_temp2 = ~0u;
	memcpy(&kcode_events_temp2, &kcode_events[_maple_port], sizeof(kcode_events[_maple_port]));
	kcode_events[_maple_port] = ~0u;

	u32 UP = DC_DPAD_UP;
	u32 DOWN = DC_DPAD_DOWN;
	u32 LEFT = DC_DPAD_LEFT;
	u32 RIGHT = DC_DPAD_RIGHT;

	u32 _UP = DC_DPAD_UP|EMU_AXIS_DPAD_UP;
	u32 _DOWN = DC_DPAD_DOWN|EMU_AXIS_DPAD_DOWN;
	u32 _LEFT = DC_DPAD_LEFT|EMU_AXIS_DPAD_LEFT;
	u32 _RIGHT = DC_DPAD_RIGHT|EMU_AXIS_DPAD_RIGHT;

	if (((kcode[_maple_port]|kcode_events_temp2) & (LEFT | RIGHT)) == 0) {
		kcode_events_temp2 |= (_LEFT | _RIGHT);
		NOTICE_LOG(INPUT,"Removed LEFT|RIGHT COMBO");
	}else if (((kcode[_maple_port]|kcode_events_temp2) & (UP | DOWN)) == 0) {
		kcode_events_temp2 |= (_UP | _DOWN);
		NOTICE_LOG(INPUT,"Removed UP|DOWN COMBO");
	}

	if ((kcode[_maple_port] & (LEFT | RIGHT)) == 0) {
		kcode[_maple_port] |= (_LEFT | _RIGHT);
		NOTICE_LOG(INPUT,"Removed LEFT|RIGHT kcode");
	}else if ((kcode[_maple_port] & (UP | DOWN)) == 0) {
		kcode[_maple_port] |= (_UP | _DOWN);
		NOTICE_LOG(INPUT,"Removed LEFT|RIGHT kcode");
	}

	kcode[_maple_port] &= kcode_events_temp2;
	
	// DOWN LEFT
	if ((kcode[_maple_port] & (DOWN | LEFT)) == 0) {
		if ((kcode_lastframe[_maple_port] & (DOWN | RIGHT))==0) {
			kcode[_maple_port] |= _LEFT;
			kcode_events[_maple_port] &= ~_DOWN;
			NOTICE_LOG(INPUT,"DOWN RIGHT > DOWN LEFT");
		} else if ((kcode_lastframe[_maple_port] & (UP | LEFT))==0) {
			kcode[_maple_port] |= _DOWN;
			kcode_events[_maple_port] &= ~_LEFT;
			NOTICE_LOG(INPUT,"UP LEFT > DOWN LEFT");
		}

	} // DOWN RIGHT
	else if ((kcode[_maple_port] & (DOWN | RIGHT))==0) {

		if ((kcode_lastframe[_maple_port] & (DOWN | LEFT))==0) {
			kcode[_maple_port] |= _RIGHT;
			kcode_events[_maple_port] &= ~_DOWN;
			NOTICE_LOG(INPUT,"DOWN LEFT > DOWN RIGHT");
		}else if ((kcode_lastframe[_maple_port] & (UP | RIGHT))==0) {
			kcode[_maple_port] |= _DOWN;
			kcode_events[_maple_port] &= ~_RIGHT;
			NOTICE_LOG(INPUT,"UP RIGHT > DOWN RIGHT");
		}

	} // UP LEFT
	else if ((kcode[_maple_port] & (UP | LEFT))==0) {

		if ((kcode_lastframe[_maple_port] & (UP | RIGHT))==0) {
			kcode[_maple_port] |= LEFT;
			kcode_events[_maple_port] &= ~_UP;
			NOTICE_LOG(INPUT,"UP RIGHT > UP LEFT");
		}else if ((kcode_lastframe[_maple_port] & (DOWN | LEFT))==0) {
			kcode[_maple_port] |= _UP;
			kcode_events[_maple_port] &= ~_LEFT;
			NOTICE_LOG(INPUT,"DOWN LEFT > UP LEFT");
		}

	} // UP RIGHT
	else if ((kcode[_maple_port] & (UP | RIGHT))==0) {
		if ((kcode_lastframe[_maple_port] & (UP | LEFT))==0) {
			kcode[_maple_port] |= _RIGHT;
			kcode_events[_maple_port] &= ~_UP;
			NOTICE_LOG(INPUT,"UP LEFT > UP RIGHT");
		}else if ((kcode_lastframe[_maple_port] & (DOWN | RIGHT))==0) {
			kcode[_maple_port] |= _UP;
			kcode_events[_maple_port] &= ~_RIGHT;
			NOTICE_LOG(INPUT,"DOWN RIGHT > UP RIGHT");
		}
	}

	memcpy(&kcode_lastframe[_maple_port],&kcode[_maple_port],sizeof(kcode[_maple_port]));
}

bool GamepadDevice::gamepad_btn_input(u32 code, bool pressed, bool isevent)
{
	u32 *InputsToUse;
	if(isevent){InputsToUse = kcode_events;
	}else{InputsToUse = kcode;}

	if (_input_detected != nullptr && _detecting_button
			&& os_GetSeconds() >= _detection_start_time && pressed){
		_input_detected(code);
		_input_detected = nullptr;
		return true;
	}

	if (!input_mapper || _maple_port < 0 || _maple_port > (int)ARRAY_SIZE(kcode) || gui_is_open())
		return false;

	auto handle_key = [&](u32 port, DreamcastKey key)
	{
		if (key == EMU_BTN_NONE)
			return false;

		if (key <= DC_BTN_RELOAD)
		{
			if (pressed) InputsToUse[port] &= ~key;

			if (isevent && !pressed)
			{
				switch (key)
				{
				case DC_DPAD_UP:
				case DC_DPAD_DOWN:
					if (((kcode_events[port] & DC_DPAD_LEFT) == 0) || ((kcode_events[port] & DC_DPAD_RIGHT) == 0))
					{
						kcode_events[port] &= ~key;
						NOTICE_LOG(INPUT, "Added UP/DOWN");
					}
				case DC_DPAD_LEFT:
				case DC_DPAD_RIGHT:
					if (((kcode_events[port] & DC_DPAD_UP) == 0) || ((kcode_events[port] & DC_DPAD_DOWN) == 0))
					{
						kcode_events[port] &= ~key;
						NOTICE_LOG(INPUT, "Added LEFT/RIGHT");
					}
				}
			}

#ifdef TEST_AUTOMATION
			if (record_input != NULL)
				fprintf(record_input, "%ld button %x %04x\n", sh4_sched_now64(), port, kcode[port]);
#endif
		}
		else
		{
			// If this is GGPO set the digital controls instead
			int _joyx = 0;
			int _joyy = 0;
			switch (key)
			{
			case EMU_BTN_ESCAPE:
				if (pressed)
					dc_exit();
				break;
			case EMU_BTN_MENU:
				if (pressed)
					gui_open_settings();
				break;
			case EMU_BTN_FFORWARD:
				if (pressed && !gui_is_open())
					settings.input.fastForwardMode = !settings.input.fastForwardMode && !settings.online;
				break;
			case EMU_BTN_TRIGGER_LEFT:
				lt[port] = pressed ? 255 : 0;
				break;
			case EMU_BTN_TRIGGER_RIGHT:
				rt[port] = pressed ? 255 : 0;
				break;
			case EMU_BTN_ANA_UP:
				joyy[port] -= pressed ? 128 : 0;
				break;
			case EMU_BTN_ANA_DOWN:
				joyy[port] += pressed ? 127 : 0;
				break;
			case EMU_BTN_ANA_LEFT:
				joyx[port] -= pressed ? 128 : 0;
				break;
			case EMU_BTN_ANA_RIGHT:
				joyx[port] += pressed ? 127 : 0;
				break;
			default:
				return false;
			}
			
		}

		DEBUG_LOG(INPUT, "%d: BUTTON %s %x -> %d. kcode=%x", port, pressed ? "down" : "up", code, key, kcode[port]);

		return true;
	};

	bool rc = false;
	if (_maple_port == 4)
	{
		for (int port = 0; port < 4; port++)
		{
			DreamcastKey key = input_mapper->get_button_id(port, code);
			rc = handle_key(port, key) || rc;
		}
	}
	else
	{
		DreamcastKey key = input_mapper->get_button_id(0, code);
		rc = handle_key(_maple_port, key);
	}

	return rc;
}

bool GamepadDevice::gamepad_axis_input(int code, int value, bool isevent)
{
	u32 *InputsToUse;
	if(isevent){InputsToUse = kcode_events;
	}else{InputsToUse = kcode;}

	if (input_mapper->get_axis_inverted(0, code)) value *= -1;
	
	s32 v; // The final Value
	if(code > 3){
		v = value / 128; // Should make a value between 0-256
		if(v < 0) v = 0;
		if(v > 255) v = 255;
	}else{
		v = value / 256; // Should make a value between 0-128
		if(v < -128) v = -128;
		if(v > 127) v = 127;
		
	}
	
	// This little bit looks to be for the keymapping
	if (_input_detected != NULL && !_detecting_button && os_GetSeconds() >= _detection_start_time && (v >= 64 || v <= -64)){
		_input_detected(code);
		_input_detected = NULL;
		return true;
	}

	if (!input_mapper || _maple_port < 0 || _maple_port > 4 || gui_is_open()) return false;

	int _deadzone = int((255 * input_mapper->dead_zone) / 100);
	if(code <= 3){_deadzone /= 2;} // Sticks have half the deadzone because they only go up to 128

	auto handle_axis = [&](u32 port, DreamcastKey key)
	{
		if ((int)key < 0x10000) // Key Codes DIGITAL
		{
			if (code > 3)
			{ // This is a trigger
				if (v >= _deadzone)
					InputsToUse[port] |= key;
					
			}
			else
			{
				bool pressed = false;
				if (v <= -_deadzone){
					InputsToUse[port] &= ~key;
					pressed = true;
				}else if (v >= _deadzone){
					InputsToUse[port] &= ~(key << 1);
					pressed = true;
				}
					
				if (isevent && !pressed)
				{
					switch (key)
					{
					case DC_DPAD_UP:
					case DC_DPAD_DOWN:
						if (((kcode_events[port] & DC_DPAD_LEFT) == 0) || ((kcode_events[port] & DC_DPAD_RIGHT) == 0))
						{
							if((kcode_lastframe[port] & DC_DPAD_UP)==0){
								kcode_events[port] &= ~DC_DPAD_UP;
								NOTICE_LOG(INPUT,"Added LEFT Analog");
							}

							if((kcode_lastframe[port] & DC_DPAD_DOWN)==0){
								kcode_events[port] &= ~DC_DPAD_DOWN;
								NOTICE_LOG(INPUT,"Added RIGHT Analog");
							}
							
						}
						break;
					case DC_DPAD_LEFT:
					case DC_DPAD_RIGHT:
						if (((kcode_events[port] & DC_DPAD_UP) == 0) || ((kcode_events[port] & DC_DPAD_DOWN) == 0)){
							
							if((kcode_lastframe[port] & DC_DPAD_LEFT)==0){
								kcode_events[port] &= ~DC_DPAD_LEFT;
								NOTICE_LOG(INPUT,"Added UP Analog");
							}

							if((kcode_lastframe[port] & DC_DPAD_RIGHT)==0){
								kcode_events[port] &= ~DC_DPAD_RIGHT;
								NOTICE_LOG(INPUT,"Added DOWN Analog");
							}
							
						}
						break;
					}
				}
			}
		}
		else if (((int)key >> 16) == 1)	// Triggers
		{
			if(v < 0)
			{
				v = 0;
			}
			if(v > 255)
			{
				v = 255;
			}
			
			if (key == DC_AXIS_LT)
			{
				lt[port] = (u8)v;
			}
			else if (key == DC_AXIS_RT)
			{
				rt[port] = (u8)v;
			}
			else
			{
				return false;
			}
			
		}
		else if (((int)key >> 16) == 2) // Analog axes
		{
			s8 *this_axis;
			s8 *other_axis;
			switch (key)
			{
			case DC_AXIS_X:
				this_axis = &joyx[port];
				other_axis = &joyy[port];
				break;

			case DC_AXIS_Y:
				this_axis = &joyy[port];
				other_axis = &joyx[port];
				break;

			case DC_AXIS_X2:
				this_axis = &joyrx[port];
				other_axis = &joyry[port];
				break;

			case DC_AXIS_Y2:
				this_axis = &joyry[port];
				other_axis = &joyrx[port];
				break;

			default:
				return false;
			}
			
			if (abs(v) > _deadzone) *this_axis = (s8)v;
			
		}
		else if (((int)key >> 16) == 4) // Map triggers to digital buttons
		{
			if (abs(v) >= _deadzone)
				InputsToUse[port] &= ~(key & ~0x40000); // button pressed
		}
		else
			return false;

		return true;
	};

	bool rc = false;
	if (_maple_port == 4)
	{
		for (u32 port = 0; port < 4; port++)
		{
			DreamcastKey key = input_mapper->get_axis_id(port, code);
			rc = handle_axis(port, key) || rc;
		}
	}
	else
	{
		DreamcastKey key = input_mapper->get_axis_id(0, code);
		rc = handle_axis(_maple_port, key);
	}

	return rc;
}

int GamepadDevice::get_axis_min_value(u32 axis) {
	auto it = axis_min_values.find(axis);
	if (it == axis_min_values.end()) {
		load_axis_min_max(axis);
		it = axis_min_values.find(axis);
		if (it == axis_min_values.end())
			return INT_MIN;
	}
	return it->second;
}

unsigned int GamepadDevice::get_axis_range(u32 axis) {
	auto it = axis_ranges.find(axis);
	if (it == axis_ranges.end()) {
		load_axis_min_max(axis);
		it = axis_ranges.find(axis);
		if (it == axis_ranges.end())
			return UINT_MAX;
	}
	return it->second;
}

std::string GamepadDevice::make_mapping_filename(bool instance)
{
	std::string mapping_file = api_name() + "_" + name();
	if (instance)
		mapping_file += "-" + _unique_id;
	std::replace(mapping_file.begin(), mapping_file.end(), '/', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '\\', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), ':', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '?', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '*', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '|', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '"', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '<', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '>', '-');
	mapping_file += ".cfg";

	return mapping_file;
}

void GamepadDevice::verify_or_create_system_mappings()
{
	std::string dc_name = make_mapping_filename(false, 0);
	std::string arcade_name = make_mapping_filename(false, 2);

	std::string dc_path = get_readonly_config_path(std::string("mappings/") + dc_name);
	std::string arcade_path = get_readonly_config_path(std::string("mappings/") + arcade_name);

	if (!file_exists(arcade_path))
	{
		save_mapping(2);
		input_mapper->ClearMappings();
	}
	if (!file_exists(dc_path))
	{
		save_mapping(0);
		input_mapper->ClearMappings();
	}

	find_mapping(DC_PLATFORM_DREAMCAST);
}

void GamepadDevice::load_system_mappings(int system)
{
	for (int i = 0; i < GetGamepadCount(); i++)
	{
		std::shared_ptr<GamepadDevice> gamepad = GetGamepad(i);
		gamepad->find_mapping(system);
	}
}

std::string GamepadDevice::make_mapping_filename(bool instance, int system)
{
	std::string mapping_file = api_name() + "_" + name();
	if (instance)
		mapping_file += "-" + _unique_id;
	if (system != 0)
		mapping_file += "_arcade";
	std::replace(mapping_file.begin(), mapping_file.end(), '/', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '\\', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), ':', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '?', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '*', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '|', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '"', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '<', '-');
	std::replace(mapping_file.begin(), mapping_file.end(), '>', '-');
	mapping_file += ".cfg";

	return mapping_file;
}

bool GamepadDevice::find_mapping(int system)
{
	std::string mapping_file;
	mapping_file = make_mapping_filename(false, system);

	// fall back on default flycast mapping filename if system profile not found
	std::string system_mapping_path = get_readonly_config_path(std::string("mappings/") + mapping_file);
	if (!file_exists(system_mapping_path))
		mapping_file = make_mapping_filename(false);

	std::shared_ptr<InputMapping> mapper = InputMapping::LoadMapping(mapping_file.c_str());

	if (!mapper)
		return false;
	input_mapper = mapper;
	return true;
}

bool GamepadDevice::find_mapping(const char *custom_mapping /* = nullptr */)
{
	std::string mapping_file;
	if (custom_mapping != nullptr)
		mapping_file = custom_mapping;
	else
		mapping_file = make_mapping_filename(true);

	input_mapper = InputMapping::LoadMapping(mapping_file.c_str());
	if (!input_mapper && custom_mapping == nullptr)
	{
		mapping_file = make_mapping_filename(false);
		input_mapper = InputMapping::LoadMapping(mapping_file.c_str());
	}
	return !!input_mapper;
}

int GamepadDevice::GetGamepadCount()
{
	_gamepads_mutex.lock();
	int count = _gamepads.size();
	_gamepads_mutex.unlock();
	return count;
}

std::shared_ptr<GamepadDevice> GamepadDevice::GetGamepad(int index)
{
	_gamepads_mutex.lock();
	std::shared_ptr<GamepadDevice> dev;
	if (index >= 0 && index < (int)_gamepads.size())
		dev = _gamepads[index];
	else
		dev = NULL;
	_gamepads_mutex.unlock();
	return dev;
}

void GamepadDevice::save_mapping()
{
	if (!input_mapper)
		return;
	std::string filename = make_mapping_filename();
	InputMapping::SaveMapping(filename.c_str(), input_mapper);
}

void GamepadDevice::save_mapping(int system)
{
	if (!input_mapper)
		return;
	std::string filename = make_mapping_filename(false, system);
	input_mapper->set_dirty();
	InputMapping::SaveMapping(filename.c_str(), input_mapper);
}

void UpdateVibration(u32 port, float power, float inclination, u32 duration_ms)
{
	int i = GamepadDevice::GetGamepadCount() - 1;
	for ( ; i >= 0; i--)
	{
		std::shared_ptr<GamepadDevice> gamepad = GamepadDevice::GetGamepad(i);
		if (gamepad != NULL && gamepad->maple_port() == (int)port && gamepad->is_rumble_enabled())
			gamepad->rumble(power, inclination, duration_ms);
	}
}

void GamepadDevice::detect_btn_input(input_detected_cb button_pressed)
{
	_input_detected = button_pressed;
	_detecting_button = true;
	_detection_start_time = os_GetSeconds() + 0.2;
}

void GamepadDevice::detect_axis_input(input_detected_cb axis_moved)
{
	_input_detected = axis_moved;
	_detecting_button = false;
	_detection_start_time = os_GetSeconds() + 0.2;
}

#ifdef TEST_AUTOMATION
static FILE *get_record_input(bool write)
{
	if (write && !cfgLoadBool("record", "record_input", false))
		return NULL;
	if (!write && !cfgLoadBool("record", "replay_input", false))
		return NULL;
	std::string game_dir = settings.imgread.ImagePath;
	size_t slash = game_dir.find_last_of("/");
	size_t dot = game_dir.find_last_of(".");
	std::string input_file = "scripts/" + game_dir.substr(slash + 1, dot - slash) + "input";
	return nowide::fopen(input_file.c_str(), write ? "w" : "r");
}
#endif

void GamepadDevice::Register(const std::shared_ptr<GamepadDevice>& gamepad)
{
	int maple_port = cfgLoadInt("input",
			MAPLE_PORT_CFG_PREFIX + gamepad->unique_id(), 12345);
	if (maple_port != 12345)
		gamepad->set_maple_port(maple_port);
#ifdef TEST_AUTOMATION
	if (record_input == NULL)
	{
		record_input = get_record_input(true);
		if (record_input != NULL)
			setbuf(record_input, NULL);
	}
#endif
	_gamepads_mutex.lock();
	_gamepads.push_back(gamepad);
	_gamepads_mutex.unlock();
}

void GamepadDevice::Unregister(const std::shared_ptr<GamepadDevice>& gamepad)
{
	gamepad->save_mapping();
	_gamepads_mutex.lock();
	for (auto it = _gamepads.begin(); it != _gamepads.end(); it++)
		if (*it == gamepad) {
			_gamepads.erase(it);
			break;
		}
	_gamepads_mutex.unlock();
}

void GamepadDevice::SaveMaplePorts()
{
	for (int i = 0; i < GamepadDevice::GetGamepadCount(); i++)
	{
		std::shared_ptr<GamepadDevice> gamepad = GamepadDevice::GetGamepad(i);
		if (gamepad != NULL && !gamepad->unique_id().empty())
			cfgSaveInt("input", MAPLE_PORT_CFG_PREFIX + gamepad->unique_id(), gamepad->maple_port());
	}
}

void Mouse::setAbsPos(int x, int y, int width, int height) {
	SetMousePosition(x, y, width, height, maple_port());
}

void Mouse::setRelPos(int deltax, int deltay) {
	SetRelativeMousePosition(deltax, deltay, maple_port());
}

void Mouse::setWheel(int delta) {
	if (maple_port() >= 0 && maple_port() < (int)ARRAY_SIZE(mo_wheel_delta))
		mo_wheel_delta[maple_port()] += delta;
}

void Mouse::setButton(Button button, bool pressed)
{
	if (maple_port() >= 0 && maple_port() < (int)ARRAY_SIZE(mo_buttons))
	{
		if (pressed)
			mo_buttons[maple_port()] &= ~(1 << (int)button);
		else
			mo_buttons[maple_port()] |= 1 << (int)button;
	}
	if (gui_is_open() && !is_detecting_input())
		// Don't register mouse clicks as gamepad presses when gui is open
		// This makes the gamepad presses to be handled first and the mouse position to be ignored
		return;
	gamepad_btn_input(button, pressed);
}


void SystemMouse::setAbsPos(int x, int y, int width, int height) {
	gui_set_mouse_position(x, y);
	Mouse::setAbsPos(x, y, width, height);
}

void SystemMouse::setButton(Button button, bool pressed) {
	int uiBtn = (int)button - 1;
	if (uiBtn < 2)
		uiBtn ^= 1;
	gui_set_mouse_button(uiBtn, pressed);
	Mouse::setButton(button, pressed);
}

void SystemMouse::setWheel(int delta) {
	gui_set_mouse_wheel(delta * 35);
	Mouse::setWheel(delta);
}

#ifdef TEST_AUTOMATION
#include "cfg/option.h"
static bool replay_inited;
FILE *replay_file;
u64 next_event;
u32 next_port;
u32 next_kcode;
bool do_screenshot;

void replay_input()
{
	if (!replay_inited)
	{
		replay_file = get_record_input(false);
		replay_inited = true;
	}
	u64 now = sh4_sched_now64();
	if (config::UseReios)
	{
		// Account for the swirl time
		if (config::Broadcast == 0)
			now = std::max((int64_t)now - 2152626532L, 0L);
		else
			now = std::max((int64_t)now - 2191059108L, 0L);
	}
	if (replay_file == NULL)
	{
		if (next_event > 0 && now - next_event > SH4_MAIN_CLOCK * 5)
			die("Automation time-out after 5 s\n");
		return;
	}
	while (next_event <= now)
	{
		if (next_event > 0)
			kcode[next_port] = next_kcode;

		char action[32];
		if (fscanf(replay_file, "%ld %s %x %x\n", &next_event, action, &next_port, &next_kcode) != 4)
		{
			fclose(replay_file);
			replay_file = NULL;
			NOTICE_LOG(INPUT, "Input replay terminated");
			do_screenshot = true;
			break;
		}
	}
}
#endif
