#include "game_state.h"
#include "engine.h"
#include "tileset4922.inl"


guiSignal gui_start_menu_panel_begin(f32 width, f32 height) {
	char box_name[128];
    gui_push_bg_color(gv4(0.4,0.4,0.4,0.5));
    guiVec2 middle_point = gv2(get_ngs()->win.ww/2,get_ngs()->win.wh/2); 
	gui_set_next_fixed_x(middle_point.x-width/2);
	gui_set_next_fixed_y(middle_point.y-height/2);
	gui_set_next_fixed_width(width);
	gui_set_next_fixed_height(height);
	gui_set_next_child_layout_axis(AXIS2_Y);
	sprintf(box_name, "main_menu_panel");
	guiSignal main_menu_panel = gui_panel(box_name);
	gui_push_parent(main_menu_panel.box);
    gui_push_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1,0});

}

void gui_start_menu_panel_end() {
    gui_pop_pref_height();
    gui_pop_parent();
}

void do_start_menu_gui() {
    gui_start_menu_panel_begin(100,100);
    //gui_set_next_pref_width((guiSize){GUI_SIZEKIND_TEXT_CONTENT,1,0});
    gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1,0});
    guiSignal play_sig = gui_button("Play");
    if (play_sig.flags & GUI_SIGNAL_FLAG_LMB_RELEASED) {
        // generate a dungeon
        game_state_generate_new_level();
        // and start the game 
        game_state_status_set(GAME_STATUS_IDLE);
    }
    gui_start_menu_panel_end();
}

void do_ingame_gui() {
    //TBA
}

void do_game_gui() {
    gui_build_begin();
    //guiVec4 colors[15] = { gv4(0.95f, 0.61f, 0.73f, 1.0f), gv4(0.55f, 0.81f, 0.95f, 1.0f), gv4(0.68f, 0.85f, 0.90f, 1.0f), gv4(0.67f, 0.88f, 0.69f, 1.0f), gv4(1.00f, 0.78f, 0.49f, 1.0f), gv4(0.98f, 0.93f, 0.36f, 1.0f), gv4(1.00f, 0.63f, 0.48f, 1.0f), gv4(0.55f, 0.81f, 0.25f, 1.0f), gv4(0.85f, 0.44f, 0.84f, 1.0f), gv4(0.94f, 0.90f, 0.55f, 1.0f), gv4(0.80f, 0.52f, 0.25f, 1.0f), gv4(0.70f, 0.13f, 0.13f, 1.0f), gv4(0.56f, 0.93f, 0.56f, 1.0f), gv4(0.93f, 0.51f, 0.93f, 1.0f), gv4(0.95f, 0.61f, 0.73f, 1.0f), };
    if (game_state_status_match(GAME_STATUS_START_MENU)) {
        do_start_menu_gui();
    } else if (game_state_status_match(GAME_STATUS_NEW_TURN) || game_state_status_match(GAME_STATUS_IDLE)) {
        do_ingame_gui();
    }
    gui_build_end();
}