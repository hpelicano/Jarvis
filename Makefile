TARGET  = $(BIN_DIR)/jarvisd.out
OBJ_DIR = ../obj
BIN_DIR = ../bin
INC_DIR = ../inc
MOD_DIR = ../mod
SRC_DIR = .

#OBJS_MOD = $(MOD_DIR)/libaudit.so $(MOD_DIR)/libenfrm.so
OBJS = $(OBJ_DIR)/Servidor2.o #$(OBJS_MOD)
CFLAGS = -Wall -g -DTEST --debug -I$(INC_DIR) -pthread 

#-rdynamic -laudit -lenfrm -L"../mod"
$(TARGET) : $(OBJS)
	mkdir -p $(BIN_DIR)
	gcc $(CFLAGS) $(OBJS) -o $(TARGET)  
	
-include $(OBJ_DIR)/*.d

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c -MD $(CFLAGS) $< -o $@

.PHONY: clean
clean :
	rm -f $(OBJ_DIR)/*.o $(TARGET)
