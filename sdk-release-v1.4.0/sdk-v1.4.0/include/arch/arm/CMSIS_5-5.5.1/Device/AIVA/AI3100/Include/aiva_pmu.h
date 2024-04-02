#ifndef __AIVA_PMU_H__
#define __AIVA_PMU_H__

/* Number of counters */
#define ARM_PMU_CNTER_NR 2

enum aiva_pmu_event_type {
    ARM_PMU_EVENT_PMNC_SW_INCR      = 0x00,
    ARM_PMU_EVENT_L1_ICACHE_REFILL  = 0x01,
    ARM_PMU_EVENT_ITLB_REFILL       = 0x02,
    ARM_PMU_EVENT_L1_DCACHE_REFILL  = 0x03,
    ARM_PMU_EVENT_L1_DCACHE_ACCESS  = 0x04,
    ARM_PMU_EVENT_DTLB_REFILL       = 0x05,
    ARM_PMU_EVENT_MEM_READ          = 0x06,
    ARM_PMU_EVENT_MEM_WRITE         = 0x07,
    ARM_PMU_EVENT_INSTR_EXECUTED    = 0x08,
    ARM_PMU_EVENT_EXC_TAKEN         = 0x09,
    ARM_PMU_EVENT_EXC_EXECUTED      = 0x0A,
    ARM_PMU_EVENT_CID_WRITE         = 0x0B,
};

/* Enable bit */
#define ARM_PMU_PMCR_E   (0x01 << 0)
/* Event counter reset */
#define ARM_PMU_PMCR_P   (0x01 << 1)
/* Cycle counter reset */
#define ARM_PMU_PMCR_C   (0x01 << 2)
/* Cycle counter divider */
#define ARM_PMU_PMCR_D   (0x01 << 3)

#ifdef __GNUC__
static inline void aiva_pmu_enable_cnt(int divide64)
{
    unsigned long pmcr;
    unsigned long pmcntenset;

    __asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));
    pmcr |= ARM_PMU_PMCR_E | ARM_PMU_PMCR_P | ARM_PMU_PMCR_C;
    if (divide64)
        pmcr |= ARM_PMU_PMCR_D;
    else
        pmcr &= ~ARM_PMU_PMCR_D;
    __asm volatile ("mcr p15, 0, %0, c9, c12, 0" :: "r"(pmcr));

    /* enable all the counters */
    pmcntenset = ~0;
    __asm volatile ("mcr p15, 0, %0, c9, c12, 1" :: "r"(pmcntenset));
    /* clear overflows(just in case) */
    __asm volatile ("mcr p15, 0, %0, c9, c12, 3" :: "r"(pmcntenset));
}

static inline unsigned long aiva_pmu_get_control(void)
{
    unsigned long pmcr;
    __asm ("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));
    return pmcr;
}

static inline unsigned long aiva_pmu_get_ceid(void)
{
    unsigned long reg;
    /* only PMCEID0 is suppoaivaed, PMCEID1 is RAZ. */
    __asm ("mrc p15, 0, %0, c9, c12, 6" : "=r"(reg));
    return reg;
}

static inline unsigned long aiva_pmu_get_cnten(void)
{
    unsigned long pmcnt;
    __asm ("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcnt));
    return pmcnt;
}

static inline void aiva_pmu_reset_cycle(void)
{
    unsigned long pmcr;

    __asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));
    pmcr |= ARM_PMU_PMCR_C;
    __asm volatile ("mcr p15, 0, %0, c9, c12, 0" :: "r"(pmcr));
    __asm volatile ("isb");
}

static inline void aiva_pmu_reset_event(void)
{
    unsigned long pmcr;

    __asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));
    pmcr |= ARM_PMU_PMCR_P;
    __asm volatile ("mcr p15, 0, %0, c9, c12, 0" :: "r"(pmcr));
    __asm volatile ("isb");
}

static inline unsigned long aiva_pmu_get_cycle(void)
{
    unsigned long cyc;
    __asm volatile ("isb");
    __asm volatile ("mrc  p15, 0, %0, c9, c13, 0" : "=r"(cyc));
    return cyc;
}

static inline void aiva_pmu_select_counter(int idx)
{
    //configASSERT(idx < ARM_PMU_CNTER_NR);

    __asm volatile ("mcr p15, 0, %0, c9, c12, 5" : : "r"(idx));
    /* Linux add an isb here, don't know why here. */
    __asm volatile ("isb");
}

static inline void aiva_pmu_select_event(int idx,
                                      enum aiva_pmu_event_type eve)
{
    //configASSERT(idx < ARM_PMU_CNTER_NR);

    aiva_pmu_select_counter(idx);
    __asm volatile ("mcr p15, 0, %0, c9, c13, 1" : : "r"(eve));
}

static inline unsigned long aiva_pmu_read_counter(int idx)
{
    unsigned long reg;

    aiva_pmu_select_counter(idx);
    __asm volatile ("isb");
    __asm volatile ("mrc p15, 0, %0, c9, c13, 2" : "=r"(reg));
    return reg;
}

static inline unsigned long aiva_pmu_get_ovsr(void)
{
    unsigned long reg;
    __asm volatile ("isb");
    __asm ("mrc  p15, 0, %0, c9, c12, 3" : "=r"(reg));
    return reg;
}

static inline void aiva_pmu_clear_ovsr(unsigned long reg)
{
    __asm ("mcr  p15, 0, %0, c9, c12, 3" : : "r"(reg));
    __asm volatile ("isb");
}

#endif

void aiva_pmu_dump_feature(void);

#define read_cycle() aiva_pmu_get_cycle()

#endif /* end of include guard: __AIVA_PMU_H__ */

