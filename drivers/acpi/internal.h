/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * acpi/internal.h
 * For use by Linux/ACPI infrastructure, not drivers
 *
 * Copyright (c) 2009, Intel Corporation.
 */

#ifndef _ACPI_INTERNAL_H_
#define _ACPI_INTERNAL_H_

#include <linux/idr.h>

extern struct acpi_device *acpi_root;

int early_acpi_osi_init(void);
int acpi_osi_init(void);
acpi_status acpi_os_initialize1(void);
void acpi_scan_init(void);
#ifdef CONFIG_PCI
void acpi_pci_root_init(void);
void acpi_pci_link_init(void);
#else
static inline void acpi_pci_root_init(void) {}
static inline void acpi_pci_link_init(void) {}
#endif
void acpi_processor_init(void);
void acpi_platform_init(void);
void acpi_pnp_init(void);
void acpi_int340x_thermal_init(void);
int acpi_sysfs_init(void);
void acpi_gpe_apply_masked_gpes(void);
void acpi_container_init(void);
void acpi_memory_hotplug_init(void);
#ifdef	CONFIG_ACPI_HOTPLUG_IOAPIC
void pci_ioapic_remove(struct acpi_pci_root *root);
int acpi_ioapic_remove(struct acpi_pci_root *root);
#else
static inline void pci_ioapic_remove(struct acpi_pci_root *root) { return; }
static inline int acpi_ioapic_remove(struct acpi_pci_root *root) { return 0; }
#endif
#ifdef CONFIG_ACPI_DOCK
void register_dock_dependent_device(struct acpi_device *adev,
				    acpi_handle dshandle);
int dock_notify(struct acpi_device *adev, u32 event);
void acpi_dock_add(struct acpi_device *adev);
#else
static inline void register_dock_dependent_device(struct acpi_device *adev,
						  acpi_handle dshandle) {}
static inline int dock_notify(struct acpi_device *adev, u32 event) { return -ENODEV; }
static inline void acpi_dock_add(struct acpi_device *adev) {}
#endif
#ifdef CONFIG_X86
void acpi_cmos_rtc_init(void);
#else
static inline void acpi_cmos_rtc_init(void) {}
#endif
int acpi_rev_override_setup(char *str);

void acpi_sysfs_add_hotplug_profile(struct acpi_hotplug_profile *hotplug,
				    const char *name);
int acpi_scan_add_handler_with_hotplug(struct acpi_scan_handler *handler,
				       const char *hotplug_profile_name);
void acpi_scan_hotplug_enabled(struct acpi_hotplug_profile *hotplug, bool val);

#ifdef CONFIG_DEBUG_FS
extern struct dentry *acpi_debugfs_dir;
void acpi_debugfs_init(void);
#else
static inline void acpi_debugfs_init(void) { return; }
#endif

#if defined(CONFIG_X86) && defined(CONFIG_PCI)
void acpi_lpss_init(void);
#else
static inline void acpi_lpss_init(void) {}
#endif

void acpi_apd_init(void);

acpi_status acpi_hotplug_schedule(struct acpi_device *adev, u32 src);
bool acpi_queue_hotplug_work(struct work_struct *work);
void acpi_device_hotplug(struct acpi_device *adev, u32 src);
bool acpi_scan_is_offline(struct acpi_device *adev, bool uevent);

acpi_status acpi_sysfs_table_handler(u32 event, void *table, void *context);
void acpi_scan_table_notify(void);

int acpi_active_trip_temp(struct acpi_device *adev, int id, int *ret_temp);
int acpi_passive_trip_temp(struct acpi_device *adev, int *ret_temp);
int acpi_hot_trip_temp(struct acpi_device *adev, int *ret_temp);
int acpi_critical_trip_temp(struct acpi_device *adev, int *ret_temp);

#ifdef CONFIG_ARM64
int acpi_arch_thermal_cpufreq_pctg(void);
#else
static inline int acpi_arch_thermal_cpufreq_pctg(void)
{
	return 0;
}
#endif

/* --------------------------------------------------------------------------
                     Device Node Initialization / Removal
   -------------------------------------------------------------------------- */
#define ACPI_STA_DEFAULT (ACPI_STA_DEVICE_PRESENT | ACPI_STA_DEVICE_ENABLED | \
			  ACPI_STA_DEVICE_UI | ACPI_STA_DEVICE_FUNCTIONING)

extern struct list_head acpi_bus_id_list;

struct acpi_device_bus_id {
	const char *bus_id;
	struct ida instance_ida;
	struct list_head node;
};

void acpi_init_device_object(struct acpi_device *device, acpi_handle handle,
			     int type, void (*release)(struct device *));
int acpi_tie_acpi_dev(struct acpi_device *adev);
int acpi_device_add(struct acpi_device *device);
void acpi_device_setup_files(struct acpi_device *dev);
void acpi_device_remove_files(struct acpi_device *dev);
extern const struct attribute_group *acpi_groups[];
void acpi_device_add_finalize(struct acpi_device *device);
void acpi_free_pnp_ids(struct acpi_device_pnp *pnp);
bool acpi_device_is_enabled(const struct acpi_device *adev);
bool acpi_device_is_present(const struct acpi_device *adev);
bool acpi_device_is_battery(struct acpi_device *adev);
bool acpi_device_is_first_physical_node(struct acpi_device *adev,
					const struct device *dev);
int acpi_bus_register_early_device(int type);

/* --------------------------------------------------------------------------
                     Device Matching and Notification
   -------------------------------------------------------------------------- */
const struct acpi_device *acpi_companion_match(const struct device *dev);
int __acpi_device_uevent_modalias(const struct acpi_device *adev,
				  struct kobj_uevent_env *env);

/* --------------------------------------------------------------------------
                                  Power Resource
   -------------------------------------------------------------------------- */
void acpi_power_resources_list_free(struct list_head *list);
int acpi_extract_power_resources(union acpi_object *package, unsigned int start,
				 struct list_head *list);
struct acpi_device *acpi_add_power_resource(acpi_handle handle);
void acpi_power_add_remove_device(struct acpi_device *adev, bool add);
int acpi_power_wakeup_list_init(struct list_head *list, int *system_level);
int acpi_device_sleep_wake(struct acpi_device *dev,
			   int enable, int sleep_state, int dev_state);
int acpi_power_get_inferred_state(struct acpi_device *device, int *state);
int acpi_power_on_resources(struct acpi_device *device, int state);
int acpi_power_transition(struct acpi_device *device, int state);
void acpi_turn_off_unused_power_resources(void);

/* --------------------------------------------------------------------------
                              Device Power Management
   -------------------------------------------------------------------------- */
int acpi_device_get_power(struct acpi_device *device, int *state);
int acpi_wakeup_device_init(void);

/* --------------------------------------------------------------------------
                                  Processor
   -------------------------------------------------------------------------- */
#ifdef CONFIG_ARCH_MIGHT_HAVE_ACPI_PDC
void acpi_early_processor_control_setup(void);
void acpi_early_processor_set_pdc(void);
#ifdef CONFIG_X86
void acpi_proc_quirk_mwait_check(void);
#else
static inline void acpi_proc_quirk_mwait_check(void) {}
#endif
bool processor_physically_present(acpi_handle handle);
#else
static inline void acpi_early_processor_control_setup(void) {}
#endif

#ifdef CONFIG_ACPI_PROCESSOR_CSTATE
void acpi_idle_rescan_dead_smt_siblings(void);
#else
static inline void acpi_idle_rescan_dead_smt_siblings(void) {}
#endif

/* --------------------------------------------------------------------------
                                  Embedded Controller
   -------------------------------------------------------------------------- */

enum acpi_ec_event_state {
	EC_EVENT_READY = 0,	/* Event work can be submitted */
	EC_EVENT_IN_PROGRESS,	/* Event work is pending or being processed */
	EC_EVENT_COMPLETE,	/* Event work processing has completed */
};

struct acpi_ec {
	acpi_handle handle;
	int gpe;
	int irq;
	unsigned long command_addr;
	unsigned long data_addr;
	bool global_lock;
	unsigned long flags;
	unsigned long reference_count;
	struct mutex mutex;
	wait_queue_head_t wait;
	struct list_head list;
	struct transaction *curr;
	spinlock_t lock;
	struct work_struct work;
	unsigned long timestamp;
	enum acpi_ec_event_state event_state;
	unsigned int events_to_process;
	unsigned int events_in_progress;
	unsigned int queries_in_progress;
	bool busy_polling;
	unsigned int polling_guard;
};

extern struct acpi_ec *first_ec;

/* If we find an EC via the ECDT, we need to keep a ptr to its context */
/* External interfaces use first EC only, so remember */
typedef int (*acpi_ec_query_func) (void *data);

#ifdef CONFIG_ACPI_EC

void acpi_ec_init(void);
void acpi_ec_ecdt_probe(void);
void acpi_ec_dsdt_probe(void);
void acpi_ec_block_transactions(void);
void acpi_ec_unblock_transactions(void);
int acpi_ec_add_query_handler(struct acpi_ec *ec, u8 query_bit,
			      acpi_handle handle, acpi_ec_query_func func,
			      void *data);
void acpi_ec_remove_query_handler(struct acpi_ec *ec, u8 query_bit);
void acpi_ec_register_opregions(struct acpi_device *adev);

#ifdef CONFIG_PM_SLEEP
void acpi_ec_flush_work(void);
bool acpi_ec_dispatch_gpe(void);
#endif

#else

static inline void acpi_ec_init(void) {}
static inline void acpi_ec_ecdt_probe(void) {}
static inline void acpi_ec_dsdt_probe(void) {}
static inline void acpi_ec_block_transactions(void) {}
static inline void acpi_ec_unblock_transactions(void) {}
static inline int acpi_ec_add_query_handler(struct acpi_ec *ec, u8 query_bit,
			      acpi_handle handle, acpi_ec_query_func func,
			      void *data)
{
	return -ENXIO;
}
static inline void acpi_ec_remove_query_handler(struct acpi_ec *ec, u8 query_bit) {}
static inline void acpi_ec_register_opregions(struct acpi_device *adev) {}

static inline void acpi_ec_flush_work(void) {}
static inline bool acpi_ec_dispatch_gpe(void)
{
	return false;
}

#endif

/*--------------------------------------------------------------------------
                                  Suspend/Resume
  -------------------------------------------------------------------------- */
#ifdef CONFIG_ACPI_SYSTEM_POWER_STATES_SUPPORT
extern bool acpi_s2idle_wakeup(void);
extern int acpi_sleep_init(void);
#else
static inline bool acpi_s2idle_wakeup(void) { return false; }
static inline int acpi_sleep_init(void) { return -ENXIO; }
#endif

#ifdef CONFIG_ACPI_SLEEP
void acpi_sleep_proc_init(void);
int suspend_nvs_alloc(void);
void suspend_nvs_free(void);
int suspend_nvs_save(void);
void suspend_nvs_restore(void);
#else
static inline void acpi_sleep_proc_init(void) {}
static inline int suspend_nvs_alloc(void) { return 0; }
static inline void suspend_nvs_free(void) {}
static inline int suspend_nvs_save(void) { return 0; }
static inline void suspend_nvs_restore(void) {}
#endif

#ifdef CONFIG_X86
bool force_storage_d3(void);
#else
static inline bool force_storage_d3(void)
{
	return false;
}
#endif

/*--------------------------------------------------------------------------
				Device properties
  -------------------------------------------------------------------------- */
#define ACPI_DT_NAMESPACE_HID	"PRP0001"

void acpi_init_properties(struct acpi_device *adev);
void acpi_free_properties(struct acpi_device *adev);

#ifdef CONFIG_X86
void acpi_extract_apple_properties(struct acpi_device *adev);
#else
static inline void acpi_extract_apple_properties(struct acpi_device *adev) {}
#endif

/*--------------------------------------------------------------------------
				Watchdog
  -------------------------------------------------------------------------- */

#ifdef CONFIG_ACPI_WATCHDOG
void acpi_watchdog_init(void);
#else
static inline void acpi_watchdog_init(void) {}
#endif

#ifdef CONFIG_ACPI_LPIT
void acpi_init_lpit(void);
#else
static inline void acpi_init_lpit(void) { }
#endif

/*--------------------------------------------------------------------------
		ACPI _CRS CSI-2 and MIPI DisCo for Imaging
  -------------------------------------------------------------------------- */

void acpi_mipi_check_crs_csi2(acpi_handle handle);
void acpi_mipi_scan_crs_csi2(void);
void acpi_mipi_init_crs_csi2_swnodes(void);
void acpi_mipi_crs_csi2_cleanup(void);
#ifdef CONFIG_X86
bool acpi_graph_ignore_port(acpi_handle handle);
#else
static inline bool acpi_graph_ignore_port(acpi_handle handle) { return false; }
#endif

#endif /* _ACPI_INTERNAL_H_ */
