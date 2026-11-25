#include <string.h>
#include <stdio.h>
#include <stdatomic.h>

#include "../../include/modules/ui.h"
#include "../../include/microui/microui.h"

#define CH SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE

static mu_Style style;

static int text_width(mu_Font font, const char *s, int len) {
    (void)font;
    if (len < 0) len = (int)strlen(s);
    return len * CH;
}
static int text_height(mu_Font font) { (void)font; return CH; }

void init_ui(GUI *gui) {
    mu_init(gui->mu);
    gui->mu->text_width = text_width;
    gui->mu->text_height = text_height;

    style = *gui->mu->style;
    style.colors[MU_COLOR_WINDOWBG]    = mu_color(30, 30, 46, 255);
    style.colors[MU_COLOR_TITLEBG]     = mu_color(49, 50, 68, 255);
    style.colors[MU_COLOR_TEXT]        = mu_color(205, 214, 244, 255);
    style.colors[MU_COLOR_BASE]        = mu_color(69, 71, 90, 255);
    style.colors[MU_COLOR_BASEHOVER]   = mu_color(88, 91, 112, 255);
    style.colors[MU_COLOR_BASEFOCUS]   = mu_color(108, 112, 134, 255);
    style.colors[MU_COLOR_BUTTON]      = mu_color(250, 179, 135, 255);
    style.colors[MU_COLOR_BUTTONHOVER] = mu_color(243, 139, 168, 255);
    style.colors[MU_COLOR_BUTTONFOCUS] = mu_color(235, 160, 172, 255);
    style.colors[MU_COLOR_SCROLLBASE]  = mu_color(88, 91, 112, 255);
    style.colors[MU_COLOR_SCROLLTHUMB] = mu_color(148, 226, 213, 255);
    gui->mu->style = &style;
}

void ui_handle_event(GUI *gui) {
    mu_Context *ctx = gui->mu;
    SDL_Event *e = &gui->event;
    while (SDL_PollEvent(e)) {
        switch (e->type) {
        case SDL_EVENT_MOUSE_MOTION:
            mu_input_mousemove(ctx, (int)e->motion.x, (int)e->motion.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e->button.button == SDL_BUTTON_LEFT)
                mu_input_mousedown(ctx, (int)e->button.x, (int)e->button.y, MU_MOUSE_LEFT);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (e->button.button == SDL_BUTTON_LEFT)
                mu_input_mouseup(ctx, (int)e->button.x, (int)e->button.y, MU_MOUSE_LEFT);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            mu_input_scroll(ctx, 0, (int)(e->wheel.y * -30));
            break;
        case SDL_EVENT_KEY_DOWN:
            if (gui->event.key.scancode == SDL_SCANCODE_SPACE) {
                if (atomic_load(&gui->is_audio_paused) == true) {
                    SDL_ResumeAudioStreamDevice(gui->stream); // Start audio playback
                    atomic_store(&gui->is_audio_paused, false);
                    printf("Audio playing resumed\n");
                } else {
                    SDL_PauseAudioStreamDevice(gui->stream); // Start audio playback
                    atomic_store(&gui->is_audio_paused, true);
                    printf("Audio playing paused\n");
                }
            } else if (gui->event.key.scancode == SDL_SCANCODE_Q) {
                atomic_store(&gui->is_running, false);
                //gui->is_running = false;
            } 
            break;
        default: break;
        }
    }
}

/*
void update_ui(GUI *gui) {
    mu_Context *ctx = gui->mu;
    mu_begin(ctx);

    if (mu_begin_window_ex(
            ctx, 
            "Master", 
            mu_rect(20, 20, 300, 70), 
            MU_OPT_NOCLOSE
    )) {
        mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);

        // Slider example: microui sliders use float, your amplitude is double.
        mu_label(ctx, "Volume");
        float vol = (float)atomic_load(&gui->mixer->amplitude);
        //float vol = (float)gui->mixer->amplitude;
        if (mu_slider(ctx, &vol, 0.0f, 1.0f) & MU_RES_CHANGE) {
            atomic_store(&gui->mixer->amplitude, (double)vol);
            //gui->mixer->amplitude = (double)vol;
        }

        mu_end_window(ctx);
    }

    for (int i = 0; i<gui->mixer->active_channel_count; i++) {
        char title[32];
        snprintf(title, 32, "Channel %d", i);

        if (mu_begin_treenode(ctx, title)) {  
            mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);

            // Slider example: microui sliders use float, your amplitude is double.
            mu_label(ctx, "Volume");
            float vol = (float)atomic_load(&gui->mixer->channels[i].amplitude);
            //float vol = (float)gui->mixer->channels[i].amplitude;
            if (mu_slider(ctx, &vol, 0.0f, 1.0f) & MU_RES_CHANGE) {
                atomic_store(&gui->mixer->channels[i].amplitude, (double)vol);
                //gui->mixer->channels[i].amplitude = vol;
            }

            mu_layout_row(ctx, 1, (int[]){ 140, -1 }, 0);
            char primary_label[64];  
            snprintf(
                primary_label,
                sizeof(primary_label),
                "A Waveform: %s",
                gui->waveforms[
                    atomic_load(&gui->mixer->channels[i].primary_osc.selected_waveform)
                ]->name
                //waveforms[selected]
            );  
              
            mu_layout_begin_column(ctx);
            //mu_layout_row(ctx, 2, (int[]) { 300, 300 }, 0);
            mu_label(ctx, "Primary Oscillator");
            mu_label(ctx, "Volume");
            float primary_vol = (float)atomic_load(&gui->mixer->channels[i].primary_osc.amplitude);
            if (mu_slider(ctx, &primary_vol, 0.0f, 1.0f) & MU_RES_CHANGE) {
                atomic_store(&gui->mixer->channels[i].primary_osc.amplitude, (double)primary_vol);
                //gui->mixer->channels[i].amplitude = primary_vol;
            }
            if (mu_begin_treenode(ctx, primary_label)) {  
                for (int j = 0; j < gui->waveform_count; j++) {  
                    if (mu_button(
                            ctx,
                            gui->waveforms[j]->name
                            //waveforms[i]
                    )) {  
                        atomic_store(&gui->mixer->channels[i].primary_osc.selected_waveform, j);
                        atomic_store(
                            &gui->mixer->channels[i].primary_osc.waveform,
                            gui->waveforms[j]  
                        );
                    }  
                }  
                mu_end_treenode(ctx);  
            }
            mu_layout_end_column(ctx);

            char secondary_label[64];  
            snprintf(
                secondary_label,
                sizeof(secondary_label),
                "B Waveform: %s",
                gui->waveforms[
                    atomic_load(&gui->mixer->channels[i].secondary_osc.selected_waveform)
                ]->name
                //waveforms[selected]
            );  
              
            mu_layout_begin_column(ctx);
            //mu_layout_row(ctx, 2, (int[]) { 300, 300 }, 0);
            mu_label(ctx, "Secondary Oscillator");
            mu_label(ctx, "Volume");
            float secondary_vol = (float)atomic_load(&gui->mixer->channels[i].secondary_osc.amplitude);
            if (mu_slider(ctx, &secondary_vol, 0.0f, 1.0f) & MU_RES_CHANGE) {
                atomic_store(&gui->mixer->channels[i].secondary_osc.amplitude, (double)secondary_vol);
            }
            if (mu_begin_treenode(ctx, secondary_label)) {  
                for (int j = 0; j < gui->waveform_count; j++) {  
                    if (mu_button(
                            ctx,
                            gui->waveforms[j]->name
                            //waveforms[i]
                    )) {  
                        atomic_store(&gui->mixer->channels[i].secondary_osc.selected_waveform, j);
                        atomic_store(
                            &gui->mixer->channels[i].secondary_osc.waveform,
                            gui->waveforms[j]  
                        );
                    }  
                }  
                mu_end_treenode(ctx);  
            }
            mu_layout_end_column(ctx);

            mu_end_treenode(ctx);  
        }
        if (mu_begin_window_ex(
                ctx, 
                title, 
                mu_rect(20, 90+(i*70)+20, 300, 70), 
                MU_OPT_NOCLOSE
        )) {
            mu_end_window(ctx);
        }
    }

    mu_end(ctx);
}
*/
static void osc_ui(mu_Context *ctx, GUI *gui, Oscillator *osc, const char *title) {
    mu_push_id(ctx, title, (int)strlen(title));      // separate ID scope per oscillator

    mu_layout_row(ctx, 1, (int[]){ -1 }, 0);
    mu_label(ctx, title);

    mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);
    mu_label(ctx, "Volume");
    float vol = (float)atomic_load(&osc->amplitude);
    if (mu_slider(ctx, &vol, 0.0f, 1.0f) & MU_RES_CHANGE) {
        atomic_store(&osc->amplitude, (double)vol);
    }

    mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);
    mu_label(ctx, "Limiter");
    float limit = (float)atomic_load(&osc->limit);
    if (mu_slider(ctx, &limit, 0.0f, 1.0f) & MU_RES_CHANGE) {
        atomic_store(&osc->limit, (double)limit);
    }

    int sel = atomic_load(&osc->selected_waveform);
    mu_label(ctx, "Waveform");
    mu_label(ctx, gui->waveforms[sel]->name);        // current one, shown outside the tree label

    mu_layout_row(ctx, 1, (int[]){ -1 }, 0);
    if (mu_begin_treenode(ctx, "Choose waveform")) { // constant label, so it stays open
        for (int j = 0; j < gui->waveform_count; j++) {
            if (mu_button(ctx, gui->waveforms[j]->name) & MU_RES_SUBMIT) {
                atomic_store(&osc->selected_waveform, j);
                atomic_store(&osc->waveform, gui->waveforms[j]);
            }
        }
        mu_end_treenode(ctx);
    }

    mu_pop_id(ctx);
}

void update_ui(GUI *gui) {
    mu_Context *ctx = gui->mu;
    mu_begin(ctx);

    if (mu_begin_window_ex(ctx, "Mixer", mu_rect(20, 20, 320, 520), MU_OPT_NOCLOSE)) {
        mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);
        mu_label(ctx, "Master");
        float master = (float)atomic_load(&gui->mixer->amplitude);
        if (mu_slider(ctx, &master, 0.0f, 1.0f) & MU_RES_CHANGE) {
            atomic_store(&gui->mixer->amplitude, (double)master);
        }

        for (int i = 0; i < gui->mixer->active_channel_count; i++) {
            Channel *ch = &gui->mixer->channels[i];
            char title[32];
            snprintf(title, sizeof(title), "Channel %d", i);

            if (mu_begin_treenode(ctx, title)) {   // titles are unique, so IDs are too
                mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);
                mu_label(ctx, "Volume");
                float cv = (float)atomic_load(&ch->amplitude);
                if (mu_slider(ctx, &cv, 0.0f, 1.0f) & MU_RES_CHANGE) {
                    atomic_store(&ch->amplitude, (double)cv);
                }

                mu_layout_row(ctx, 2, (int[]){ 80, -1 }, 0);
                mu_label(ctx, "Limiter");
                float limit = (float)atomic_load(&gui->mixer->channels[i].limit);
                if (mu_slider(ctx, &limit, 0.0f, 1.0f) & MU_RES_CHANGE) {
                    atomic_store(&gui->mixer->channels[i].limit, (double)limit);
                }

                osc_ui(ctx, gui, &ch->primary_osc,   "Primary");
                osc_ui(ctx, gui, &ch->secondary_osc, "Secondary");
                mu_end_treenode(ctx);
            }
        }
        mu_end_window(ctx);
    }

    mu_end(ctx);
}

// Turn microui's draw commands into SDL3 draw calls.
void draw_ui(GUI *gui) {
    SDL_Renderer *r = gui->renderer;
    mu_Command *cmd = NULL;

    while (mu_next_command(gui->mu, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_RECT: {
            mu_Color c = cmd->rect.color;
            SDL_FRect rect = { cmd->rect.rect.x, cmd->rect.rect.y,
                               cmd->rect.rect.w, cmd->rect.rect.h };
            SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(r, &rect);
            break; }
        case MU_COMMAND_TEXT: {
            mu_Color c = cmd->text.color;
            SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
            SDL_RenderDebugText(r, cmd->text.pos.x, cmd->text.pos.y, cmd->text.str);
            break; }
        case MU_COMMAND_ICON: {
            // Window close button, checkbox tick, etc. A character is enough.
            static const char glyph[] = { 0, 'x', 'v', '>', 'v' };
            mu_Rect b = cmd->icon.rect;
            char s[2] = { glyph[cmd->icon.id % 5], 0 };
            mu_Color c = cmd->icon.color;
            SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
            SDL_RenderDebugText(r, b.x + (b.w - CH) / 2, b.y + (b.h - CH) / 2, s);
            break; }
        case MU_COMMAND_CLIP: {
            mu_Rect cr = cmd->clip.rect;
            if (cr.w > 0xFFFFF) { SDL_SetRenderClipRect(r, NULL); }  // "no clipping"
            else {
                SDL_Rect clip = { cr.x, cr.y, cr.w, cr.h };
                SDL_SetRenderClipRect(r, &clip);
            }
            break; }
        }
    }
    SDL_SetRenderClipRect(r, NULL);
}
