namespace edb
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.btnLoadMCF = new System.Windows.Forms.Button();
            this.dbFilePathLabel = new System.Windows.Forms.Label();
            this.dbTreeView = new System.Windows.Forms.TreeView();
            this.btnAddEntity = new System.Windows.Forms.Button();
            this.btnRemoveEntity = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.entityPropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // btnLoadMCF
            // 
            this.btnLoadMCF.Location = new System.Drawing.Point(12, 11);
            this.btnLoadMCF.Name = "btnLoadMCF";
            this.btnLoadMCF.Size = new System.Drawing.Size(75, 23);
            this.btnLoadMCF.TabIndex = 0;
            this.btnLoadMCF.Text = "Load File";
            this.btnLoadMCF.UseVisualStyleBackColor = true;
            this.btnLoadMCF.Click += new System.EventHandler(this.btnLoadMCF_Click);
            // 
            // dbFilePathLabel
            // 
            this.dbFilePathLabel.AutoSize = true;
            this.dbFilePathLabel.Location = new System.Drawing.Point(97, 16);
            this.dbFilePathLabel.Name = "dbFilePathLabel";
            this.dbFilePathLabel.Size = new System.Drawing.Size(137, 13);
            this.dbFilePathLabel.TabIndex = 2;
            this.dbFilePathLabel.Text = "Select entity database file...";
            // 
            // dbTreeView
            // 
            this.dbTreeView.FullRowSelect = true;
            this.dbTreeView.Location = new System.Drawing.Point(12, 89);
            this.dbTreeView.Name = "dbTreeView";
            this.dbTreeView.Size = new System.Drawing.Size(222, 349);
            this.dbTreeView.TabIndex = 4;
            this.dbTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.dbTreeView_AfterSelect);
            // 
            // btnAddEntity
            // 
            this.btnAddEntity.Location = new System.Drawing.Point(12, 60);
            this.btnAddEntity.Name = "btnAddEntity";
            this.btnAddEntity.Size = new System.Drawing.Size(75, 23);
            this.btnAddEntity.TabIndex = 5;
            this.btnAddEntity.Text = "Add Entity...";
            this.btnAddEntity.UseVisualStyleBackColor = true;
            this.btnAddEntity.Click += new System.EventHandler(this.btnAddEntity_Click);
            // 
            // btnRemoveEntity
            // 
            this.btnRemoveEntity.Location = new System.Drawing.Point(136, 60);
            this.btnRemoveEntity.Name = "btnRemoveEntity";
            this.btnRemoveEntity.Size = new System.Drawing.Size(98, 23);
            this.btnRemoveEntity.TabIndex = 6;
            this.btnRemoveEntity.Text = "Remove Entity...";
            this.btnRemoveEntity.UseVisualStyleBackColor = true;
            this.btnRemoveEntity.Click += new System.EventHandler(this.btnRemoveEntity_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(258, 60);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(530, 378);
            this.tabControl1.TabIndex = 7;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.entityPropertyGrid);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(522, 352);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Entity View";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // entityPropertyGrid
            // 
            this.entityPropertyGrid.Location = new System.Drawing.Point(0, 0);
            this.entityPropertyGrid.Name = "entityPropertyGrid";
            this.entityPropertyGrid.Size = new System.Drawing.Size(522, 352);
            this.entityPropertyGrid.TabIndex = 1;
            this.entityPropertyGrid.Tag = "";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.richTextBox1);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(522, 352);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Database Raw View";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(6, 3);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(510, 343);
            this.richTextBox1.TabIndex = 4;
            this.richTextBox1.Text = "";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.btnRemoveEntity);
            this.Controls.Add(this.btnAddEntity);
            this.Controls.Add(this.dbTreeView);
            this.Controls.Add(this.dbFilePathLabel);
            this.Controls.Add(this.btnLoadMCF);
            this.Name = "Form1";
            this.Text = "Entity Database Manager";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button btnLoadMCF;
        private System.Windows.Forms.Label dbFilePathLabel;
        private System.Windows.Forms.TreeView dbTreeView;
        private System.Windows.Forms.Button btnAddEntity;
        private System.Windows.Forms.Button btnRemoveEntity;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.PropertyGrid entityPropertyGrid;
    }
}