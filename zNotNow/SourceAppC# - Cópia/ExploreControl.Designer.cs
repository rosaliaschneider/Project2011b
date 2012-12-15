namespace AviDemo 
{
    partial class ExploreControl 
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnSelectFile = new System.Windows.Forms.Button();
            this.txtAviFileName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.grpShow = new System.Windows.Forms.GroupBox();
            this.numPosition = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.picFrame = new System.Windows.Forms.PictureBox();
            this.panel1.SuspendLayout();
            this.grpShow.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numPosition)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picFrame)).BeginInit();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.btnSelectFile);
            this.panel1.Controls.Add(this.txtAviFileName);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Location = new System.Drawing.Point(4, 4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(917, 42);
            this.panel1.TabIndex = 14;
            // 
            // btnSelectFile
            // 
            this.btnSelectFile.Location = new System.Drawing.Point(659, 10);
            this.btnSelectFile.Name = "btnSelectFile";
            this.btnSelectFile.Size = new System.Drawing.Size(62, 20);
            this.btnSelectFile.TabIndex = 7;
            this.btnSelectFile.Text = "Browse...";
            this.btnSelectFile.Click += new System.EventHandler(this.btnSelectFile_Click);
            // 
            // txtAviFileName
            // 
            this.txtAviFileName.Location = new System.Drawing.Point(227, 10);
            this.txtAviFileName.Name = "txtAviFileName";
            this.txtAviFileName.Size = new System.Drawing.Size(425, 20);
            this.txtAviFileName.TabIndex = 5;
            this.txtAviFileName.Text = "..\\..\\testdata\\test.avi";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(127, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(100, 20);
            this.label2.TabIndex = 6;
            this.label2.Text = "Explore AVI File";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // grpShow
            // 
            this.grpShow.Controls.Add(this.numPosition);
            this.grpShow.Controls.Add(this.label1);
            this.grpShow.Controls.Add(this.picFrame);
            this.grpShow.Location = new System.Drawing.Point(4, 52);
            this.grpShow.Name = "grpShow";
            this.grpShow.Size = new System.Drawing.Size(917, 537);
            this.grpShow.TabIndex = 13;
            this.grpShow.TabStop = false;
            this.grpShow.Text = "Show Frames";
            // 
            // numPosition
            // 
            this.numPosition.Location = new System.Drawing.Point(864, 511);
            this.numPosition.Name = "numPosition";
            this.numPosition.Size = new System.Drawing.Size(47, 20);
            this.numPosition.TabIndex = 2;
            this.numPosition.ValueChanged += new System.EventHandler(this.numPosition_ValueChanged);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(757, 511);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(107, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "Extract Frame No.";
            // 
            // picFrame
            // 
            this.picFrame.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.picFrame.Location = new System.Drawing.Point(13, 19);
            this.picFrame.Name = "picFrame";
            this.picFrame.Size = new System.Drawing.Size(898, 486);
            this.picFrame.TabIndex = 0;
            this.picFrame.TabStop = false;
            // 
            // ExploreControl
            // 
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.grpShow);
            this.Name = "ExploreControl";
            this.Size = new System.Drawing.Size(933, 597);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.grpShow.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numPosition)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picFrame)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btnSelectFile;
        private System.Windows.Forms.TextBox txtAviFileName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox grpShow;
        private System.Windows.Forms.NumericUpDown numPosition;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox picFrame;
    }
}
