import subprocess as sp
import os

output_dir = "capture_output/"
pattern_dir = "pattern/"

files = os.listdir(pattern_dir)


for f in files:
	print 'Projection:', f
	sp.Popen(['eog',pattern_dir + f,'-f'])
	sp.check_call(['gphoto2', '--capture-image-and-download', '--filename', str(output_dir+f) ])
	sp.check_call(['killall','-9','eog'])
		
print "Done!"