using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace edb
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private struct mesh_entry_header
        {
            public string name;
            public int numVerts;
            public int numInds;
            public byte flag;

            public byte[] data_block;
        };

        private struct mat_entry_header
        {
            public string name;
            public bool hasDiffuse;
            public bool hasNormal;
            public byte flag;

            public byte[] diffuse_data;
            public byte[] normal_data;
        }

        private class MeshTreeNode : TreeNode
        {
            public MeshTreeNode(mesh_entry_header mesh) :base(mesh.name)
            {
                Mesh_name = mesh.name;
                Num_verts = mesh.numVerts;
                Num_inds  = mesh.numInds;
            }

            private mesh_entry_header _mesh;
            public string Mesh_name
            {
                get { return _mesh.name; }
                set
                {
                    _mesh.name = value;
                    // Let it throw an exception if value was null
                    base.Text = _mesh.name;
                }
            }
            public int Num_verts
            {
                get { return _mesh.numVerts; }
                set
                {
                    _mesh.numVerts = value;
                    // Let it throw an exception if value was null
                }
            }
            public int Num_inds
            {
                get { return _mesh.numInds; }
                set
                {
                    _mesh.numInds = value;
                    // Let it throw an exception if value was null
                }
            }
        }


        private class MatTreeNode : TreeNode
        {
            public MatTreeNode(mat_entry_header mat) : base(mat.name)
            {
                Mat_name = mat.name;
                Has_diffuse = mat.hasDiffuse;
                Has_normal = mat.hasNormal;
            }

            private mat_entry_header _mat;
            public string Mat_name
            {
                get { return _mat.name; }
                set
                {
                    _mat.name = value;
                    // Let it throw an exception if value was null
                    base.Text = _mat.name;
                }
            }
            public bool Has_diffuse
            {
                get { return _mat.hasDiffuse; }
                set
                {
                    _mat.hasDiffuse = value;
                    // Let it throw an exception if value was null
                }
            }
            public bool Has_normal
            {
                get { return _mat.hasNormal; }
                set
                {
                    _mat.hasNormal = value;
                    // Let it throw an exception if value was null
                }
            }
        }

        private Dictionary<string, mesh_entry_header> mesh_dict;
        private Dictionary<string, mat_entry_header>  mat_dict;
        private bool dict_created = false;

        private void btnLoadMCF_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();

            string filename = openFileDialog1.FileName;
            byte[] data = File.ReadAllBytes(filename);
            //richTextBox1.Text = readfile;

            byte[] correct_signature = { 109, 99, 102, 72, 95, 107, 106, 0, 13, 10 };
            byte[] actual_signature = { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9] };

            if (ByteArrayCompare(correct_signature, actual_signature))
            {
                if (!dict_created)
                {
                    mesh_dict = new Dictionary<string, mesh_entry_header>();
                    mat_dict = new Dictionary<string, mat_entry_header>();
                    dbTreeView.Nodes.Add("Meshes");
                    dbTreeView.Nodes.Add("Materials");
                    dbTreeView.Nodes.Add("Entities");
                    dict_created = true;
                }

                // try Array.Copy instead
                // https://stackoverflow.com/questions/44928541/how-to-skip-first-few-elements-from-array
                //
                var numMeshes_byteArray = data.Skip(94).Take(10).ToArray();
                var numMats_byteArray = data.Skip(127).Take(10).ToArray();
                var numEntities_byteArray = data.Skip(159).Take(10).ToArray();

                string nme_str = Encoding.UTF8.GetString(numMeshes_byteArray, 0, numMeshes_byteArray.Length);
                string nva_str = Encoding.UTF8.GetString(numMats_byteArray, 0, numMats_byteArray.Length);
                string nen_str = Encoding.UTF8.GetString(numEntities_byteArray, 0, numEntities_byteArray.Length);

                int total_numMeshes = Int32.Parse(nme_str);
                int total_numMats = Int32.Parse(nva_str);
                int total_numEntities = Int32.Parse(nen_str);

                var mesh_data = data.Skip(184).ToArray();

                for (int n = 0; n < total_numMeshes; n++)
                {
                    var mesh_name_arr = mesh_data.Skip(6).ToArray();
                    int end = 0;
                    bool not_done = (mesh_name_arr[end] != 13) && (mesh_name_arr[end] != 10) && (mesh_name_arr[end] != 0);
                    while (not_done)
                    {
                        end++;
                        not_done = (mesh_name_arr[end] != 13) && (mesh_name_arr[end] != 10) && (mesh_name_arr[end] != 0);
                    }
                    string mesh_name = Encoding.UTF8.GetString(mesh_name_arr, 0, end);

                    int offset = mesh_name.Length + 8;

                    byte flag = mesh_data[offset];
                    int numInds  = BitConverter.ToInt32(mesh_data, offset+1);
                    byte[] mesh_indices = mesh_data.Skip(offset + 5).Take(numInds * 4).ToArray();
                    int numVerts = BitConverter.ToInt32(mesh_data, offset + 5 + numInds*4);
                    byte[] mesh_verts = mesh_data.Skip(offset + 6 + numInds*4).Take(numVerts * 12).ToArray();

                    offset += 7 + numInds * 4 + numVerts * 12;

                    // now see if we need to extract normals, UVs, and tangents.
                    bool hasNormals     = (flag & (1 << 0)) != 0;
                    bool hasUVs         = (flag & (1 << 1)) != 0;
                    bool hasTangents    = (flag & (1 << 2)) != 0;
                    bool hasfourth      = (flag & (1 << 3)) != 0; // not used

                    if (hasNormals)
                    {
                        byte[] mesh_normals = mesh_data.Skip(offset).Take(numVerts * 12).ToArray();
                        offset += numVerts * 12;
                    }
                    if (hasUVs)
                    {
                        byte[] mesh_uvs = mesh_data.Skip(offset).Take(numVerts * 8).ToArray();
                        offset += numVerts * 8;
                    }
                    if (hasTangents)
                    {
                        byte[] mesh_tangents = mesh_data.Skip(offset).Take(numVerts * 16).ToArray();
                        offset += numVerts * 16;
                    }

                    offset += 4;

                    // store in dictionary
                    Form1.mesh_entry_header meh;
                    meh.name = mesh_name;
                    meh.numInds = numInds;
                    meh.numVerts = numVerts;
                    meh.flag = flag;
                    meh.data_block = mesh_data.Take(offset).ToArray();

                    if (!mesh_dict.ContainsKey(meh.name))
                    {
                        // new mesh, add to dictionary.
                        mesh_dict.Add(meh.name, meh);
                    }

                    // advance along the byte-stream
                    mesh_data = mesh_data.Skip(offset).ToArray();
                }

                for (int n = 0; n < total_numMats; n++)
                {
                    // "Material: "
                    var mat_name_arr = mesh_data.Skip(10).ToArray();
                    int end = 0;
                    bool not_done = (mat_name_arr[end] != 13) && (mat_name_arr[end] != 10) && (mat_name_arr[end] != 0);
                    while (not_done)
                    {
                        end++;
                        not_done = (mat_name_arr[end] != 13) && (mat_name_arr[end] != 10) && (mat_name_arr[end] != 0);
                    }
                    string mat_name = Encoding.UTF8.GetString(mat_name_arr, 0, end);

                    int offset = mat_name.Length + 12;

                    byte flag = mesh_data[offset];
                    bool hasDiffuse = (flag & (1 << 0)) != 0;
                    bool hasNormals = (flag & (1 << 1)) != 0;
                    bool hasThird   = (flag & (1 << 2)) != 0; // not used
                    bool hasFourth  = (flag & (1 << 3)) != 0; // not used

                    offset++;
                    mat_entry_header mah = new mat_entry_header();

                    if (hasDiffuse)
                    {
                        int byteLength = BitConverter.ToInt32(mesh_data, offset);
                        offset += 4;
                        mah.diffuse_data = mesh_data.Skip(offset).Take(byteLength).ToArray();
                        // need to also store the byteLength and texture info
                        offset += byteLength;
                    }

                    if (hasNormals)
                    {
                        int byteLength = BitConverter.ToInt32(mesh_data, offset);
                        offset += 4;
                        mah.normal_data = mesh_data.Skip(offset).Take(byteLength).ToArray();
                        // need to also store the byteLength and texture info
                        offset += byteLength;
                    }

                    offset += 2;

                    // store in dictionary
                    mah.name = mat_name;
                    mah.hasDiffuse = hasDiffuse;
                    mah.hasNormal = hasNormals;
                    mah.flag = flag;

                    if (!mat_dict.ContainsKey(mah.name))
                    {
                        // new mesh, add to dictionary.
                        mat_dict.Add(mah.name, mah);
                    }

                    // advance along the byte-stream
                    mesh_data = mesh_data.Skip(offset).ToArray();
                }

                var meshes_loaded = mesh_dict.Keys.ToArray();
                var mats_loaded = mat_dict.Keys.ToArray();

                dbTreeView.Nodes[0].Nodes.Clear();
                dbTreeView.Nodes[1].Nodes.Clear();
                for (int n = 0; n < mesh_dict.Keys.Count; n++)
                {
                    var node = dbTreeView.Nodes[0].Nodes.Add(new MeshTreeNode(mesh_dict[meshes_loaded[n]]));
                }
                for (int n = 0; n < mat_dict.Keys.Count; n++)
                {
                    var node = dbTreeView.Nodes[1].Nodes.Add(new MatTreeNode(mat_dict[mats_loaded[n]]));
                }

                MessageBox.Show("Done loading catalog file. " + mesh_dict.Keys.Count + " unique meshes loaded, and " + mat_dict.Keys.Count + " unique materials loaded.",
                    "Done loading", MessageBoxButtons.OK, MessageBoxIcon.Information);
            } else
            {
                MessageBox.Show("Incorrect file selected. A proper Mesh Catalog File needs to be selected. Try again.", 
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void dbTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            entityPropertyGrid.SelectedObject = dbTreeView.SelectedNode;
            //dbTreeView.SelectedNode.Tag = "ayo";
            //entityPropertyGrid.PropertyTabs
        }

        private void btnAddEntity_Click(object sender, EventArgs e)
        {
            if (dbTreeView.GetNodeCount(false) > 0)
            {
                dbTreeView.Nodes[0].Nodes.Add("Entity_001");
                dbTreeView.Nodes[0].Nodes.Add("Entity_002");
                dbTreeView.Nodes[0].Nodes.Add("Entity_003");
            }
        }

        private void btnRemoveEntity_Click(object sender, EventArgs e)
        {
            if (dbTreeView.GetNodeCount(false) > 0)
            {
                dbTreeView.SelectedNode.Remove();
            }
        }

        private bool ByteArrayCompare(byte[] a1, byte[] a2)
        {
            if (a1.Length != a2.Length)
                return false;

            for (int i = 0; i < a1.Length; i++)
                if (a1[i] != a2[i])
                    return false;

            return true;
        }
    }
}
