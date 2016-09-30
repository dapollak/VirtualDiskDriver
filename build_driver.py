import os
import subprocess
import glob
import sys
import re
import win32api

WINDDK_PATH = r'C:\WinDDK\7600.16385.1'

class BuildError(Exception):
	pass

class Sources_File(object):


	FILE_NAME = "sources"

	def __init__(self, target_dir, list_of_src_files, target_binary_name, target_type):
		self._target_dir = target_dir
		self._list_of_src_files = list_of_src_files
		self._target_binary_name = target_binary_name
		self._target_type = target_type


	def create_file(self):
		with open(os.path.join(self._target_dir, self.FILE_NAME), 'wb') as f:
			f.write('SOURCES={0}\r\n'.format(' \\\r\n'.join(self._list_of_src_files)))
			f.write('TARGETNAME={0}\r\n'.format(self._target_binary_name))
			f.write('TARGETTYPE={0}\r\n'.format(self._target_type))
			f.write('TARGETPATH=.\r\n')

def get_short_name(name):
	return win32api.GetShortPathName(name)

def get_sources_list(dir_path):
	return map(lambda x: os.path.basename(x), glob.glob(os.path.join(dir_path, '*.c')))

def move_sys_pdb_to_final_dir(final_dir):
	pass

def build_driver(dir_path, chk, arch, os_ver, driver_name):
	s = Sources_File(dir_path, get_sources_list(dir_path), driver_name, target_type='DRIVER')
	s.create_file()

	# build the driver
	command = 'cmd.exe /c '
	command += '"{0} {1} {2} {3} {4} & cd {5} & cd {6} & build /g"'.format(os.path.join(WINDDK_PATH, 'bin', 'setenv.bat'),
												WINDDK_PATH, chk, arch, os_ver, get_short_name(os.path.dirname(os.path.abspath(__file__))), dir_path)
	print command
	p = subprocess.Popen(command, stdout=subprocess.PIPE)
	out = p.communicate()[0]

def main():
	if len(sys.argv) != 6:
		print 'Usage Error: build_driver.py target_dir <debug/release> <x86/x64> <WXP/WIN7> driver_name'
		return

	if sys.argv[2].lower() not in ['debug', 'release']:
		print '{0} is not valid parameter'.format(sys.argv[2])
		return

	if sys.argv[3].lower() not in ['x86', 'x64']:
		print '{0} is not valid parameter'.format(sys.argv[3])
		return		

	chk_fre_map = {'debug': 'chk', 'release': 'fre'}
	build_driver(sys.argv[1], chk_fre_map[sys.argv[2].lower()], sys.argv[3], sys.argv[4], sys.argv[5])

if __name__ == '__main__':
	main()

