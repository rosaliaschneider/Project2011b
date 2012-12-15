#region Using directives

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using AviFile;

#endregion

namespace AviDemo 
{
    public partial class ExploreControl : UserControl 
    {
        public ExploreControl() 
        {
            InitializeComponent();
        }

        private void btnRead_Click(object sender, System.EventArgs e) 
        {
            AviManager aviManager = new AviManager(txtAviFileName.Text, true);
            VideoStream stream = aviManager.GetVideoStream();

            try {
                AudioStream waveStream = aviManager.GetWaveStream();
            } 
            catch {}
        }

        private void numPosition_ValueChanged(object sender, System.EventArgs e) 
        {
            ShowFrame();
        }

        private void ShowFrame() 
        {
            if (System.IO.File.Exists(txtAviFileName.Text)) {
                try {
                    AviManager aviManager = new AviManager(txtAviFileName.Text, true);
                    VideoStream aviStream = aviManager.GetVideoStream();
                    aviStream.GetFrameOpen();

                    Bitmap bmp = aviStream.GetBitmap(Convert.ToInt32(numPosition.Value));
                    picFrame.Image = bmp;
                    aviStream.GetFrameClose();
                    aviManager.Close();
                } 
                catch (Exception ex) {
                    MessageBox.Show(ex.ToString());
                }
            }
        }

        private void btnSelectFile_Click(object sender, System.EventArgs e) 
        {
            String fileName = GetFileName("Videos (*.avi)|*.avi;*.mpe;*.mpeg");
            if (fileName != null) {
                txtAviFileName.Text = fileName;
                ShowFrame();
            }
        }

        private String GetCurrentFilePath() 
        {
            return txtAviFileName.Text.Substring(0, txtAviFileName.Text.LastIndexOf("\\") + 1);
        }

        private void Form1_Load(object sender, System.EventArgs e)
        {
            ShowFrame();
        }

        private String GetFileName(String filter) 
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = filter;
            dlg.RestoreDirectory = true;
            if (txtAviFileName.Text.Length > 0) {
                dlg.InitialDirectory = GetCurrentFilePath();
            }
            if (dlg.ShowDialog(this) == DialogResult.OK) {
                return dlg.FileName;
            } else {
                return null;
            }
        }
    }
}
