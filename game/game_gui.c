#include "game_state.h"
#include "engine.h"
#include "tileset4922.inl"


guiSignal gui_menu_panel_begin(char *panel_name, Axis2 axis, f32 width, f32 height) {
	char box_name[128];
    gui_push_bg_color(gv4(0.4,0.4,0.4,0.5));
    guiVec2 middle_point = gv2(get_nwin()->ww/2,get_nwin()->wh/2);
	gui_set_next_fixed_x(middle_point.x-width/2);
	gui_set_next_fixed_y(middle_point.y-height/2);
	gui_set_next_fixed_width(width);
	gui_set_next_fixed_height(height);
	gui_set_next_child_layout_axis(axis);
    sprintf(box_name, "menu_panel_%s", panel_name);
	guiSignal main_menu_panel = gui_panel(box_name);
	gui_push_parent(main_menu_panel.box);
    gui_push_pref_height((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1,0});

    return main_menu_panel;
}

void gui_menu_panel_end() {
    gui_pop_pref_height();
    gui_pop_parent();
}

void do_start_menu_gui(GameState *gs) {
    gui_menu_panel_begin("mainmenu", AXIS2_Y, gs->panel_dim.x, gs->panel_dim.y);
    gui_set_next_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1,0});
    gui_set_next_bg_color(gv4(0.23,0.35,0.65,1)); // #3b5ba5
    guiSignal play_sig = gui_button("Play");
    if (play_sig.flags & GUI_SIGNAL_FLAG_LMB_RELEASED) {
        // generate a new level
        game_state_generate_new_level(gs);
        // and start the game
        game_state_status_set(gs, GAME_STATUS_RUNNING);
    }
    gui_set_next_bg_color(gv4(0.9,0.47,0.36,1)); // #e87a5d
    guiSignal options_sig = gui_button("Options");
    if (options_sig.flags & GUI_SIGNAL_FLAG_LMB_RELEASED) {
        // Do Options menu (TBA)
        // -- Comment this out for a sample sound to play (for testing purposes) --
        // nSound *s = push_array(get_global_arena(), nSound, 1);
        // nSoundPcmData sound_data = nsound_gen_sample_pcm_data();
        // nsound_load_from_pcm_data(get_nactx(), s, &sound_data);
        // nsound_load(get_nactx(), s, "assets/sound.wav");
        // nsound_play(get_nactx(), s);
        // nsound_pcm_data_deinit(&sound_data);
        game_state_status_set(gs, GAME_STATUS_OPTIONS_MENU);
    }
    gui_set_next_bg_color(gv4(0.95,0.72,0.25,1)); // #f3b941
    guiSignal quit_sig = gui_button("Quit");
    if (quit_sig.flags & GUI_SIGNAL_FLAG_LMB_RELEASED) {
        exit(1);
    }
    gui_menu_panel_end();
}

void do_options_menu_gui(GameState *gs) {
    gui_menu_panel_begin("optionsmenu", AXIS2_X, gs->panel_dim.x, gs->panel_dim.y);
    {
        static guiScrollPoint sp = {0};
        guiScrollListRowBlock sb[6];
        for (u32 i = 0; i < 6; i+=1){ sb[i] = (guiScrollListRowBlock){.item_count = 1, .row_count = 1}; }
        guiScrollListOptions scroll_opt = {
            .dim_px = gv2(gs->panel_dim.x,gs->panel_dim.y),
            .item_range = (guiRange2){0,2},
            .row_blocks = (guiScrollListRowBlockArray){.blocks = sb, .count = array_count(sb)},
            .row_height_px = 50,
        };
        gui_scroll_list_begin(&scroll_opt, &sp);
        {
            gui_push_pref_width((guiSize){GUI_SIZEKIND_PERCENT_OF_PARENT,1,0});
            // Music button
            gui_set_next_bg_color(gv4(0.25,0.72,0.25,1)); // #f3b941
            guiSignal music_sig = gui_checkbox("Music", &gs->music_enabled);
            // Effects button
            gui_set_next_bg_color(gv4(0.25,0.22,0.55,1)); // #f3b941
            guiSignal effects_sig = gui_checkbox("Effects", &gs->effects_enabled);
            // Fullscreen button
            gui_set_next_bg_color(gv4(0.23,0.35,0.65,1)); // #3b5ba5
            guiSignal fullscreen_sig = gui_checkbox("FullScreen", &gs->fullscreen_enabled);
            // HUD button
            gui_set_next_bg_color(gv4(0.26,0.26,0.3,1)); // #adadc9
            guiSignal hud_sig = gui_checkbox("HUD", &gs->hud_enabled);
            // Endless button
            gui_set_next_bg_color(gv4(0.9,0.47,0.36,1)); // #e87a5d
            guiSignal endless_sig = gui_checkbox("Endless", &gs->endless_mode_enabled);
            // Back button
            gui_set_next_bg_color(gv4(1,0.34,0.2,1)); // #ff5733
            guiSignal back_sig = gui_button("Back");
            if (back_sig.flags & GUI_SIGNAL_FLAG_LMB_RELEASED) {
                game_state_status_set(gs, GAME_STATUS_START_MENU);
            }
            gui_pop_pref_width();
        }
        gui_scroll_list_end();
    }
    gui_menu_panel_end();
}

void do_ingame_gui(GameState *gs) {
    //TBA
}

void do_game_gui(GameState *gs) {
    gui_build_begin();

    if (game_state_status_match(gs, GAME_STATUS_START_MENU)) {
        do_start_menu_gui(gs);
    } else if (game_state_status_match(gs, GAME_STATUS_RUNNING) || game_state_status_match(gs, GAME_STATUS_PAUSED)) {
        do_ingame_gui(gs);
    }else if (game_state_status_match(gs, GAME_STATUS_OPTIONS_MENU)) {
        do_options_menu_gui(gs);
    }
    gui_build_end();
}