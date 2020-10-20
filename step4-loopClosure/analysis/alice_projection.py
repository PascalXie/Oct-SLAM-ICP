import numpy as np
import matplotlib.pyplot as plt

def ReadFile(path, fileName):
	print("Data File Path : {}".format(path))
	print("File Name : {}".format(fileName))

	# read
	f = open(path+fileName)
	lines = f.readlines()

	xs = []
	ys = []
	zs = []

	for line in lines:
		line = line.strip().split()
		xs.append(float(line[1])*-1.)
		ys.append(float(line[2])*-1.)
		zs.append(float(line[3]))

	return xs, ys, zs


if __name__ == '__main__':
	print("hello")

	path = './'

	#fileName = 'PointCloud_FrameID_0.txt'
	#xs, ys, zs = ReadFile(path, fileName)

	#
	# plot
	#
	for i in range(100):
		fileName = 'PointCloud_FrameID_'+str(i)+'.txt'

		xs, ys, zs = ReadFile(path, fileName)

		# plot
		fig = plt.figure(figsize=(8,8))
		plt.plot(xs, ys, label = 'frame 0', lw=2, ls='', marker='s', markersize=2)

		plt.xlim(-6,6)
		plt.ylim(-6,6)
		#plt.ylim((6,-6))
		plt.xlabel('X',fontdict={'family' : 'Times New Roman', 'size': 12})
		plt.ylabel('Y',fontdict={'family' : 'Times New Roman', 'size': 12})
		plt.title('Projection of Suzanne')
		plt.legend(frameon=True)

		plt.tight_layout()
		plt.savefig('figure-projectionSuzanne-frame'+str(i)+'.png',dpi=300)
		plt.close('all')

		#plt.show()

