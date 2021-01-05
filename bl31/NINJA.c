#include "NINJA.h"
#define JUNO_IRQ_PMI 34
#define JUNO_IRQ_PMI_2 38
#define JUNO_IRQ_PMI_3 50
#define JUNO_IRQ_PMI_4 54
#define JUNO_IRQ_PMI_5 58
#define JUNO_IRQ_PMI_6 62

spinlock_t console_lock;
#define MY_ERROR(args)        \
    spin_lock(&console_lock); \
    ERROR(args);              \
    spin_unlock(&console_lock)

void check_current_EL()
{
    uint64_t el_reg = 0;
    uint64_t el = 0x3f;
    READ_SYSREG(el_reg, CurrentEL);
    el = (el_reg >> 1) & 0b11;
    ERROR("CurrentEL=%llu\n", el);
}

uint32_t ninja_interrupt_handler()
{
    uint32_t id;
    READ_REG(id, x9);

    spin_lock(&console_lock);
    ERROR("[Ninja]: Detected Interrupt, id=%u\n", id);
    spin_unlock(&console_lock);
    if (id == 34 ||
        id == 38 ||
        id == 50 ||
        id == 54 ||
        id == 58 ||
        id == 62)
    {
        MY_ERROR("[Ninja]: Detected PMI\n");
        uint32_t inf = 0xffffffff;
        WRITE_SYSREG(inf, pmevcntr0_el0);
        MY_ERROR("[Ninja]: Reset pmevcntr0_el0 to 0xFFFFFFFF\n");
        uint32_t zero = 101;
        WRITE_REG(zero, x9);
    }
    return 0;
}

void enable_PMI()
{
    uint32_t t = 0;
    READ_SYSREG(t, PMINTENSET_EL1);
    t = 0xffffffff;
    WRITE_SYSREG(t, PMINTENSET_EL1);
    READ_SYSREG(t, PMINTENSET_EL1);
    ERROR("[NINJA]: change PMINTENSET_EL1 to %u\n", t);
}

int NINJA_INIT()
{
    MY_ERROR("NINJA INIT BEGIN\n");
    check_current_EL();
    enable_PMI();
    unsigned int flags = 0;
    set_interrupt_rm_flag(flags, NON_SECURE);
    set_interrupt_rm_flag(flags, SECURE);

    MY_ERROR("NINJA INIT END\n");
    return 0;
}