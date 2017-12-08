using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void listView1_MouseClick(object sender, MouseEventArgs e)
        {

        }

        private void populate() {
            ImageList imgs = new ImageList();
            imgs.ImageSize = new Size(100,100);

            String[] paths = { };
            paths = System.IO.Directory.GetFiles("E:/Programozas/Github/BME-Onlab/WS2812ConfigurationProgram/stilusok");

            try
            {
                foreach (String path in paths)
                {
                    imgs.Images.Add(Image.FromFile(path));
                }
            }
            catch (Exception e){
                MessageBox.Show(e.Message);
            }


            listView1.SmallImageList = imgs;
            listView1.Items.Add("Első stílus", 0);
            listView1.Items.Add("Második stílus", 1);
            listView1.Items.Add("Harmadik Stílus", 2);

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            listView1.View = View.Details;

            listView1.Columns.Add("Kinézet",150);
            listView1.AutoResizeColumn(0, ColumnHeaderAutoResizeStyle.HeaderSize);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            populate();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK) {
                display(colorDialog1.Color.Name);
            }

        }

        private void display(string text) {
            MessageBox.Show(text);
        }

    }
}
