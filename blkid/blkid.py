import _blkid

class BlkId(object):
    def __init__(self, dev_path):
        self.__dev_path = dev_path
        self.__blkid = None
        self.__opened = False

    def __enter__(self):
        self.open()

        return self
    
    def open(self, mode = "r"):
    	f = open(self.__dev_path, mode = mode)
        self.__blkid = _blkid.BlkId(f)
        self.__opened = True

    def __exit__(self, type, value, tb):
        self.__blkid.close()
        self.__opened = False
        return False

    def do_probe(self, type = "safe"):
    	if not type in ['safe', 'full', 'normal']:
    		raise Exception('Invalid probe option')

    	if type == "safe":
    		self.__blkid.do_safeprobe()
    	elif type == "full":
    		self.__blkid.do_fullprobe()
    	elif type == "normal":
    		self.__blkid.do_probe()

    def probe_results(self):
    	return self.__blkid.probe_results()

    def disable_partitions(self):
    	self.__blkid.disable_partitions()

    def enable_partitions(self):
    	self.__blkid.enable_partitions()

    def set_partitions_flags(self, flags):
    	self.__blkid.set_partitions_flags(flags)

    def enable_superblocks(self):
    	self.__blkid.enable_superblocks()

    def disable_superblocks(self):
    	self.__blkid.disable_superblocks()

    def set_superblocks_flags(self, flags):
    	self.__blkid.set_superblocks_flags(flags)
