.PHONY: clean
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
ifeq ($(MINGW), 1)
DEP = $(OBJ:.o=.d)
endif
CC = gcc
ifeq ($(MAC_OS), 1)
CFLAGS = -std=c17 -pedantic -Wall -DUNIX -arch x86_64 -arch arm64
LDFLAGS = -lSDL2-2.0.0 -lSDL2_mixer-2.0.0 -lSDL2_net-2.0.0 -lm
endif
ifeq ($(MINGW), 1)
CFLAGS = -std=c2x -pedantic -Wall -DWINDOWS -g -DINSTRUMENTED -DRANGECHECK
LDFLAGS = -lmingw32 -lgdi32 -lkernel32 -luser32 -mwindows -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_net
else
CFLAGS = -std=c2x -pedantic -Wall -DUNIX -g -DINSTRUMENTED -DRANGECHECK
LDFLAGS = -lSDL2 -lSDL2_mixer -lSDL2_net -lm
endif

ifeq ($(MINGW), 1)
-include $(DEP)
endif

reboom: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	
ifeq ($(MINGW), 1)
%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

endif

ifeq ($(MINGW), 1)
clean:
	del $(OBJ) $(DEP) reboom
else
clean:
	rm -f $(OBJ) reboom
endif