
# ---- Copile options
OS      = LINUX

#C_FLAGS = -g -Wall -ggdb -Wextra
#C_FLAGS = -g -Wall -ggdb -Wunused-but-set-variable
C_FLAGS = -g -Wall -Wunused-but-set-variable
#L_FLAGS = -lncurses -lcrypt -lsocket
L_FLAGS = -lncurses -lcrypt
CC      = gcc

AP_UFU  = ufu
AP_UFUD = ufud

# ---- Compile & link flags
CFLAGS  = -D${OS} ${C_FLAGS}
LDFLAGS = ${L_FLAGS} 

# ---- Source UFU
SRC_UFU  = ufu.c \
           ufu.config.c \
           ufu.cmd.c \
           ufu.com.c \
           ufu.com.func.c \
           ufu.log.c \
           ufu.logo.c \
           ufu.misc.c \
           ufu.mem.c \
           ufu.panel.c \
           ufu.entry.c \
           ufu.sort.c \
           ufu.main.c \
           ufu.help.c \
           ufu.mark.c \
           ufu.ucmd.c \
           ufu.txt.c \
           ufu.hist.c \
           ufu.remote.c \
           ufu.trash.c \
           ufu.id.c \
           ufu.rl.c \
           ufu.ug.c \
           ufu.msg.c \
           ufu.curses.c

# ---- Source UFUd
SRC_UFUD = ufud.c \
           ufud.config.c \
           ufud.log.c \
           ufud.com.c \
           ufud.mem.c \
           ufud.misc.c \
           ufud.entry.c \
           ufud.file.c \
           ufud.version.c \
           ufud.id.c \
           ufud.ip.c \
           ufud.sigchld_handler.c

# ---- Objects UFU
OBJ_UFU  = ufu.o \
           ufu.config.o \
           ufu.cmd.o \
           ufu.com.o \
           ufu.com.func.o \
           ufu.log.o \
           ufu.logo.o \
           ufu.misc.o \
           ufu.mem.o \
           ufu.panel.o \
           ufu.entry.o \
           ufu.sort.o \
           ufu.main.o \
           ufu.help.o \
           ufu.mark.o \
           ufu.ucmd.o \
           ufu.txt.o \
           ufu.hist.o \
           ufu.remote.o \
           ufu.trash.o \
           ufu.id.o \
           ufu.rl.o \
           ufu.ug.o \
           ufu.msg.o \
           ufu.curses.o

# ---- Objects UFUd
OBJ_UFUD = ufud.o \
           ufud.config.o \
           ufud.log.o \
           ufud.com.o \
           ufud.mem.o \
           ufud.misc.o \
           ufud.entry.o \
           ufud.file.o \
           ufud.version.o \
           ufud.id.o \
           ufud.ip.o \
           ufud.sigchld_handler.o

# ---- Compile UFU
ufu:	${OBJ_UFU}
		echo "#define UFU_BUILDDATE \"Build on `date`\"" > ufu.build
		${CC} ${SRC_UFU} ${C_FLAGS} ${L_FLAGS} -o ${AP_UFU}

# ---- Compile UFUd
ufud:	${OBJ_UFUD}
		echo "#define UFUD_BUILDDATE \"Build on `date`\"" > ufud.build
		${CC} ${SRC_UFUD} ${C_FLAGS} ${L_FLAGS} -o ${AP_UFUD}

clean:
		rm -f ${AP_UFU} ${AP_UFUD} ${OBJ_UFU} ${OBJ_UFUD}

all:	ufu ufud

# ---- Dependancies UFU (client)
ufu.o:	    ${AP_UFU}.c ufu.h ufu.p ufu.k
ufu.config.o:           ufu.h ufu.p ufu.k
ufu.cmd.o:              ufu.h ufu.p ufu.k
ufu.com.o:              ufu.h ufu.p ufu.k
ufu.com.func.o:         ufu.h ufu.p ufu.k
ufu.log.o:              ufu.h ufu.p ufu.k
ufu.logo.o:             ufu.h ufu.p ufu.k
ufu.misc.o:             ufu.h ufu.p ufu.k
ufu.mem.o:              ufu.h ufu.p ufu.k
ufu.panel.o:            ufu.h ufu.p ufu.k
ufu.entry.o:            ufu.h ufu.p ufu.k
ufu.sort.o:             ufu.h ufu.p ufu.k
ufu.main.o:             ufu.h ufu.p ufu.k
ufu.help.o:             ufu.h ufu.p ufu.k
ufu.mark.o:             ufu.h ufu.p ufu.k
ufu.ucmd.o:             ufu.h ufu.p ufu.k
ufu.txt.o:              ufu.h ufu.p ufu.k
ufu.hist.o:             ufu.h ufu.p ufu.k
ufu.remote.o:           ufu.h ufu.p ufu.k
ufu.trash.o:            ufu.h ufu.p ufu.k
ufu.id.o:               ufu.h ufu.p ufu.k
ufu.rl.o:               ufu.h ufu.p ufu.k
ufu.ug.o:               ufu.h ufu.p ufu.k
ufu.msg.o:              ufu.h ufu.p ufu.k

# ---- Dependancies UFUd (server)
ufud.o:	   ${AP_UFUD}.c ufud.h ufud.p
ufud.config.o:	        ufud.h ufud.p
ufud.log.o:	            ufud.h ufud.p
ufud.com.o:	            ufud.h ufud.p
ufud.mem.o:	            ufud.h ufud.p
ufud.misc.o:            ufud.h ufud.p
ufud.entry.o:           ufud.h ufud.p
ufud.file.o:            ufud.h ufud.p
ufud.version.o:         ufud.h ufud.p
ufud.id.o:	            ufud.h ufud.p
ufud.ip.o:              ufud.h ufud.p
ufud.sigchld_handler.o: ufud.h ufud.p

