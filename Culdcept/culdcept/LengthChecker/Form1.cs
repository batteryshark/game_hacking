using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using theli;

namespace LengthChecker
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void TextBox_TextChanged(object sender, EventArgs e)
        {
            curlength.Text = SJSSpecEncoder.Encode(TextBox.Lines).Length.ToString();
        }
    }
}
