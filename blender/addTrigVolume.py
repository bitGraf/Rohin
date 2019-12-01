bl_info = {
    "name": "New Volume",
    "author": "bitgraf",
    "version": (1, 0),
    "blender": (2, 80, 0),
    "location": "View3D > Add > Mesh > New Trigger Volume",
    "description": "Adds a new Trigger Volume",
    "warning": "",
    "wiki_url": "",
    "category": "Add Mesh",
}


import bpy
from bpy.types import Operator
from bpy.props import FloatVectorProperty
from bpy_extras.object_utils import AddObjectHelper, object_data_add
from mathutils import Vector


def add_object(self, context):
    a = self.b_min
    b = self.b_max

    verts = [
        Vector((a[0], a[1], a[2])),
        Vector((a[0], a[1], b[2])),
        Vector((b[0], a[1], b[2])),
        Vector((b[0], a[1], a[2])),
        
        Vector((a[0], b[1], a[2])),
        Vector((a[0], b[1], b[2])),
        Vector((b[0], b[1], b[2])),
        Vector((b[0], b[1], a[2])),
    ]

    edges = [[0,1], [1,2], [2,3], [3,0], [4,5], [5,6], [6,7], [7,4], [0,4], [1,5], [2,6], [3,7]]
    faces = []#[0, 1, 2, 3],[4, 5, 6, 7],[0, 1, 5, 4]]

    mesh = bpy.data.meshes.new(name="New Trigger Volume")
    mesh.from_pydata(verts, edges, faces)
    # useful for development when the mesh may be invalid.
    # mesh.validate(verbose=True)
    obj = object_data_add(context, mesh, operator=self)
    obj.display_type = 'WIRE'
    
    obj['min_x'] = a[0]
    obj['min_y'] = a[1]
    obj['min_z'] = a[2]
    
    obj['max_x'] = b[0]
    obj['max_y'] = b[1]
    obj['max_z'] = b[2]
    
    obj['Target'] = '_target_name_'


class OBJECT_OT_add_object(Operator, AddObjectHelper):
    """Create a new Trigger Volume"""
    bl_idname = "empty.add_object"
    bl_label = "Add Trigger Volume"
    bl_options = {'REGISTER', 'UNDO'}

    b_min: FloatVectorProperty(
        name="min",
        default=(-1, -1, -1),
        subtype='TRANSLATION',
        description="boundMin",
    )
    
    b_max: FloatVectorProperty(
        name="max",
        default=(1.0, 1.0, 1.0),
        subtype='TRANSLATION',
        description="boundMax",
    )

    def execute(self, context):

        add_object(self, context)

        return {'FINISHED'}


# Registration

def add_object_button(self, context):
    self.layout.operator(
        OBJECT_OT_add_object.bl_idname,
        text="Add Trigger Volume",
        icon='PLUGIN')


# This allows you to right click on a button and link to documentation
def add_object_manual_map():
    url_manual_prefix = "https://docs.blender.org/manual/en/latest/"
    url_manual_mapping = (
        ("bpy.ops.empty.add_object", "scene_layout/object/types.html"),
    )
    return url_manual_prefix, url_manual_mapping


def register():
    bpy.utils.register_class(OBJECT_OT_add_object)
    bpy.utils.register_manual_map(add_object_manual_map)
    bpy.types.VIEW3D_MT_mesh_add.append(add_object_button)


def unregister():
    bpy.utils.unregister_class(OBJECT_OT_add_object)
    bpy.utils.unregister_manual_map(add_object_manual_map)
    bpy.types.VIEW3D_MT_mesh_add.remove(add_object_button)


if __name__ == "__main__":
    register()
