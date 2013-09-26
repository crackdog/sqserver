CC = gcc 
CFLAGS = #-O2 #-Wall -Wextra
PROG = sqserver
SRC = main.c daemon.c encryption.c param.c server.c log.c
HDR = sqserver.h daemon.h encryption.h param.h server.h log.h #$(SRC:%.c=%.h)
OBJ = $(SRC:%.c=%.o)
GIT = $(SRC) $(HDR) Makefile info.txt README.md runscript.sh
DEP = .dep.mk
LOGDIR = logs/


all: $(PROG)

$(PROG): $(OBJ) $(DEP)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $<
	
dep: $(DEP)
	
.dep.mk: $(SRC)
	$(CC) -MM $(SRC) > $(DEP)
	
-include $(DEP)

.PHONY: addgit
addgit:
	git add $(GIT)
	
.PHONY: clean
clean:
	rm -rf $(OBJ) $(PROG) $(DEP) $(LOGDIR)
