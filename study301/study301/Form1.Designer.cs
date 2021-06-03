namespace study301
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.encodeBtn = new System.Windows.Forms.Button();
            this.inputTxt = new System.Windows.Forms.TextBox();
            this.outputTxt = new System.Windows.Forms.TextBox();
            this.decodeBtn = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.inputKeyTxt = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.privateTxt = new System.Windows.Forms.TextBox();
            this.publicTxt = new System.Windows.Forms.TextBox();
            this.makingKeyBtn = new System.Windows.Forms.Button();
            this.getFileBtn = new System.Windows.Forms.Button();
            this.md5HashTxt = new System.Windows.Forms.TextBox();
            this.sha1HashTxt = new System.Windows.Forms.TextBox();
            this.inputHashTxt = new System.Windows.Forms.TextBox();
            this.cklsbEnDecoder = new System.Windows.Forms.CheckedListBox();
            this.base64RadioBtn = new System.Windows.Forms.RadioButton();
            this.hexRadioBtn = new System.Windows.Forms.RadioButton();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // encodeBtn
            // 
            this.encodeBtn.Enabled = false;
            this.encodeBtn.Location = new System.Drawing.Point(38, 142);
            this.encodeBtn.Name = "encodeBtn";
            this.encodeBtn.Size = new System.Drawing.Size(75, 23);
            this.encodeBtn.TabIndex = 0;
            this.encodeBtn.Text = "암호화";
            this.encodeBtn.UseVisualStyleBackColor = true;
            this.encodeBtn.Click += new System.EventHandler(this.encodeBtn_Click);
            // 
            // inputTxt
            // 
            this.inputTxt.Enabled = false;
            this.inputTxt.Location = new System.Drawing.Point(119, 142);
            this.inputTxt.Multiline = true;
            this.inputTxt.Name = "inputTxt";
            this.inputTxt.Size = new System.Drawing.Size(419, 52);
            this.inputTxt.TabIndex = 1;
            // 
            // outputTxt
            // 
            this.outputTxt.Enabled = false;
            this.outputTxt.Location = new System.Drawing.Point(119, 200);
            this.outputTxt.Multiline = true;
            this.outputTxt.Name = "outputTxt";
            this.outputTxt.Size = new System.Drawing.Size(419, 52);
            this.outputTxt.TabIndex = 2;
            // 
            // decodeBtn
            // 
            this.decodeBtn.Enabled = false;
            this.decodeBtn.Location = new System.Drawing.Point(38, 171);
            this.decodeBtn.Name = "decodeBtn";
            this.decodeBtn.Size = new System.Drawing.Size(75, 23);
            this.decodeBtn.TabIndex = 3;
            this.decodeBtn.Text = "복호화";
            this.decodeBtn.UseVisualStyleBackColor = true;
            this.decodeBtn.Click += new System.EventHandler(this.decodeBtn_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // inputKeyTxt
            // 
            this.inputKeyTxt.Enabled = false;
            this.inputKeyTxt.Location = new System.Drawing.Point(119, 115);
            this.inputKeyTxt.MaxLength = 64;
            this.inputKeyTxt.Name = "inputKeyTxt";
            this.inputKeyTxt.Size = new System.Drawing.Size(174, 21);
            this.inputKeyTxt.TabIndex = 8;
            this.inputKeyTxt.TextChanged += new System.EventHandler(this.inputKeyTxt_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(299, 118);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(11, 12);
            this.label1.TabIndex = 9;
            this.label1.Text = "0";
            // 
            // privateTxt
            // 
            this.privateTxt.Enabled = false;
            this.privateTxt.Font = new System.Drawing.Font("굴림", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.privateTxt.Location = new System.Drawing.Point(325, 269);
            this.privateTxt.Multiline = true;
            this.privateTxt.Name = "privateTxt";
            this.privateTxt.Size = new System.Drawing.Size(213, 154);
            this.privateTxt.TabIndex = 11;
            // 
            // publicTxt
            // 
            this.publicTxt.Enabled = false;
            this.publicTxt.Font = new System.Drawing.Font("굴림", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.publicTxt.Location = new System.Drawing.Point(119, 269);
            this.publicTxt.Multiline = true;
            this.publicTxt.Name = "publicTxt";
            this.publicTxt.Size = new System.Drawing.Size(200, 154);
            this.publicTxt.TabIndex = 12;
            // 
            // makingKeyBtn
            // 
            this.makingKeyBtn.Enabled = false;
            this.makingKeyBtn.Location = new System.Drawing.Point(38, 269);
            this.makingKeyBtn.Name = "makingKeyBtn";
            this.makingKeyBtn.Size = new System.Drawing.Size(75, 23);
            this.makingKeyBtn.TabIndex = 3;
            this.makingKeyBtn.Text = "키생성";
            this.makingKeyBtn.UseVisualStyleBackColor = true;
            this.makingKeyBtn.Click += new System.EventHandler(this.makingKeyBtn_Click);
            // 
            // getFileBtn
            // 
            this.getFileBtn.Location = new System.Drawing.Point(38, 19);
            this.getFileBtn.Name = "getFileBtn";
            this.getFileBtn.Size = new System.Drawing.Size(75, 23);
            this.getFileBtn.TabIndex = 13;
            this.getFileBtn.Text = "파일";
            this.getFileBtn.UseVisualStyleBackColor = true;
            this.getFileBtn.Click += new System.EventHandler(this.getFileBtn_Click);
            // 
            // md5HashTxt
            // 
            this.md5HashTxt.Location = new System.Drawing.Point(119, 47);
            this.md5HashTxt.Name = "md5HashTxt";
            this.md5HashTxt.Size = new System.Drawing.Size(419, 21);
            this.md5HashTxt.TabIndex = 14;
            this.md5HashTxt.Text = "MD5Hash";
            // 
            // sha1HashTxt
            // 
            this.sha1HashTxt.Location = new System.Drawing.Point(119, 74);
            this.sha1HashTxt.Name = "sha1HashTxt";
            this.sha1HashTxt.Size = new System.Drawing.Size(419, 21);
            this.sha1HashTxt.TabIndex = 15;
            this.sha1HashTxt.Text = "SHA1Hash";
            // 
            // inputHashTxt
            // 
            this.inputHashTxt.Location = new System.Drawing.Point(119, 19);
            this.inputHashTxt.Name = "inputHashTxt";
            this.inputHashTxt.Size = new System.Drawing.Size(419, 21);
            this.inputHashTxt.TabIndex = 16;
            this.inputHashTxt.KeyUp += new System.Windows.Forms.KeyEventHandler(this.inputHashTxt_KeyUp);
            // 
            // cklsbEnDecoder
            // 
            this.cklsbEnDecoder.CheckOnClick = true;
            this.cklsbEnDecoder.FormattingEnabled = true;
            this.cklsbEnDecoder.Items.AddRange(new object[] {
            "UTF8",
            "Base64",
            "DES",
            "Rijndael",
            "RSA"});
            this.cklsbEnDecoder.Location = new System.Drawing.Point(544, 142);
            this.cklsbEnDecoder.Name = "cklsbEnDecoder";
            this.cklsbEnDecoder.Size = new System.Drawing.Size(75, 100);
            this.cklsbEnDecoder.TabIndex = 17;
            this.cklsbEnDecoder.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.cklsbEnDecoder_ItemCheck);
            // 
            // base64RadioBtn
            // 
            this.base64RadioBtn.AutoSize = true;
            this.base64RadioBtn.Checked = true;
            this.base64RadioBtn.Enabled = false;
            this.base64RadioBtn.Location = new System.Drawing.Point(423, 116);
            this.base64RadioBtn.Name = "base64RadioBtn";
            this.base64RadioBtn.Size = new System.Drawing.Size(64, 16);
            this.base64RadioBtn.TabIndex = 18;
            this.base64RadioBtn.TabStop = true;
            this.base64RadioBtn.Text = "Base64";
            this.base64RadioBtn.UseVisualStyleBackColor = true;
            this.base64RadioBtn.CheckedChanged += new System.EventHandler(this.base64RadioBtn_CheckedChanged);
            // 
            // hexRadioBtn
            // 
            this.hexRadioBtn.AutoSize = true;
            this.hexRadioBtn.Enabled = false;
            this.hexRadioBtn.Location = new System.Drawing.Point(493, 116);
            this.hexRadioBtn.Name = "hexRadioBtn";
            this.hexRadioBtn.Size = new System.Drawing.Size(45, 16);
            this.hexRadioBtn.TabIndex = 19;
            this.hexRadioBtn.TabStop = true;
            this.hexRadioBtn.Text = "Hex";
            this.hexRadioBtn.UseVisualStyleBackColor = true;
            this.hexRadioBtn.CheckedChanged += new System.EventHandler(this.hexRadioBtn_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(75, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 12);
            this.label2.TabIndex = 20;
            this.label2.Text = "MD5 :";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(70, 77);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(43, 12);
            this.label3.TabIndex = 21;
            this.label3.Text = "SHA1 :";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(74, 118);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(39, 12);
            this.label4.TabIndex = 22;
            this.label4.Text = "Key : ";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(186, 433);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(66, 12);
            this.label5.TabIndex = 23;
            this.label5.Text = "Public Key";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(397, 433);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(69, 12);
            this.label6.TabIndex = 24;
            this.label6.Text = "Private Key";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(648, 454);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.hexRadioBtn);
            this.Controls.Add(this.base64RadioBtn);
            this.Controls.Add(this.cklsbEnDecoder);
            this.Controls.Add(this.inputHashTxt);
            this.Controls.Add(this.sha1HashTxt);
            this.Controls.Add(this.md5HashTxt);
            this.Controls.Add(this.getFileBtn);
            this.Controls.Add(this.publicTxt);
            this.Controls.Add(this.privateTxt);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.inputKeyTxt);
            this.Controls.Add(this.makingKeyBtn);
            this.Controls.Add(this.decodeBtn);
            this.Controls.Add(this.outputTxt);
            this.Controls.Add(this.inputTxt);
            this.Controls.Add(this.encodeBtn);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button encodeBtn;
        private System.Windows.Forms.TextBox inputTxt;
        private System.Windows.Forms.TextBox outputTxt;
        private System.Windows.Forms.Button decodeBtn;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.TextBox inputKeyTxt;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox privateTxt;
        private System.Windows.Forms.TextBox publicTxt;
        private System.Windows.Forms.Button makingKeyBtn;
        private System.Windows.Forms.Button getFileBtn;
        private System.Windows.Forms.TextBox md5HashTxt;
        private System.Windows.Forms.TextBox sha1HashTxt;
        private System.Windows.Forms.TextBox inputHashTxt;
        private System.Windows.Forms.CheckedListBox cklsbEnDecoder;
        private System.Windows.Forms.RadioButton base64RadioBtn;
        private System.Windows.Forms.RadioButton hexRadioBtn;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
    }
}

