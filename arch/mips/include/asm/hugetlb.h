/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008, 2009 Cavium Networks, Inc.
 */

#ifndef __ASM_HUGETLB_H
#define __ASM_HUGETLB_H

#include <asm/page.h>

#define __HAVE_ARCH_HUGE_PTEP_GET_AND_CLEAR
static inline pte_t huge_ptep_get_and_clear(struct mm_struct *mm,
					    unsigned long addr, pte_t *ptep,
					    unsigned long sz)
{
	pte_t clear;
	pte_t pte = *ptep;

	pte_val(clear) = (unsigned long)invalid_pte_table;
	set_pte_at(mm, addr, ptep, clear);
	return pte;
}

#define __HAVE_ARCH_HUGE_PTEP_CLEAR_FLUSH
static inline pte_t huge_ptep_clear_flush(struct vm_area_struct *vma,
					  unsigned long addr, pte_t *ptep)
{
	pte_t pte;
	unsigned long sz = huge_page_size(hstate_vma(vma));

	/*
	 * clear the huge pte entry firstly, so that the other smp threads will
	 * not get old pte entry after finishing flush_tlb_page and before
	 * setting new huge pte entry
	 */
	pte = huge_ptep_get_and_clear(vma->vm_mm, addr, ptep, sz);
	flush_tlb_page(vma, addr);
	return pte;
}

#define __HAVE_ARCH_HUGE_PTE_NONE
static inline int huge_pte_none(pte_t pte)
{
	unsigned long val = pte_val(pte) & ~_PAGE_GLOBAL;
	return !val || (val == (unsigned long)invalid_pte_table);
}

#define __HAVE_ARCH_HUGE_PTEP_SET_ACCESS_FLAGS
static inline int huge_ptep_set_access_flags(struct vm_area_struct *vma,
					     unsigned long addr,
					     pte_t *ptep, pte_t pte,
					     int dirty)
{
	int changed = !pte_same(*ptep, pte);

	if (changed) {
		set_pte_at(vma->vm_mm, addr, ptep, pte);
		/*
		 * There could be some standard sized pages in there,
		 * get them all.
		 */
		flush_tlb_range(vma, addr, addr + HPAGE_SIZE);
	}
	return changed;
}

#include <asm-generic/hugetlb.h>

#endif /* __ASM_HUGETLB_H */
