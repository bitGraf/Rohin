import os
import bpy
import bmesh
from mathutils import Matrix

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
    
    from struct import pack
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
    
def write_mesh_data(f, vertices, indices, e):
    # start compound
    write_tag(f, 10)
    write_string(f, "mesh_data", e) 
    
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
    
    # tag_end: end of "mesh_data" compound
    write_tag(f, 0)
    

def triangulate_object(me):
    # Get a BMesh representation
    bm = bmesh.new()
    bm.from_mesh(me)

    bmesh.ops.triangulate(bm, faces=bm.faces[:])
    # V2.79 : bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

    # Finish up, write the bmesh back to the mesh
    bm.to_mesh(me)
    bm.free()
        
# get the average vector from a list of vectors
def averageVectors(dupes):
    import mathutils
    total = mathutils.Vector((0,0,0))
    for d in dupes:
        v = mathutils.Vector((d[0],d[1],d[2]))
        total = total + v
    total.normalize()
    return [total[0],total[1],total[2]]
        
# choose the correct UV from a list
def getUV(dupes):
    # take the easiest way out
    # TODO: add error checking
    return dupes[0]

def nbt_write_test(context, filepath): #-- , apply_modifiers, export_selection, global_matrix, path_mode):
    print("Writing mesh to .nbt file!")
    #obj = context.selected_objects[0]
    obj = context.view_layer.objects.active
    mesh = obj.data.copy()
    triangulate_object(mesh)
    mesh.calc_tangents()
    # TODO: need to make sure that all uv seams are spli

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
        for vert_idx, loop_idx in zip(face.vertices, face.loop_indices):
            vert = mesh.loops[loop_idx]
            
            v_idx = vert.vertex_index
            normal = [vert.normal[0],vert.normal[1],vert.normal[2]]
            tangent = [vert.tangent[0],vert.tangent[1],vert.tangent[2]]
            bt = vert.bitangent_sign * vert.normal.cross(vert.tangent)
            bitangent = [bt[0],bt[1],bt[2]]
            uv_v = mesh.uv_layers.active.data[loop_idx].uv
            uv = [uv_v[0], uv_v[1]]
            
            vertex_indices.append(v_idx)
            uvs_indexed.append(uv)
            normals_indexed.append(normal)
            tangents_indexed.append(tangent)
            bitangents_indexed.append(bitangent)
        
    # de-index arrays (still have duplicates/multiples)
    uvs_d = uvs_indexed.copy()
    normals_d = normals_indexed.copy()
    tangents_d = tangents_indexed.copy()
    bitangents_d = bitangents_indexed.copy()
    for n in range(len(vertex_indices)):
        index = vertex_indices[n]
        uvs_d[n] = uvs_indexed[n] # uvs are already de-indexed? TODO: check this
        normals_d[n] = normals_indexed[index]
        tangents_d[n] = tangents_indexed[index]
        bitangents_d[n] = bitangents_indexed[index]

    # get averages for each normal/tangent/bitangent
    from collections import defaultdict
    d = defaultdict(list)
    for n in range(len(indices)):
        big_idx = n
        lit_idx = indices[n]
        
        d[lit_idx].append(big_idx)

    normals = [None] * len(positions)
    tangents = [None] * len(positions)
    bitangents = [None] * len(positions)
    uvs = [None] * len(positions)
    for key in d:
        # key is vertex index
        # d[key] is a list of vertices that share an index
        #print(key, d[key])
        
        # normals
        dupes = []
        for n in d[key]:
            dupes.append(normals_d[n])
        normal = averageVectors(dupes)
        
        #tangents
        dupes = []
        for n in d[key]:
            dupes.append(tangents_d[n])
        tangent = averageVectors(dupes)
        
        #bitangents
        dupes = []
        for n in d[key]:
            dupes.append(bitangents_d[n])
        bitangent = averageVectors(dupes)
        
        
        # renormalize
        import mathutils
        nn = mathutils.Vector(normal)
        tt = mathutils.Vector(tangent)
        bb_ = mathutils.Vector(bitangent)
        
        # keep normal
        # b = n x t
        # t = b x n
        # need to use original sign of b to 
        bb = nn.cross(tt)
        tt = bb.cross(nn)
        if bb.dot(nn) * bb_.dot(nn) < 0:
            # incorrect sign
            bb = -bb
        
        # assign to index
        normals[key] = [nn[0],nn[1],nn[2]]
        tangents[key] = [tt[0],tt[1],tt[2]]
        bitangents[key] = [bb[0],bb[1],bb[2]]
        
        # check all uvs on a vertex are identical
        dupes = []
        for n in d[key]:
            dupes.append(uvs_d[n])
        uv = getUV(dupes)
        
        uvs[key] = [uv[0], 1-uv[1]] #flip y-dir?
    
    from collections import namedtuple
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
        write_mesh_data(f, vertices, indices, endianness)
        
        print('done!')

def save(context,
         filepath,
         *,
         use_selection=True,
         use_mesh_modifiers=True,
         path_mode=None,
         global_matrix=None,
         ):

    nbt_write_test(context, filepath)

    return {'FINISHED'}

if __name__ == "__main__":
    context = bpy.context
    apply_modifiers = True
    export_selection = True
    global_scale = 1.0
    global_matrix = Matrix.Scale(global_scale, 4)
    path_mode = "uhh"
    filepath = "D:\\Desktop\\Gamedev\\rohin\\Game\\run_tree\\Data\\Models\\cube.nbt_mesh"
    
    save(context, filepath)
    #nbt_write_test(context, filepath, apply_modifiers, export_selection, global_matrix, path_mode)