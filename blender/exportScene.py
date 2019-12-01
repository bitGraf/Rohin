# Mapping
# x = b.x
# y = b.z
# z = -b.y

bl_info = {
    "name": "Rohin Export Scene",
    "blender": (2, 80, 0),
    "category": "Object",
}

import bpy
from bpy import data as D
import math
from mathutils import *


class RohinLevelExport(bpy.types.Operator):
    """My Level Export Script"""      # Use this as a tooltip for menu items and buttons.
    bl_idname = "object.scene_export"        # Unique identifier for buttons and menu items to reference.
    bl_label = "Export to .scene"         # Display name in the interface.
    bl_options = {'REGISTER', 'UNDO'}  # Enable undo for the operator.

    def execute(self, context):        # execute() is called when running the operator.
        
        blend_file = bpy.path.basename(bpy.context.blend_data.filepath)
        sceneName = blend_file.split('.')[0]
        filename = 'G:/run_tree/Data/outFile.Scene'
        
        with open(filename, 'w') as f:
            f.write('SCENE ')
            self.writeStr(f, sceneName)
            f.write('\n')
            
            f.write('RESOURCE ')
            self.writeStr(f, 'Data/Models/%s.glb' % (sceneName))
            f.write('\n')
            
            f.write('SKYBOX HDR "carpentry_shop_02_4k.hdr"\n')
    
            # Print all obj names
            for obj in D.collections['Volumes'].objects:
                self.writeVolumeEnt(f, obj)
                
            for obj in D.collections['Lights'].objects:
                light = obj.data
                
                if light.type == 'SUN':
                    self.writeDirLight(f, obj, light)
                if light.type == 'POINT':
                    self.writePointLight(f, obj, light)
                if light.type == 'SPOT':
                    self.writeSpotLight(f, obj, light)
                    
            for obj in D.collections['Cameras'].objects:
                cam = obj.data
                
                self.writeCamera(f, obj, cam)
            
            for obj in D.collections['Static'].objects:
                mesh = obj.data

                self.writeMesh(f, 'RENDERABLE', obj, mesh.copy())



        return {'FINISHED'}            # Lets Blender know the operator finished successfully.
    
    def writeMesh(self, fid, type, obj, mesh):
        fid.write('ENTITY %s ' % (type))
        #Name
        self.writeStr(fid, obj.name_full)

        #Position
        self.writeVec3(fid, obj.location)

        #YawPitchRoll
        conv = Matrix((
        (1, 0, 0, 0),
        (0, 0, 1, 0),
        (0, -1, 0, 0),
        (0, 0, 0, 1)))
        
        wm = obj.matrix_world
        
        mat = conv @ wm
        forward = Vector((-mat[0][2],-mat[1][2],-mat[2][2])).normalized()
        right   = Vector((mat[0][0],mat[1][0],mat[2][0])).normalized()
        up      = Vector((mat[0][1],mat[1][1],mat[2][1])).normalized()
        
        YawPitchRoll = self.calcYawPitchRoll(forward, right, up)
        
        self.writeVec3_nomod(fid, YawPitchRoll)

        #Scale
        self.writeVec3(fid, obj.scale)

        #Parent
        self.writeStr(fid, '')
        
        #MeshName
        self.writeStr(fid, obj.name_full)
        
        #Material
        self.writeStr(fid, mesh.materials[0].name_full)
        
        #Mesh_Pos
        self.writeVec3(fid, [0,0,0])
        
        #Mesh_YPR
        self.writeVec3_nomod(fid, [0,0,0])
        
        #Mesh_Scale
        self.writeVec3_nomod(fid, [1,1,1])
        
        #cull
        fid.write('"%.2f" ' % (0.25))
        
        fid.write('\n')

    def writeCamera(self, fid, obj, cam):
        fid.write('ENTITY CAMERA ')
        #Name
        self.writeStr(fid, obj.name_full)

        #Position
        self.writeVec3(fid, obj.location)

        #YawPitchRoll
        conv = Matrix((
        (1, 0, 0, 0),
        (0, 0, 1, 0),
        (0, -1, 0, 0),
        (0, 0, 0, 1)))
        
        wm = obj.matrix_world
        
        mat = conv @ wm
        forward = Vector((-mat[0][2],-mat[1][2],-mat[2][2]))
        right   = Vector((mat[0][0],mat[1][0],mat[2][0]))
        up      = Vector((mat[0][1],mat[1][1],mat[2][1]))
        
        YawPitchRoll = self.calcYawPitchRoll(forward, right, up)
        
        self.writeVec3_nomod(fid, YawPitchRoll)

        #Scale
        self.writeVec3(fid, obj.scale)

        #Parent
        self.writeStr(fid, '')
        
        #fov
        fid.write('"%.2f" ' % (cam.angle * 180 / math.pi))
        #near
        fid.write('"%.2f" ' % (cam.clip_start))
        #far
        fid.write('"%.2f" ' % (cam.clip_end))
        
        fid.write('\n')
    
    def calcYawPitchRoll(self, forward, right, up):
        #YawPitchRoll.y = asin(forward.y) * r2d;
        #YawPitchRoll.x = -atan2(forward.z, forward.x) * r2d;
        #YawPitchRoll.z = -atan2(right.y, up.y) * r2d;
        r2d = 180 / math.pi
        pitch = math.asin(forward.y) * r2d
        yaw = -math.atan2(forward.z, forward.x) * r2d
        roll = -math.atan2(right.y, up.y) * r2d
        
        return [yaw, pitch, roll]
    
    def writeDirLight(self, fid, obj, light):
        fid.write('ENTITY DIR ')
        
        self.writeVec4(fid, light.color)
        fid.write('"%.2f" ' % (light.energy)) #strength
        fid.write('"%.2f" ' % (obj.location.length))

        mat = obj.matrix_world
        z = [-mat[0][2],-mat[1][2],-mat[2][2]]
        self.writeVec3(fid, z)
        
        fid.write('\n')
        
        
    def writeSpotLight(self, fid, obj, light):
        fid.write('ENTITY SPOT ')
        
        self.writeVec4(fid, light.color)
        fid.write('"%.2f" ' % (light.energy)) #strength
        self.writeVec3(fid, obj.location)
        
        fid.write('\n')
        
    def writePointLight(self, fid, obj, light):
        fid.write('ENTITY POINT ')
        
        self.writeVec4(fid, light.color)
        fid.write('"%.2f" ' % (light.energy)) #strength

        mat = obj.matrix_world
        z = [-mat[0][2],-mat[1][2],-mat[2][2]]
        self.writeVec3(fid, z)
        
        fid.write('\n')
    
    def writeVolumeEnt(self, fid, obj):
        name = obj.name_full
        
        name = obj.name_full
        pos = obj.location
        b_min = self.getPropVec3(obj, 'min')
        b_max = self.getPropVec3(obj, 'max')
        
        tmp = b_min[1]
        b_min = [b_min[0], b_max[1], b_min[2]]
        b_max = [b_max[0], tmp,      b_max[2]]
        
        targ = 'YaBoy'
                
        fid.write('%s %s ' % ('ENTITY', 'VOLUME'))
        
        self.writeStr(fid, name) #write entity name
        self.writeVec3(fid, pos) #write entity position
        self.writeVec3(fid, b_min) #write bounding box min
        self.writeVec3(fid, b_max) #write bounding box max
        self.writeStr(fid, targ) #write target name
        
        fid.write('\n')
        
    def writeStr(self, fid, str2write):
        fid.write('\"')
        fid.write(str2write)
        fid.write('\" ')
    
    def writeVec3(self, fid, vec):
        fid.write('\"')
        fid.write('%.2f %.2f %.2f' % (vec[0], vec[2], -vec[1]))
        fid.write('\" ')
        
    def writeVec3_nomod(self, fid, vec):
        fid.write('\"')
        fid.write('%.2f %.2f %.2f' % (vec[0], vec[1], vec[2]))
        fid.write('\" ')
        
    def writeVec4(self, fid, vec):
        fid.write('\"')
        fid.write('%.2f %.2f %.2f % .2f' % (vec[0], vec[1], vec[2], 0))
        fid.write('\" ')
    
    def getProp(self, obj, propName):
        if obj.get(propName) is not None:
            return obj[propName]
        else:
            return 0
    
    def getPropVec3(self, obj, propName):
        x = self.getProp(obj, propName+('_x'))
        y = self.getProp(obj, propName+('_y'))
        z = self.getProp(obj, propName+('_z'))
        
        return [x,y,z]

def register():
    bpy.utils.register_class(RohinLevelExport)


def unregister():
    bpy.utils.unregister_class(RohinLevelExport)


# This allows you to run the script directly from Blender's Text editor
# to test the add-on without having to install it.
if __name__ == "__main__":
    register()