# Default target
for = linux

debugMaxLevel = 3
debugFlag = -g -DDEBUG0 -DDEBUG1 -DDEBUG2 -DDEBUG3

include targets/$(for)/config

all: 

$(binariesDirectory):
	mkdir -p $@

$(librariesDirectory):
	mkdir -p $@

.PRECIOUS: $(binariesDirectory)/% $(librariesDirectory)/%.o
.PHONY: parseLibraries run clean cleanlib all libraries

%: parseLibraries_% $(binariesDirectory)/%
	$(eval application = $<)

$(binariesDirectory)/%: $(librariesDirectory)/%.o $(binariesDirectory)
	$(linker) -o $@$(applicationSuffix) $< $(libraries) $(linkerOptions)

$(librariesDirectory)/%.o: $(applicationDirectory)/%.cpp libraries
	$(compiler) $(compilerOptions) $< -o $@

parseLibraries_%: get-dependencies.sh
	$(eval libraries := $(shell app="$@" ; app="$${app#parseLibraries_}" ; ./get-dependencies.sh $(applicationDirectory)/$${app}.cpp | sed 's|^.*$$|$(librariesDirectory)/\0|g'))

libraries: $(librariesDirectory)
# $(libraries) is always empty... bug
	@( for source in `cd $(sourcesDirectory); find . -name '*.cpp' -type f`; \
	do \
		if [ -z "$(libraries)" ] ; then \
			mkdir -p $(librariesDirectory)/`dirname $$source` ; \
			if [ ! -e $(librariesDirectory)/$${source%.cpp}.o -o $(sourcesDirectory)/$$source -nt $(librariesDirectory)/$${source%.cpp}.o ] ; then \
				echo "$(compiler) $(compilerOptions) $(sourcesDirectory)/$$source -o $(librariesDirectory)/$${source%.cpp}.o" ; \
				$(compiler) $(compilerOptions) $(sourcesDirectory)/$$source -o $(librariesDirectory)/$${source%.cpp}.o ; \
				if [ $$? -eq 1 ] ; then \
					exit 1 ; \
				fi ; \
			fi ; \
		fi ; \
	done )

run: $(application)
	@( if [ ! -z "$(application)" ] ; then \
		$(application) ; \
	fi )

debug: 
	@( make --no-print-directory debug0 )

debug%:
	@( target="$@"; \
	target=$${target#debug}; \
	for level in `seq $$target $(debugMaxLevel)`; \
	do \
		flags="-DDEBUG$$level $$flags"; \
	done ; \
	make --no-print-directory run debugFlag="$$flags -g" )

cleanlib:
	rm -rf $(librariesDirectory)/*
	
clean:
	find . -name '*~' | xargs rm -f
	rm -f $(binariesDirectory)/*

