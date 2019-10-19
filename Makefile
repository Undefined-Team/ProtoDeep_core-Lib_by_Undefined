FLAGS             =     -g -Wall -Wextra -Werror -O3
CC                =     gcc

SRC_PATH          =     ./res/src/
BIN_PATH          =     ./res/bin/
INCLUDE_PATH      =     ./res/include/

BLACKLIST         =

SRC_BLACKLIST     =     $(addprefix $(SRC_PATH), $(BLACKLIST))

INCLUDE           =     -I$(INCLUDE_PATH)
SRCS              =     $(wildcard $(SRC_PATH)*.c)
SRC               =     $(filter-out $(SRC_BLACKLIST), $(SRCS))
BINS              =     $(SRC:.c=.o)
BIN               =     $(addprefix $(BIN_PATH), $(addprefix $(LIBNAME)_, $(notdir $(BINS))))

COMPILE           =     $(CC) $(FLAGS) $(INCLUDE)

.PHONY            =     all extract clean fclean re

# Colors
G                 =     \33[1;32m
N                 =     \033[0m
# Colors

all: libud_$(LIBNAME).a
	
libud_$(LIBNAME).a: $(BIN) extract
ifdef ARNAME
	@$(eval LIB_OBJ=$(shell echo *.o))
endif
	@ar rc libud_${LIBNAME}.a ${BIN} ${LIB_OBJ}
	@echo "\t$(G)Success: Static library [ libud_${LIBNAME}.a ] compiled.$(N)"
	@ranlib libud_${LIBNAME}.a
	@echo "\t$(G)Success: Static library [ libud_${LIBNAME}.a ] indexed.$(N)"
ifdef ARNAME
	@rm *.o
endif

extract:
ifdef ARNAME
	@for dep in $(ARNAME); do \
		ar x $${dep}; \
	done
endif

$(BIN_PATH)$(LIBNAME)_%.o: $(SRC_PATH)%.c $(DEPHEADER)
	@mkdir -p $(BIN_PATH) || true
	@echo -n "\t$(G)Success: "
	$(COMPILE) $< -o $@ -c
	@echo -n "$(N)"

clean:
	@rm -f $(BIN_PATH)/*.o

fclean: clean
	@rm -f ${wildcard *.a}

re: fclean all