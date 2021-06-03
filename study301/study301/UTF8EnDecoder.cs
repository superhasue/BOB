using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace study301
{
    class UTF8EnDecoder : IEnDecoder
    {
        public string key { get; set; }
        public string IV { get; set; }
        public string format { get; set; }

        public UTF8EnDecoder()
        {

        }

        public string Encode(string sParam)
        {
            if(sParam == null || sParam == String.Empty )
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            UTF8Encoding utf8 = new UTF8Encoding();
            string str = String.Empty;

            try
            {
                byte[] enbytes = utf8.GetBytes(sParam);

                foreach (byte bt in enbytes)
                {
                    str += String.Format("%{0:X2}", bt); // 16진수로 소숫점 뒤 2자리 까지
                }
                str = str.ToUpper();
            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
                //throw;
            }

            return str;
        }

        public string Decode(string sParam)
        {
            if (sParam == null || sParam == String.Empty)
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            string str = string.Empty;

            try
            {
                byte[] debytes = HexStringToBytes(sParam);
                char[] chars = new char[debytes.Length];
                chars = Encoding.UTF8.GetChars(debytes);

                foreach (char c in chars)
                {
                    str += c;
                }
            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
                //throw;
            }

            return str;
        }

        private byte[] HexStringToBytes(string sParam)
        {
            string hexString = string.Empty;
            char c;

            for (int i = 0; i < sParam.Length; i++)
            {
                c = sParam[i];
                if (IsHexDigit(c))
                    hexString += c;
            }

            byte[] bytes = new byte[hexString.Length/2];

            string hex;

            for (int i = 0; i < bytes.Length; i++)
            {
                hex = hexString.Substring(i * 2, 2);

                bytes[i] = Convert.ToByte(hex, 16);
                //bytes[i] = byte.Parse(hex, System.Globalization.NumberStyles.HexNumber);
            }

            return bytes;
        }

        private bool IsHexDigit(char c)
        {
            c = char.ToUpper(c);
            int i = Convert.ToInt32(c);

            if (i >= 65 && i <= 70) //A~F
                return true;
            if (i >= 48 && i <= 57) //0~9
                return true;

            return false;

        }
    }
}

//http://www.csharpstudy.com/Tip/Tip-byte-bit.aspx