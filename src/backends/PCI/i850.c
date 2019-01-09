/*
 * $id:$
 * 
 * Originally a kernel patch by Drew Hess <dhess@CS.Stanford.EDU>
 * ported to userspace by the Powertweak team.
 *
 */

#include <powertweak.h>
#include <pci.h>
#include <pciutils.h>

#define PCI_DEVICE_ID_INTEL_82850_MC1	0x2530

/*
 * The BIOS on some Intel i850 motherboards (notably the D850GB) 
 * may configure pool C RDRAM devices to run in nap mode due to thermal 
 * concerns. We set up pool C to run in standby mode for better
 * performance and set pools A and B to their max sizes.  Then we
 * write (but don't commit) a "safe" value in case an over-temperature
 * condition occurs.
 */

static void quirk_tehama (struct pci_dev *dev)
{
	u8 rdps;

	/*
	printk(KERN_INFO "i850: Configuring pool C RDRAM devices to run in standby mode\n");
	pci_read_config_byte(dev, 0x88, &rdps);
	printk(KERN_INFO "i850: BIOS set RDPS to 0x%02x\n", rdps);
	*/

	/* 
	 * i850 docs are unclear about how to unlock/set RDPS and
	 * then initialize the pool logic, so do it one step at a time.
	 * Wait for poolinit bit to clear for indication that pools
	 * are running in new mode.
	 */
	pci_write_byte(dev, 0x88, 0x0);
	pci_write_byte(dev, 0x88, 0x0f);
	pci_write_byte(dev, 0x88, 0x2f);
	do
		pci_read_byte(dev, 0x88, &rdps);
	while (rdps & 0x20);

	/* prep for nap mode in case of over-temperature condition */
	pci_write_byte(dev, 0x88, 0x19);
}


static int add_quirk_tehama (struct pci_dev *dev)
{
	struct tweak *tweak;
	struct private_PCI_data *private;
	char *Tab = "Tweaks";

	tweak = alloc_PCI_tweak (dev, TYPE_BOOLEAN);
	if (tweak == NULL)
		return FALSE;

	 tweak->ConfigName=strdup ("8086:2530_STANDBY_RDRAM");
	 tweak->WidgetText = strdup("RDRAM standby mode");
	 tweak->Description = strdup ("Configures pool C RDRAM devices to run in standby mode");
	 private = tweak->PrivateData;
//	 set_value_int (private->value, pci_read_byte (dev, 0x88));
	 private->Mask = 0xFF;
	 private->device = dev;
	 private->base_address = 0x88;
	 AddTo_PCI_tree (tweak, dev, FrameName, Tab, NULL);
	 return TRUE;
}


int InitPlugin (int pt_unused showinfo)
{
	struct pci_dev *dev;
	int retval=FALSE;

	if (have_core_plugin("libpcilib.so") == FALSE)
		return FALSE;

	dev = pacc->devices;

	while (dev) {
		if ((dev->vendor_id == 0x8086) && (dev->device_id == PCI_DEVICE_ID_INTEL_82850_MC1))
			return add_quirk_tehama(dev);
		dev = dev->next;
	}

	return retval;
}
