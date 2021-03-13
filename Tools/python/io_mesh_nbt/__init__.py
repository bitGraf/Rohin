bl_info = {
    "name": "Rohin mesh output (nbt)",
    "author": "Andrew Blaskovich",
    "version": (1, 4, 0),
    "blender": (2, 80, 0),
    "location": "File > Import-Export",
    "description": "Export selected object to a .nbt file as a mesh",
    "warning": "",
    "wiki_url": "",
    "support": 'COMMUNITY',
    "category": "Import-Export"}

if "bpy" in locals():
    import importlib
    if "export_nbt" in locals():
        importlib.reload(export_nbt)


import bpy
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from bpy_extras.io_utils import (
        ImportHelper,
        ExportHelper,
        orientation_helper,
        path_reference_mode,
        axis_conversion,
        )

@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportNBT(bpy.types.Operator, ExportHelper):
    """Save a .nbt mesh File"""

    bl_idname = "export_scene.nbt"
    bl_label = 'Export NBT'
    bl_options = {'PRESET'}

    filename_ext = ".nbt"
    filter_glob: StringProperty(
            default="*.nbt;*.mesh",
            options={'HIDDEN'},
            )

    # context group
    use_selection: BoolProperty(
            name="Selection Only",
            description="Export selected objects only",
            default=False,
            )

    # object group
    use_mesh_modifiers: BoolProperty(
            name="Apply Modifiers",
            description="Apply modifiers",
            default=True,
            )
    # Non working in Blender 2.8 currently.
    # ~ use_mesh_modifiers_render: BoolProperty(
            # ~ name="Use Modifiers Render Settings",
            # ~ description="Use render settings when applying modifiers to mesh objects",
            # ~ default=False,
            # ~ )

    # extra data group
    #use_materials: BoolProperty(
    #        name="Write Materials",
    #        description="Write out the MTL file",
    #        default=True,
    #        )
    global_scale: FloatProperty(
            name="Scale",
            min=0.01, max=1000.0,
            default=1.0,
            )

    path_mode: path_reference_mode

    check_extension = True

    def execute(self, context):
        from . import export_nbt

        from mathutils import Matrix
        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))

        global_matrix = (Matrix.Scale(self.global_scale, 4) @
                         axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up,
                                         ).to_4x4())

        keywords["global_matrix"] = global_matrix
        return export_nbt.save(context, **keywords)


def menu_func_export(self, context):
    self.layout.operator(ExportNBT.bl_idname, text="Rohin Mesh (.nbt)")


classes = (
    ExportNBT,
)


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()
