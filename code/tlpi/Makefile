

DIRS = lib \


BUILD_DIRS = $(DIRS)



all:
	@ echo $(BUILD_DIRS)
	@ for dir in $(BUILD_DIRS);do (cd $$(dir);${MAKE});done

allgen:
	@ for dir in $(BUILD_DIRS);do (cd $$(dir);${MAKE});done

clean:
	@ for dir in $(BUILD_DIRS);do (cd $$(dir);${MAKE} clean) ;done
