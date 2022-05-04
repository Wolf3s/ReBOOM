CFLAGS= -std=c11 -pedantic -Wall -DUNIX -g `sdl2-config --cflags --libs`
LDFLAGS= -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_net -lm
OUTPUT= build
OBJS=				\
		$(OUTPUT)/am_map.o \
		$(OUTPUT)/d_deh.o \
		$(OUTPUT)/d_items_struct.o \
		$(OUTPUT)/d_main.o \
		$(OUTPUT)/d_net.o \
		$(OUTPUT)/doomdef.o \
		$(OUTPUT)/doomstat.o \
		$(OUTPUT)/dstrings.o \
		$(OUTPUT)/f_finale.o \
		$(OUTPUT)/f_wipe.o \
		$(OUTPUT)/g_game.o \
		$(OUTPUT)/hu_lib.o \
		$(OUTPUT)/hu_stuff.o \
		$(OUTPUT)/i_net.o \
		$(OUTPUT)/i_sound.o \
		$(OUTPUT)/i_system.o \
		$(OUTPUT)/i_video.o \
		$(OUTPUT)/info.o \
		$(OUTPUT)/lprintf.o \
		$(OUTPUT)/m_argv.o \
		$(OUTPUT)/m_bbox.o \
		$(OUTPUT)/m_cheat.o \
		$(OUTPUT)/m_menu.o \
		$(OUTPUT)/m_misc.o \
		$(OUTPUT)/m_random.o \
		$(OUTPUT)/mmus2mid.o \
		$(OUTPUT)/p_ceilng.o \
		$(OUTPUT)/p_doors.o \
		$(OUTPUT)/p_enemy.o \
		$(OUTPUT)/p_floor.o \
		$(OUTPUT)/p_genlin.o \
		$(OUTPUT)/p_inter.o \
		$(OUTPUT)/p_lights.o \
		$(OUTPUT)/p_map.o \
		$(OUTPUT)/p_maputl.o \
		$(OUTPUT)/p_mobj.o \
		$(OUTPUT)/p_plats.o \
		$(OUTPUT)/p_pspr.o \
		$(OUTPUT)/p_saveg.o \
		$(OUTPUT)/p_setup.o \
		$(OUTPUT)/p_sight.o \
		$(OUTPUT)/p_spec.o \
		$(OUTPUT)/p_switch.o \
		$(OUTPUT)/p_telept.o \
		$(OUTPUT)/p_tick.o \
		$(OUTPUT)/p_user.o \
		$(OUTPUT)/r_bsp.o \
		$(OUTPUT)/r_data.o \
		$(OUTPUT)/r_draw.o \
		$(OUTPUT)/r_main.o \
		$(OUTPUT)/r_plane.o \
		$(OUTPUT)/r_segs.o \
		$(OUTPUT)/r_sky.o \
		$(OUTPUT)/r_things.o \
		$(OUTPUT)/s_musinfo.o \
		$(OUTPUT)/s_sound.o \
		$(OUTPUT)/sounds.o \
		$(OUTPUT)/st_lib.o \
		$(OUTPUT)/st_stuff.o \
		$(OUTPUT)/tables.o \
		$(OUTPUT)/umapinfo.o \
		$(OUTPUT)/uscanner.o \
		$(OUTPUT)/v_video.o \
		$(OUTPUT)/version.o \
		$(OUTPUT)/w_wad.o \
		$(OUTPUT)/wi_stuff.o \
		$(OUTPUT)/z_zone.o \
		$(OUTPUT)/txt_sdl.o \
		$(OUTPUT)/txt_utf8.o \
		$(OUTPUT)/txt_widget.o \
		$(OUTPUT)/txt_window.o \

all:	 $(OUTPUT)/reboom

clean:
	rm -f *.o *~
	rm -f build/*

$(OUTPUT)/reboom:	$(OBJS) $(OUTPUT)/i_main.o
	$(CC) $(CFLAGS) $(OBJS) $(OUTPUT)/i_main.o \
	-o $(OUTPUT)/reboom $(LDFLAGS)

$(OUTPUT)/%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@
