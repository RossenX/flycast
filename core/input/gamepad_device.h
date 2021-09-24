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

#pragma once
#include "types.h"
#include "mapping.h"

#include <map>
#include <memory>
#include <mutex>
#include <vector>

class GamepadDevice
{
public:
	typedef void (*input_detected_cb)(u32 code);
	
	const std::string& api_name() { return _api_name; }
	const std::string& name() { return _name; }
	int maple_port() const { return _maple_port; }
	virtual void set_maple_port(int port) { _maple_port = port; }
	const std::string& unique_id() { return _unique_id; }
	
	void gamepad_btn_reset();
	void gamepad_btn_cleanup();

	virtual bool gamepad_btn_input(u32 code, bool pressed, bool isevent = false);
	bool gamepad_axis_input(int code, int value, bool isevent = false);
	virtual ~GamepadDevice() = default;
	
	virtual void detect_btn_input(input_detected_cb button_pressed);
	void detect_axis_input(input_detected_cb axis_moved);
	virtual void cancel_detect_input()
	{
		_input_detected = nullptr;
	}
	std::shared_ptr<InputMapping> get_input_mapping() { return input_mapper; }
	void save_mapping();
	void save_mapping(int system);

	void verify_or_create_system_mappings();

	virtual const char *get_button_name(u32 code) { return nullptr; }
	virtual const char *get_axis_name(u32 code) { return nullptr; }
	bool remappable() { return _remappable && input_mapper; }
	virtual bool is_virtual_gamepad() { return false; }

	virtual void rumble(float power, float inclination, u32 duration_ms) {}
	virtual void update_rumble() {}
	bool is_rumble_enabled() const { return _rumble_enabled; }

	static void Register(const std::shared_ptr<GamepadDevice>& gamepad);

	static void Unregister(const std::shared_ptr<GamepadDevice>& gamepad);

	static int GetGamepadCount();
	static std::shared_ptr<GamepadDevice> GetGamepad(int index);
	static void SaveMaplePorts();

	static void load_system_mappings(int system = settings.platform.system);
	bool find_mapping(int system);

protected:
	GamepadDevice(int maple_port, const char *api_name, bool remappable = true)
		: _api_name(api_name), _maple_port(maple_port), _input_detected(nullptr), _remappable(remappable)
	{
	}

	bool find_mapping(const char *custom_mapping = nullptr);
	void loadMapping() {
		if (!find_mapping())
			input_mapper = getDefaultMapping();
	}
	virtual std::shared_ptr<InputMapping> getDefaultMapping() {
		return std::make_shared<IdentityInputMapping>();
	}

	virtual void load_axis_min_max(u32 axis) {}
	bool is_detecting_input() { return _input_detected != nullptr; }

	std::string _name;
	std::string _unique_id;
	std::shared_ptr<InputMapping> input_mapper;
	std::map<u32, int> axis_min_values;
	std::map<u32, unsigned int> axis_ranges;
	bool _rumble_enabled = true;

private:
	int get_axis_min_value(u32 axis);
	unsigned int get_axis_range(u32 axis);
	std::string make_mapping_filename(bool instance = false);
	std::string make_mapping_filename(bool instance, int system);

	std::string _api_name;
	int _maple_port;
	bool _detecting_button = false;
	double _detection_start_time = 0.0;
	input_detected_cb _input_detected;
	bool _remappable;

	static std::vector<std::shared_ptr<GamepadDevice>> _gamepads;
	static std::mutex _gamepads_mutex;
};

#ifdef TEST_AUTOMATION
void replay_input();
#endif

extern u32 kcode[4];
extern u8 rt[4], lt[4];
extern s8 joyx[4], joyy[4];
extern s8 joyrx[4], joyry[4];

void UpdateVibration(u32 port, float power, float inclination, u32 duration_ms);

class MouseInputMapping : public InputMapping
{
public:
	MouseInputMapping()
	{
		name = "Mouse";
		set_button(DC_BTN_A, 2);		// Left
		set_button(DC_BTN_B, 1);		// Right
		set_button(DC_BTN_START, 3);	// Middle

		dirty = false;
	}
};

class Mouse : public GamepadDevice
{
protected:
	Mouse(const char *apiName, int maplePort = 0) : GamepadDevice(maplePort, apiName) {
		this->_name = "Mouse";
	}

	virtual std::shared_ptr<InputMapping> getDefaultMapping() override {
		return std::make_shared<MouseInputMapping>();
	}

public:
	enum Button {
		LEFT_BUTTON = 2,
		RIGHT_BUTTON = 1,
		MIDDLE_BUTTON = 3,
		BUTTON_4 = 4,
		BUTTON_5 = 5
	};

	virtual const char *get_button_name(u32 code) override
	{
		switch((Button)code)
		{
		case LEFT_BUTTON:
			return "Left Button";
		case RIGHT_BUTTON:
			return "Right Button";
		case MIDDLE_BUTTON:
			return "Middle Button";
		case BUTTON_4:
			return "Button 4";
		case BUTTON_5:
			return "Button 5";
		default:
			return nullptr;
		}
	}

	void setAbsPos(int x, int y, int width, int height);
	void setRelPos(int deltax, int deltay);
	void setButton(Button button, bool pressed);
	void setWheel(int delta);
};

class SystemMouse : public Mouse
{
protected:
	SystemMouse(const char *apiName, int maplePort = 0) : Mouse(apiName, maplePort) {}

public:
	void setAbsPos(int x, int y, int width, int height);
	void setButton(Button button, bool pressed);
	void setWheel(int delta);
};
