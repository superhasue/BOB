using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace study301
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
       
        IEnDecoder enDecoder;
        HashConverter hash = new HashConverter();

        //파일 버튼
        private void getFileBtn_Click(object sender, EventArgs e)
        {
            if (this.openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                this.inputHashTxt.Text = this.openFileDialog1.FileName;
            }

            this.md5HashTxt.Text = hash.MD5HashFile(this.inputHashTxt.Text);
            this.sha1HashTxt.Text = hash.SHA1HashFile(this.inputHashTxt.Text);

        }

        //텍스트박스 해쉬 직접입력
        private void inputHashTxt_KeyUp(object sender, KeyEventArgs e)
        {
            if (this.inputHashTxt.Text == string.Empty)
            {
                this.md5HashTxt.Text = string.Empty;
                this.sha1HashTxt.Text = string.Empty;

                return;
            }

            this.md5HashTxt.Text = hash.MD5HashTxt(this.inputHashTxt.Text);
            this.sha1HashTxt.Text = hash.SHA1HashTxt(this.inputHashTxt.Text);

        }

        //암호화 버튼 클릭
        private void encodeBtn_Click(object sender, EventArgs e)
        {
            enDecoder.key = inputKeyTxt.Text;
            outputTxt.Text = enDecoder.Encode(inputTxt.Text);
        }

        //복호화 버튼 클릭
        private void decodeBtn_Click(object sender, EventArgs e)
        {
            enDecoder.key = inputKeyTxt.Text;
            outputTxt.Text = enDecoder.Decode(inputTxt.Text);

        }

        //키 텍스트박스 글자수 세기
        private void inputKeyTxt_TextChanged(object sender, EventArgs e)
        {
            label1.Text = inputKeyTxt.Text.Length.ToString();
        }

        //체크박스 체크
        private void cklsbEnDecoder_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            //중복체크 방지
            if (e.NewValue == CheckState.Checked)
                for (int i = 0; i < cklsbEnDecoder.Items.Count; ++i)
                    if (e.Index != i) cklsbEnDecoder.SetItemChecked(i, false);

            //초기화
            hexRadioBtn.Enabled = false;
            base64RadioBtn.Enabled = false;
            inputKeyTxt.Enabled = false;
            inputTxt.Enabled = true;
            outputTxt.Enabled = true;
            encodeBtn.Enabled = true;
            decodeBtn.Enabled = true;
            makingKeyBtn.Enabled = false;
            privateTxt.Enabled = false;
            publicTxt.Enabled = false;
            privateTxt.Clear();
            publicTxt.Clear();
            inputKeyTxt.Clear();

            switch (cklsbEnDecoder.GetItemText(cklsbEnDecoder.Items[e.Index]))
            {
                case "UTF8":
                    enDecoder = new UTF8EnDecoder();
                    return;
                case "Base64":
                    enDecoder = new Base64EnDecoder();
                    return;
                case "DES":
                    hexRadioBtn.Enabled = true;
                    base64RadioBtn.Enabled = true;
                    inputKeyTxt.Enabled = true;
                    inputKeyTxt.MaxLength = 8;
                    enDecoder = new DESEnDecoder();
                    return;
                case "Rijndael":
                    hexRadioBtn.Enabled = true;
                    base64RadioBtn.Enabled = true;
                    inputKeyTxt.Enabled = true;
                    inputKeyTxt.MaxLength = 32;
                    enDecoder = new RijndaelEnDecoder();
                    return;
                case "RSA":
                    hexRadioBtn.Enabled = true;
                    base64RadioBtn.Enabled = true;
                    privateTxt.Enabled = true;
                    publicTxt.Enabled = true;
                    makingKeyBtn.Enabled = true;
                    enDecoder = new RSAEnDecoder();
                    return;
            }

        }

        //공개키 암호화키 생성
        private void makingKeyBtn_Click(object sender, EventArgs e)
        {
            RSAEnDecoder rsa = (RSAEnDecoder)enDecoder;

            rsa.MakingKey();
            this.privateTxt.Text = rsa.privateKeyText.Replace(">", ">\r\n").Replace("</", "\r\n</");
            this.publicTxt.Text = rsa.publicKeyText.Replace(">", ">\r\n").Replace("</", "\r\n</");

        }

        private void base64RadioBtn_CheckedChanged(object sender, EventArgs e)
        {
            enDecoder.format = "Base64";
        }

        private void hexRadioBtn_CheckedChanged(object sender, EventArgs e)
        {
            enDecoder.format = "Hex";
        }
    }
}
