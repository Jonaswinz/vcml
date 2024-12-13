/******************************************************************************
 *                                                                            *
 * Copyright (C) 2022 MachineWare GmbH                                        *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 *                                                                            *
 ******************************************************************************/

#ifndef VCML_UI_INPUT_H
#define VCML_UI_INPUT_H

#include "vcml/core/types.h"
#include "vcml/logging/logger.h"
#include "vcml/ui/codes.h"
#include "vcml/ui/keymap.h"

namespace vcml {
namespace ui {

struct input_event {
    u16 type;
    u16 code;
    i32 state;

    constexpr bool is_syn() const { return type == EV_SYN; }
    constexpr bool is_key() const { return type == EV_KEY; }
    constexpr bool is_abs() const { return type == EV_ABS; }
    constexpr bool is_rel() const { return type == EV_REL; }
};

class input
{
private:
    string m_name;

    mutable mutex m_mutex;
    queue<input_event> m_events;

    static unordered_map<string, input*>& all_inputs();

protected:
    void push_event(const input_event& ev);
    void push_key(u16 key, u32 state);
    void push_rel(u16 axis, i32 delta);
    void push_abs(u16 axis, u32 state);
    void push_syn();

public:
    const char* input_name() const { return m_name.c_str(); }

    input(const string& name);
    virtual ~input();

    bool has_events() const;
    bool pop_event(input_event& ev);

    virtual void notify_key(u32 symbol, bool down) = 0;
    virtual void notify_btn(u32 button, bool down) = 0;
    virtual void notify_pos(u32 x, u32 y, u32 w, u32 h) = 0;

    template <typename T = input>
    static vector<T*> all();

    template <typename T = input>
    static T* find(const string& name);
};

template <typename T>
vector<T*> input::all() {
    vector<T*> res;
    for (auto [name, obj] : all_inputs()) {
        T* ptr = dynamic_cast<T*>(obj);
        if (ptr)
            res.push_back(ptr);
    }

    return res;
}

template <typename T>
T* input::find(const string& name) {
    auto it = all_inputs().find(name);
    if (it == all_inputs().end())
        return nullptr;
    return dynamic_cast<T*>(it->second);
}

class keyboard : public input
{
private:
    bool m_ctrl_l;
    bool m_ctrl_r;
    bool m_shift_l;
    bool m_shift_r;
    bool m_capsl;
    bool m_alt_l;
    bool m_alt_r;
    bool m_meta_l;
    bool m_meta_r;
    u32 m_prev_sym;
    string m_layout;

public:
    bool ctrl_l() const { return m_ctrl_l; }
    bool ctrl_r() const { return m_ctrl_r; }
    bool ctrl() const { return m_ctrl_l || m_ctrl_r; }

    bool shift_l() const { return m_shift_l; }
    bool shift_r() const { return m_shift_r; }
    bool shift() const { return m_shift_l || m_shift_r; }

    bool alt_l() const { return m_alt_l; }
    bool alt_r() const { return m_alt_r; }
    bool alt() const { return m_alt_l || m_alt_r; }

    bool meta_l() const { return m_meta_l; }
    bool meta_r() const { return m_meta_r; }
    bool meta() const { return m_meta_l || m_meta_r; }

    bool capslock() const { return m_capsl; }

    const char* layout() const { return m_layout.c_str(); }
    void set_layout(const string& layout) { m_layout = layout; }

    keyboard(const string& name, const string& layout = "");
    virtual ~keyboard() = default;

    virtual void notify_key(u32 symbol, bool down) override;
    virtual void notify_btn(u32 button, bool down) override;
    virtual void notify_pos(u32 x, u32 y, u32 w, u32 h) override;
};

enum mouse_buttons : u32 {
    BUTTON_NONE = 0,
    BUTTON_LEFT = bit(0),
    BUTTON_RIGHT = bit(1),
    BUTTON_MIDDLE = bit(2),
    BUTTON_WHEEL_UP = bit(3),
    BUTTON_WHEEL_DOWN = bit(4),
};

class mouse : public input
{
private:
    u32 m_buttons;
    u32 m_xabs;
    u32 m_yabs;

public:
    u32 x() const { return m_xabs; }
    u32 y() const { return m_yabs; }

    bool left() const { return m_buttons & BUTTON_LEFT; }
    bool middle() const { return m_buttons & BUTTON_MIDDLE; }
    bool right() const { return m_buttons & BUTTON_RIGHT; }

    mouse(const string& name);
    virtual ~mouse() = default;

    virtual void notify_key(u32 symbol, bool down) override;
    virtual void notify_btn(u32 button, bool down) override;
    virtual void notify_pos(u32 x, u32 y, u32 w, u32 h) override;
};

class touchpad : public input
{
private:
    bool m_touch;
    u32 m_xabs;
    u32 m_yabs;

public:
    u32 x() const { return m_xabs; }
    u32 y() const { return m_yabs; }

    bool is_touching() const { return m_touch; }

    touchpad(const string& name);
    virtual ~touchpad() = default;

    virtual void notify_key(u32 symbol, bool down) override;
    virtual void notify_btn(u32 button, bool down) override;
    virtual void notify_pos(u32 x, u32 y, u32 w, u32 h) override;
};

} // namespace ui
} // namespace vcml

#endif
