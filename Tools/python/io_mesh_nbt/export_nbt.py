import os
import bpy
import bmesh
import mathutils
from mathutils import Matrix
from collections import namedtuple
from collections import defaultdict
from struct import pack

def write_header(f, version_major, version_minor, endianness):
    f.write(b"1234") #signature
    f.write(version_major.to_bytes(1, byteorder=endianness, signed=True))
    f.write(version_minor.to_bytes(1, byteorder=endianness, signed=True))
    if endianness == 'big':
        f.write((1).to_bytes(1, byteorder=endianness, signed=True))
    elif endianness == 'little':
        f.write((0).to_bytes(1, byteorder=endianness, signed=True))
    
    # padding
    f.write((0).to_bytes(25, byteorder=endianness, signed=True))
    
def write_float(f, x, numBytes, e):
    form = ['>', 'f']
    
    if e == 'little':
        form[0] = '<'
    if numBytes == 8:
        form[1] = 'd'

    bb = pack("".join(form), x)
    f.write(bb)
    
def write_num(f, num, numBytes, e):
    f.write(num.to_bytes(numBytes, byteorder=e, signed=True))
    
def write_tag(f, value):
    write_num(f, value, 1, 'little') #byte order doesnt matter for single byte

def write_string(f, string, e):
    write_num(f, len(string), 2, e)
    f.write(bytearray(string, encoding='utf-8'))
    
def pack_verts(vertices, e):
    packed = []
    for vert in vertices:
        pos = vert.position
        nor = vert.normal
        tan = vert.tangent
        bit = vert.bitangent
        uvc = vert.uv
        
        format = '<'
        if e == 'big':
            format = '>'
        
        format = format + ('fff') #position
        format = format + ('fff') #normal
        format = format + ('fff') #tangent
        format = format + ('fff') #bitangent
        format = format + ('ff') #uv
        
        from struct import pack
        packed.append(pack(format, 
            pos[0], pos[1], pos[2],
            nor[0], nor[1], nor[2],
            tan[0], tan[1], tan[2],
            bit[0], bit[1], bit[2],
            uvc[0], uvc[1]))
    return packed
    
def write_start_compound(f, e):
    # start compound
    write_tag(f, 10)
    write_string(f, "mesh_data", e) 

def write_mesh_data(f, vertices, indices, e):    
    # tag_int: numVerts
    write_tag(f, 3)
    write_string(f, "num_verts", e)
    write_num(f, len(vertices), 4, e) # payload
    
    # tag_int: numIndices
    write_tag(f, 3)
    write_string(f, "num_inds", e)
    write_num(f, len(indices), 4, e) # payload
    
    # tag_byte_array: vertices
    write_tag(f, 7)
    write_string(f, "vertices", e)
    packed_verts = pack_verts(vertices, e)
    num_bytes = len(packed_verts)*len(packed_verts[0])
    write_num(f, num_bytes, 4, e)
    for p in packed_verts:
        f.write(p)
    
    # tag_int_array: indices
    write_tag(f, 11)
    write_string(f, "indices", e)
    num_ints = len(indices)
    write_num(f, num_ints, 4, e)
    for p in indices:
        write_num(f, p, 4, e)

def write_end_compound(f):
    # tag_end: end of "mesh_data" compound
    write_tag(f, 0)

def write_texture_data(f, albedo_path, normal_path, metalness_path, roughness_path, e):
    # tag_string: u_albedo_path
    write_tag(f, 8)
    write_string(f, "u_albedo_path", e)
    if albedo_path is None:
        write_string(f, 'run_tree/Data/Images/frog.png', e)
    else:
        write_string(f, albedo_path, e)

    # tag_string: u_normal_path
    write_tag(f, 8)
    write_string(f, "u_normal_path", e)
    if normal_path is None:
        write_string(f, 'run_tree/Data/Images/frog.png', e)
    else:
        write_string(f, normal_path, e)

    # tag_string: u_metalness_path
    write_tag(f, 8)
    write_string(f, "u_metalness_path", e)
    if metalness_path is None:
        write_string(f, 'run_tree/Data/Images/frog.png', e)
    else:
        write_string(f, metalness_path, e)

    # tag_string: u_roughness_path
    write_tag(f, 8)
    write_string(f, "u_roughness_path", e)
    if roughness_path is None:
        write_string(f, 'run_tree/Data/Images/frog.png', e)
    else:
        write_string(f, roughness_path, e)
    

def triangulate_mesh(mesh):
    # Get a BMesh representation
    bm = bmesh.new()
    bm.from_mesh(mesh)

    print(len(bm.faces), "tris before triangulate")
    bmesh.ops.triangulate(bm, faces=bm.faces[:])
    print(len(bm.faces), "tris after triangulate")
    print()
    # V2.79 : bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

    # Finish up, write the bmesh back to the mesh
    bm.to_mesh(mesh)
    bm.free()

def cut_seams(mesh):    
    # Get a BMesh representation
    bm = bmesh.new()
    bm.from_mesh(mesh)
    
    edges = []
    for edge in bm.edges:
        if edge.seam:
            edges.append(edge)
    
    # cut the seams now
    print(len(bm.verts), "verts before edge split")
    bmesh.ops.split_edges(bm, edges=edges)
    print(len(bm.verts), "verts after edge split")
    print()

    # Finish up, write the bmesh back to the mesh
    bm.to_mesh(mesh)
    bm.free()
        
# get the average vector from a list of vectors
def averageVectors(dupes):
    total = mathutils.Vector((0,0,0))
    for d in dupes:
        v = mathutils.Vector((d[0],d[1],d[2]))
        total = total + v
    total.normalize()
    return [total[0],total[1],total[2]]

def averageAtIndex(dvals, arr):
    dupes = []
    for i in dvals:
        dupes.append(arr[i])
    avg = averageVectors(dupes)
    return avg
        
# choose the correct UV from a list
def getUV(dupes):
    # take the easiest way out
    # TODO: add error checking
    return dupes[0]

def getCustomString(obj, key):
    if key in obj:
        return obj[key]
    else:
        return None

def nbt_write_test(context, filepath, global_matrix): #-- , apply_modifiers, export_selection, global_matrix, path_mode):
    print("Writing mesh to .nbt file!")

    if global_matrix is None:
        global_matrix = Matrix()
    #obj = context.selected_objects[0]
    obj = context.view_layer.objects.active
    mesh = obj.data.copy()
    mesh.transform(global_matrix)
    cut_seams(mesh)
    triangulate_mesh(mesh)
    mesh.calc_tangents()

    # get texture strings
    albedo_path = getCustomString(obj, 'u_albedo_path')
    normal_path = getCustomString(obj, 'u_normal_path')
    metalness_path = getCustomString(obj, 'u_metalness_path')
    roughness_path = getCustomString(obj, 'u_roughness_path')

    print('u_albedo_path', albedo_path)
    print('u_normal_path', normal_path)
    print('u_metalness_path', metalness_path)
    print('u_roughness_path', roughness_path)

    # indices: point to index of vertex in mesh.vertices
    indices = []
    for face in mesh.polygons:
        indices.append(face.vertices[0])
        indices.append(face.vertices[1])
        indices.append(face.vertices[2])

    # positions:
    positions = []
    for v in mesh.vertices:
        positions.append([v.co[0], v.co[1], v.co[2]])
        
    # uvs:
    uvs_indexed = []
    normals_indexed = []
    tangents_indexed = []
    bitangents_indexed = []
    vertex_indices = []
    for face in mesh.polygons:
        #for vert_idx, loop_idx in zip(face.vertices, face.loop_indices):
        for loop_idx in face.loop_indices:
            vert = mesh.loops[loop_idx]
            
            v_idx = vert.vertex_index
            normal = [vert.normal[0],vert.normal[1],vert.normal[2]]
            tangent = [vert.tangent[0],vert.tangent[1],vert.tangent[2]]
            bt = vert.bitangent_sign * vert.normal.cross(vert.tangent)
            bitangent = [bt[0],bt[1],bt[2]]
            uv_v = mesh.uv_layers.active.data[loop_idx].uv
            uv = [uv_v[0], uv_v[1]]

            #print("v_idx", v_idx, "normal", normal)
            
            vertex_indices.append(v_idx)
            uvs_indexed.append(uv)
            normals_indexed.append(normal)
            tangents_indexed.append(tangent)
            bitangents_indexed.append(bitangent)
            
    # construct dictionary of duplicated vertices
    d = defaultdict(list)
    
    for n in range(len(indices)):
        big_idx = n
        lit_idx = indices[n]
        
        d[lit_idx].append(big_idx)

    # de-index and remove duplicate normals/tangents/bitangents
    normals = [[]]*len(positions)
    tangents = [[]]*len(positions)
    bitangents = [[]]*len(positions)
    uvs = [[]]*len(positions)
    for index in d.keys():
        uv = uvs_indexed[d[index][0]] # just take the first
        uvs[index] = [uv[0], 1-uv[1]] # flip uv y-axis
        normals[index] = averageAtIndex(d[index], normals_indexed)
        tangents[index] = averageAtIndex(d[index], tangents_indexed)
        bitangents[index] = averageAtIndex(d[index], bitangents_indexed)
        # TODO: are these actually orthonormal?
    
    vertex = namedtuple("vertex", "position uv normal tangent bitangent")
    
    vertices = []
    for n in range(len(positions)):
        v = vertex(positions[n], uvs[n], normals[n], tangents[n], bitangents[n])
        vertices.append(v)
    
    with open(filepath, "wb") as f:
        print("Opening %s for writing..." % filepath)
        version_major = 0
        version_minor = 1
        endianness = 'little'
        write_header(f, version_major, version_minor, endianness)
        write_start_compound(f, endianness)
        write_mesh_data(f, vertices, indices, endianness)
        write_texture_data(f, albedo_path, normal_path, metalness_path, roughness_path, endianness)
        write_end_compound(f)
        
        print('done!')

def save(context,
         filepath,
         *,
         use_selection=True,
         use_mesh_modifiers=True,
         path_mode=None,
         global_matrix=None,
         ):

    nbt_write_test(context, filepath, global_matrix)

    return {'FINISHED'}

if __name__ == "__main__":
    context = bpy.context
    apply_modifiers = True
    export_selection = True
    global_scale = 1.0
    global_matrix = Matrix.Scale(global_scale, 4)
    path_mode = "uhh"
    filepath = "D:\\Desktop\\Gamedev\\rohin\\Game\\run_tree\\Data\\Models\\guard.nbt"
    
    save(context, filepath)
    #nbt_write_test(context, filepath, apply_modifiers, export_selection, global_matrix, path_mode)