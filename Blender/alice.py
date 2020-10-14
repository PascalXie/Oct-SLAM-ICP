import bpy
from easybpy import *

PI = 3.1415926

if __name__ == '__main__':
	print('hello')

	verts = []
	if object_exists('Suzanne'):
		obj   = get_object('Suzanne')
		verts = get_vertices('Suzanne')

	f = open('/Users/pascal/动画游戏科技-2020/10月-科技-机器视觉-ICP/Blender/vertices_frameOriginal.txt','w')

	for vert in verts:
		x = vert.co[0]
		y = vert.co[1]
		z = vert.co[2]
		print(vert.co, x,y,z)

		line = str(x) + ' ' + str(y) + ' ' + str(z) + '\n' 
		f.write(line)

	f.close()

	# get rotation and translation of the camera
	f = open('/Users/pascal/动画游戏科技-2020/10月-科技-机器视觉-ICP/Blender/rotation_angle_axis_translation.txt','w')

	angle = 0.
	axis = [1,1,1]
	loc  = [0,0,0]

	for frameID in range(20):
		bpy.context.scene.frame_set(frameID)
		if object_exists('Camera'):
			Cam = get_object('Camera')
			angle = Cam.rotation_axis_angle[0] /PI*180.
			axis[0] = Cam.rotation_axis_angle[1]
			axis[1] = Cam.rotation_axis_angle[2]
			axis[2] = Cam.rotation_axis_angle[3]
			loc[0]  = Cam.location[0]
			loc[1]  = Cam.location[1]
			loc[2]  = Cam.location[2]
			print(frameID,angle, axis)
			line = str(frameID) + ' ' + str(angle) + ' ' + str(axis[0]) + ' ' + str(axis[1]) + ' ' + str(axis[2]) + ' ' + str(loc[0]) + ' ' + str(loc[1]) + ' ' + str(loc[2]) + '\n' 
		f.write(line)

	f.close()
