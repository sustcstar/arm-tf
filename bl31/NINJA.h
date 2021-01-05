#include <assert.h>
#include <string.h>

#include <arch.h>
#include <arch_features.h>
#include <arch_helpers.h>
#include <bl31/bl31.h>
#include <bl31/ehf.h>
#include <drivers/arm/gic_common.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <common/runtime_svc.h>
#include <common/interrupt_props.h>
#include <drivers/console.h>
#include <lib/el3_runtime/context_mgmt.h>
#include <lib/pmf/pmf.h>
#include <lib/runtime_instr.h>
#include <lib/spinlock.h>
#include <plat/common/platform.h>
#include <services/std_svc.h>
#include <bl31/interrupt_mgmt.h>

#define READ_SYSREG(dst, sysreg) asm("mrs %0, " #sysreg \
                                     : "=r"(dst));

#define WRITE_SYSREG(src, sysreg) \
    {                             \
        asm("isb");               \
        asm("msr " #sysreg ", %0" \
            :                     \
            : "r"(src));          \
    }

#define READ_REG(dst, reg) asm("mov %0, " #reg \
                                     : "=r"(dst));

#define WRITE_REG(src, reg) \
    {                             \
        asm("mov " #reg ", %0" \
            :                     \
            : "r"(src));          \
    }

int NINJA_INIT();
void PMI_handler();