CFLAGS = -std=gnu11 -Wall -Wno-unused-function
LDLIBS = -lpci -lpci
OBJ = detect.o \
	platforms/skl_kbl.o \
	platforms/cfl.o \
	platforms/adl_p.o \
        platforms/adl_n.o \
	platforms/adl_s.o \
	platforms/rpl_p.o \
	platforms/rpl_s.o \
	platforms/cnp_lp.o \
	platforms/cml_u.o \
	platforms/cml_dt.o \
	platforms/tgl.o \
	platforms/arl_s.o \
	platforms/mtl.o \
	platforms/pre_skl.o \
	platforms/ehl.o \
	platforms/jsl.o \
	platforms/glk.o

.PHONY: clean

tpm-gpio-detect: $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -f tpm-gpio-detect $(OBJ)
