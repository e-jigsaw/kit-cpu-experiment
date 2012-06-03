project(kue-chip2)
add_library(kuechip SHARED 
cont.c init.c interface.c interpret.c 
mem_file.c memdiff.c reg.c set.c memory.c string.c
)

export(TARGETS kuechip NAMESPACE kuechip_ FILE kuechip-config.cmake)
export(PACKAGE kuechip)