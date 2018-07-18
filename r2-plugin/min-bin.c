#include <r_types.h>
#include <r_util.h>
#include <r_lib.h>
#include <r_bin.h>

static ut32 entrypoint = 0;

// Function from nes binr
static RList* entries(RBinFile *bf) {
	RList *ret;
	RBinAddr *ptr = NULL;
	if (!(ret = r_list_new ())) {
		return NULL;
	}
	if (!(ptr = R_NEW0 (RBinAddr))) {
		return ret;
	}
	ptr->paddr = entrypoint;
	ptr->vaddr = entrypoint;
	r_list_append (ret, ptr);
	return ret;
}

static RBinInfo* info(RBinFile *bf)
{
    RBinInfo *ret = NULL;

    if(!(ret = R_NEW0(RBinInfo)) {
        return NULL;
    }

    ret->type = strdup("min bytecode");
    ret->machine = strdup("min-vm");
    ret->os = strdup("min-vm");
    ret->arch = strdup("min");
    ret->bits = 32;
    ret->has_va = false;

    return ret;
}

static bool check_bytes(const ut8 *buf, ut64 length)
{
    // length 6: [2b: Magic][4b: Entrypoint][4b: File size]
    if(length < 10 || buf[0] != 'M' || buf[1] != 'X')
        return false;
    
    entrypoint = *(ut32 *)(buf + 2);

    return entrypoint < length;
}

RBinPlugin r_bin_plugin_lua53 = {
	.name = "min",
	.desc = "min bytecode binary loader",
	.license = "none",
	.sections = &sections,
	// .check = &check,
	.check_bytes = &check_bytes,
	.info = &info,
	.entries = &entries,
};

#ifndef CORELIB
RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_BIN,
	.data = &r_bin_plugin_lua53,
	.version = R2_VERSION
};
#endif