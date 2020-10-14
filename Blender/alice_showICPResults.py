import bpy
from easybpy import *

PI = 3.1415926

def ReadFile(path, fileName):
	print("Data File Path : {}".format(path))
	print("File Name : {}".format(fileName))

	# read
	f = open(path+fileName)
	lines = f.readlines()

	# rotation matrix: angle axis
	angles = [] # rad
	axs	= []
	ays	= []
	azs	= []

	#a translation
	xs = []
	ys = []
	zs = []

	for line in lines:
		line = line.strip().split()
		angles.append(float(line[0]))
		axs.append(float(line[1]))
		ays.append(float(line[2]))
		azs.append(float(line[3]))
		xs.append(float(line[4]))
		ys.append(float(line[5]))
		zs.append(float(line[6]))

	return angles, axs, ays, azs, xs, ys, zs


if __name__ == '__main__':
	print('hello')

	path = '/Users/pascal/动画游戏科技-2020/10月-科技-机器视觉-ICP/Blender/'
	fileName = 'Transformation_AllFrames.txt'
	angles, axs, ays, azs, xs, ys, zs = ReadFile(path, fileName)


	CamICP = ''
	if object_exists('CameraICP'):
		CamICP = get_object('CameraICP')
	else:
		print('CameraICP is not found!')

	for i in range(len(angles)):
		ang = angles[i]
		ax  = axs[i]
		ay  = ays[i]
		az  = azs[i]
		x  = xs[i]
		y  = ys[i]
		z  = zs[i]

		frameID = i

		# set frameID
		bpy.context.scene.frame_set(frameID)

		# set rotation matrix
		CamICP.rotation_axis_angle[0] = ang
		CamICP.rotation_axis_angle[1] = ax
		CamICP.rotation_axis_angle[2] = ay
		CamICP.rotation_axis_angle[3] = az
		CamICP.location[0] = x
		CamICP.location[1] = y
		CamICP.location[2] = z

		bpy.ops.anim.keyframe_insert_menu(type='BUILTIN_KSI_LocRot')

	
