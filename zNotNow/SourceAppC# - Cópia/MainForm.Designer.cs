namespace AviDemo {
    partial class MainForm {
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.exploreControl1 = new AviDemo.ExploreControl();
            this.SuspendLayout();
            // 
            // exploreControl1
            // 
            this.exploreControl1.Location = new System.Drawing.Point(12, 12);
            this.exploreControl1.Name = "exploreControl1";
            this.exploreControl1.Size = new System.Drawing.Size(926, 590);
            this.exploreControl1.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(937, 606);
            this.Controls.Add(this.exploreControl1);
            this.Name = "MainForm";
            this.Text = "Basics of AVI Files";
            this.ResumeLayout(false);

        }

        #endregion

        private ExploreControl exploreControl1;
    }
}