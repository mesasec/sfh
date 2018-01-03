#opt: OPTFLAGS = -O2
#export OPTFLAGS

.PHONY: all clean opt

all:
	cd src && $(MAKE)
	cd tools && $(MAKE)
clean:
	cd src && $(MAKE) clean
	cd tools && $(MAKE) clean
			 
opt:
	$(MAKE) all
