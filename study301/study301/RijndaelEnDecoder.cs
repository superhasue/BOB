using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Security.Cryptography;

namespace study301
{
    class RijndaelEnDecoder : IEnDecoder
    {
        public string key { get; set; }
        public string IV { get; set; }
        public string format { get; set; }

        public RijndaelEnDecoder()
        {
            key = string.Empty;
            IV = string.Empty;
            format = "Base64";
        }

        public RijndaelEnDecoder(string _key)
        {
            key = _key;
        }

        public RijndaelEnDecoder(string _key, string _IV)
        {
            key = _key;
            IV = _IV;
        }

        public void CreatingIV(string str)
        {
            char[] c1 = str.ToCharArray();
            Array.Reverse(c1);
            //String.ToCharArray - String 문자열을 char[] 으로 변경
            //Array.Reverse -  배열전체에 있는 요소의 시퀀스를 역순으로 설정

            //or

            char[] c2 = str.ToCharArray().Reverse().ToArray();
            //String.ToCharArray - String 문자열을 char[] 으로 변경
            //Enumerable.Reverse - 배열에서 요소의 순서를 바꿈.
            //Enumerable.ToArray - Enumerable 클래스의 확장 메서드는 IEnumerable 인터페이스를 반환한다. 하지만 코드를 작성하다보면 인터페이스가 아닌 구체화된 클래스가 필요할 때가 있다. 그 때 변환 연산자를 활용하여 Array 타입으로 변환할 수 있다.
            
            IV = new string(c1).Substring(0, 16);
        }

        public string Encode(string sParam)
        {
            if (string.IsNullOrEmpty(sParam))
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            if (key == String.Empty || key.Length != 16 && key.Length != 24 && key.Length != 32) //키길이는 16byte, 24byte, 32byte
                return sParam = "입력된 키가 올바르지 않습니다.";

            if (IV == String.Empty)
                CreatingIV(key);

            string str = String.Empty;

            //AES는 128 비트의 블록 크기에 대해서만 정의된 Rijndael의 축소 버전
            //Rijndael-256과 Rijndael-192는 AES (Rijndael-128)와 완전히 다른 알고리즘
            RijndaelManaged rijAES = new RijndaelManaged();
            rijAES.KeySize = 128; //AES128 -> 10라운드 
            rijAES.BlockSize = 128; //128bit -> IV의 길이는 16byte
            rijAES.Mode = CipherMode.CBC;
            rijAES.Padding = PaddingMode.PKCS7;

            rijAES.Key = Encoding.Default.GetBytes(key);
            rijAES.IV = Encoding.Default.GetBytes(IV);

            try
            {
                // 암호화는 스트림(바이트 배열)을 대칭키에 의존하여 암호화 하기때문에 먼저 메모리 스트림을 생성한다.
                MemoryStream ms = new MemoryStream();

                //만들어진 메모리 스트림을 이용해서 암호화 스트림 생성
                CryptoStream cs = new CryptoStream(ms, rijAES.CreateEncryptor(), CryptoStreamMode.Write);

                // 암호화 할 문자열를 바이트 배열로 변경
                byte[] data = Encoding.UTF8.GetBytes(sParam.ToCharArray());

                // 암호화 스트림에 데이터 씀
                cs.Write(data, 0, data.Length);
                cs.FlushFinalBlock();

                //Output Text Format
                if (format == "Base64")
                    str = Convert.ToBase64String(ms.ToArray());
                else if (format == "Hex")
                {
                    str = BitConverter.ToString(ms.ToArray());
                    str = str.Replace("-", "");
                } else { str = "포맷이 잘못되었습니다."; }

                cs.Close();
                ms.Close();

            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
            }


            return str;
        }

        public string Decode(string sParam)
        {
            if (string.IsNullOrEmpty(sParam))
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            if (key == String.Empty || key.Length != 16 && key.Length != 24 && key.Length != 32) //키길이는 16byte, 24byte, 32byte
                return sParam = "입력된 키가 올바르지 않습니다.";

            if (IV == String.Empty)
                CreatingIV(key);

            string str = String.Empty;

            RijndaelManaged rijAES = new RijndaelManaged();
            rijAES.KeySize = 128; //AES128 -> 10라운드 
            rijAES.BlockSize = 128; //128bit -> IV의 길이는 16byte
            rijAES.Mode = CipherMode.CBC;
            rijAES.Padding = PaddingMode.PKCS7;

            rijAES.Key = Encoding.Default.GetBytes(key);
            rijAES.IV = Encoding.Default.GetBytes(IV);

            try
            {
                //Input Text Format
                byte[] data;
                if (format == "Base64")
                    data = System.Convert.FromBase64String(sParam);
                else if (format == "Hex")
                {
                    data = new byte[sParam.Length / 2];
                    for (int i = 0; i < data.Length; i++)
                        data[i] = Convert.ToByte(sParam.Substring(i * 2, 2), 16);
                }else { str = "포맷이 잘못되었습니다."; return str; }

                MemoryStream ms = new MemoryStream(data);
                CryptoStream cs = new CryptoStream(ms, rijAES.CreateDecryptor(), CryptoStreamMode.Read);
                StreamReader sr = new StreamReader(cs);

                str = sr.ReadToEnd();

                cs.Close();
                sr.Close();
                ms.Close();
            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
            }

            return str;
        }

    }
}
